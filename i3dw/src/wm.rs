//! A floating window manager, KDE-Plasma style.
//!
//! Windows float where they are placed (Plasma defaults to "floating", unlike
//! the `allde` niri-like tiling), stack in a z-order, can be focused, raised,
//! minimized, maximized and dragged. Focusing a window raises it, which is the
//! one behaviour every floating desktop shares.

pub type WindowId = u32;

/// A rectangular region of the screen.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Rect {
    pub x: i32,
    pub y: i32,
    pub w: u32,
    pub h: u32,
}

/// A window owned by the desktop.
pub struct Window {
    pub id: WindowId,
    pub title: String,
    pub x: i32,
    pub y: i32,
    pub w: u32,
    pub h: u32,
    /// Z order; higher is closer to the viewer.
    pub z: i32,
    pub minimized: bool,
    pub maximized: bool,
    /// The pre-maximize geometry, restored on un-maximize.
    pub restore: Option<Rect>,
}

impl Window {
    fn new(id: WindowId, title: impl Into<String>, x: i32, y: i32, w: u32, h: u32, z: i32) -> Self {
        Self {
            id,
            title: title.into(),
            x,
            y,
            w,
            h,
            z,
            minimized: false,
            maximized: false,
            restore: None,
        }
    }
}

/// The floating window manager: z-ordered windows with focus and min/max.
pub struct Wm {
    pub screen_w: u32,
    pub screen_h: u32,
    windows: Vec<Window>,
    focused: Option<WindowId>,
    next_id: u32,
    next_z: i32,
    /// Cascade step between successive windows, so they overlap visibly.
    cascade: i32,
}

impl Wm {
    pub fn new(screen_w: u32, screen_h: u32) -> Self {
        Self {
            screen_w,
            screen_h,
            windows: Vec::new(),
            focused: None,
            next_id: 1,
            next_z: 1,
            cascade: 24,
        }
    }

    /// Add a window, placed in a cascade so overlapping windows stay draggable.
    pub fn add(&mut self, title: impl Into<String>, w: u32, h: u32) -> WindowId {
        let id = self.next_id;
        self.next_id += 1;

        let n = self.windows.len() as i32;
        let margin = 40;
        let max_x = (self.screen_w.saturating_sub(w)) as i32 - margin;
        let max_y = (self.screen_h.saturating_sub(h)) as i32 - margin;
        let x = (margin + n * self.cascade).min(max_x.max(0));
        let y = (margin + n * self.cascade).min(max_y.max(0));

        let z = self.next_z;
        self.next_z += 1;

        self.windows.push(Window::new(id, title, x, y, w, h, z));
        self.focused = Some(id);
        id
    }

    pub fn remove(&mut self, id: WindowId) {
        self.windows.retain(|w| w.id != id);
        if self.focused == Some(id) {
            self.focused = self.windows.iter().max_by_key(|w| w.z).map(|w| w.id);
        }
    }

    /// Focus a window and raise it to the top.
    pub fn focus(&mut self, id: WindowId) {
        if self.windows.iter().any(|w| w.id == id) {
            self.focused = Some(id);
            self.raise(id);
        }
    }

    /// Raise a window above every other.
    pub fn raise(&mut self, id: WindowId) {
        if let Some(w) = self.windows.iter_mut().find(|w| w.id == id) {
            w.z = self.next_z;
            self.next_z += 1;
        }
    }

    pub fn focused(&self) -> Option<WindowId> {
        self.focused
    }

    /// Minimize: the window disappears until restored.
    pub fn minimize(&mut self, id: WindowId) {
        if let Some(w) = self.windows.iter_mut().find(|w| w.id == id) {
            w.minimized = true;
        }
        if self.focused == Some(id) {
            self.focused = self
                .windows
                .iter()
                .filter(|w| !w.minimized)
                .max_by_key(|w| w.z)
                .map(|w| w.id);
        }
    }

    /// Restore a minimized window and raise it.
    pub fn restore(&mut self, id: WindowId) {
        if let Some(w) = self.windows.iter_mut().find(|w| w.id == id) {
            w.minimized = false;
        }
        self.focus(id);
    }

    /// Toggle maximize. Maximized windows fill the work area (screen minus the
    /// panel), which the desktop reserves at the bottom.
    pub fn toggle_maximize(&mut self, id: WindowId, panel_h: u32) {
        if let Some(w) = self.windows.iter_mut().find(|w| w.id == id) {
            if w.maximized {
                if let Some(r) = w.restore.take() {
                    w.x = r.x;
                    w.y = r.y;
                    w.w = r.w;
                    w.h = r.h;
                }
                w.maximized = false;
            } else {
                w.restore = Some(Rect { x: w.x, y: w.y, w: w.w, h: w.h });
                w.x = 0;
                w.y = 0;
                w.w = self.screen_w;
                w.h = self.screen_h.saturating_sub(panel_h);
                w.maximized = true;
            }
        }
        self.focus(id);
    }

    /// Move a window's top-left corner.
    pub fn move_to(&mut self, id: WindowId, x: i32, y: i32) {
        if let Some(w) = self.windows.iter_mut().find(|w| w.id == id) {
            w.x = x;
            w.y = y;
        }
    }

    /// Resize a window (keeps the top-left corner).
    pub fn resize(&mut self, id: WindowId, w: u32, h: u32) {
        if let Some(win) = self.windows.iter_mut().find(|win| win.id == id) {
            win.w = w.max(1);
            win.h = h.max(1);
        }
    }

    /// The effective geometry of a window (a maximized window fills the work
    /// area regardless of its stored geometry).
    fn effective(&self, w: &Window, panel_h: u32) -> Rect {
        if w.maximized {
            Rect {
                x: 0,
                y: 0,
                w: self.screen_w,
                h: self.screen_h.saturating_sub(panel_h),
            }
        } else {
            Rect { x: w.x, y: w.y, w: w.w, h: w.h }
        }
    }

    /// Visible windows, back-to-front (z ascending), with effective geometry.
    pub fn layout(&self, panel_h: u32) -> Vec<(WindowId, Rect)> {
        let mut visible: Vec<&Window> = self.windows.iter().filter(|w| !w.minimized).collect();
        visible.sort_by_key(|w| w.z);
        visible
            .iter()
            .map(|w| (w.id, self.effective(w, panel_h)))
            .collect()
    }

    /// All windows in creation order (for the panel task list).
    pub fn windows(&self) -> impl Iterator<Item = &Window> {
        self.windows.iter()
    }

    pub fn count(&self) -> usize {
        self.windows.len()
    }

    /// The topmost visible window under `(x, y)`, if any.
    pub fn window_at(&self, x: u32, y: u32, panel_h: u32) -> Option<WindowId> {
        let mut visible: Vec<&Window> = self.windows.iter().filter(|w| !w.minimized).collect();
        visible.sort_by_key(|w| std::cmp::Reverse(w.z));
        for w in visible {
            let r = self.effective(w, panel_h);
            if x >= r.x as u32
                && x < (r.x as u32 + r.w)
                && y >= r.y as u32
                && y < (r.y as u32 + r.h)
            {
                return Some(w.id);
            }
        }
        None
    }

    /// The window id at the given panel task index (creation order), if any.
    pub fn window_at_index(&self, index: usize) -> Option<WindowId> {
        self.windows.get(index).map(|w| w.id)
    }
}

