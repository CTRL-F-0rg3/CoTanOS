//! FFI do logiki DE zaimplementowanej w C++ (`src/logic/de_logic.cpp`).
//!
//! Rust trzyma wyłącznie nieprzezroczysty wskaźnik `TlDe*` i woła funkcje C.
//! Wszystkie typy na granicy są `#[repr(C)]` i składają się z samych
//! `i32`/`u32`, więc nie ma ryzyka rozjazdu ABI między Rust a C++.

use std::ffi::CString;
use std::os::raw::c_char;

use crate::geometry::Rect;
use crate::window::{Placed, WindowId, WindowKind};

/// Nieprzezroczysty handle do obiektu logiki w C++.
#[repr(C)]
struct TlDe {
    _opaque: [u8; 0],
}

/// Odpowiednik [`Placed`] na granicy C.
#[repr(C)]
struct TlPlaced {
    id: u32,
    kind: i32,
    x: i32,
    y: i32,
    w: u32,
    h: u32,
}

extern "C" {
    fn tl_de_new() -> *mut TlDe;
    fn tl_de_free(de: *mut TlDe);
    fn tl_spawn_tiled(de: *mut TlDe, title: *const c_char) -> u32;
    fn tl_spawn_floating(de: *mut TlDe, title: *const c_char, x: i32, y: i32, w: u32, h: u32) -> u32;
    fn tl_close_window(de: *mut TlDe, id: u32);
    fn tl_focus(de: *mut TlDe, id: u32);
    fn tl_focus_next(de: *mut TlDe);
    fn tl_focus_prev(de: *mut TlDe);
    fn tl_focused(de: *const TlDe) -> u32;
    fn tl_switch_workspace(de: *mut TlDe, index: u32);
    fn tl_move_window_to_workspace(de: *mut TlDe, id: u32, index: u32);
    fn tl_workspace_count(de: *const TlDe) -> u32;
    fn tl_layout(de: *const TlDe, ax: i32, ay: i32, aw: u32, ah: u32, gap: u32, out: *mut TlPlaced, max: u32) -> u32;
    fn tl_window_title(de: *const TlDe, id: u32, buf: *mut c_char, buf_len: u32) -> u32;
    fn tl_window_count(de: *const TlDe) -> u32;
}

/// Bezpieczne opakowanie logiki C++.
pub struct DeLogic {
    handle: *mut TlDe,
}

impl DeLogic {
    pub fn new() -> Self {
        Self { handle: unsafe { tl_de_new() } }
    }

    pub fn spawn_tiled(&mut self, title: &str) -> WindowId {
        let c = CString::new(title).unwrap_or_default();
        unsafe { tl_spawn_tiled(self.handle, c.as_ptr()) }
    }

    pub fn spawn_floating(&mut self, title: &str, rect: Rect) -> WindowId {
        let c = CString::new(title).unwrap_or_default();
        unsafe { tl_spawn_floating(self.handle, c.as_ptr(), rect.x, rect.y, rect.w, rect.h) }
    }

    pub fn close_window(&mut self, id: WindowId) {
        unsafe { tl_close_window(self.handle, id) }
    }

    pub fn focus(&mut self, id: WindowId) {
        unsafe { tl_focus(self.handle, id) }
    }

    pub fn focus_next(&mut self) {
        unsafe { tl_focus_next(self.handle) }
    }

    pub fn focus_prev(&mut self) {
        unsafe { tl_focus_prev(self.handle) }
    }

    pub fn focused(&self) -> Option<WindowId> {
        match unsafe { tl_focused(self.handle) } {
            0 => None,
            id => Some(id),
        }
    }

    pub fn switch_workspace(&mut self, index: usize) {
        unsafe { tl_switch_workspace(self.handle, index as u32) }
    }

    pub fn move_window_to_workspace(&mut self, id: WindowId, index: usize) {
        unsafe { tl_move_window_to_workspace(self.handle, id, index as u32) }
    }

    pub fn workspace_count(&self) -> usize {
        unsafe { tl_workspace_count(self.handle) as usize }
    }

    pub fn layout(&self, area: Rect, gap: u32) -> Vec<Placed> {
        // Dwa wywołania: najpierw liczba elementów (out = null), potem
        // wypełnienie bufora. C++ gwarantuje, że drugie wywołanie zapisze
        // dokładnie `count` elementów `#[repr(C)]` bez paddingu.
        let count = unsafe {
            tl_layout(self.handle, area.x, area.y, area.w, area.h, gap, std::ptr::null_mut(), 0)
        };
        if count == 0 {
            return Vec::new();
        }
        let mut raw: Vec<TlPlaced> = Vec::with_capacity(count as usize);
        unsafe {
            tl_layout(self.handle, area.x, area.y, area.w, area.h, gap, raw.as_mut_ptr(), count);
            raw.set_len(count as usize);
        }
        raw.into_iter()
            .map(|p| Placed {
                id: p.id,
                kind: if p.kind == 0 { WindowKind::Tiled } else { WindowKind::Floating },
                rect: Rect::new(p.x, p.y, p.w, p.h),
            })
            .collect()
    }

    pub fn window_title(&self, id: WindowId) -> String {
        let len = unsafe { tl_window_title(self.handle, id, std::ptr::null_mut(), 0) };
        if len == 0 {
            return String::new();
        }
        let mut buf = vec![0u8; len as usize + 1];
        unsafe {
            tl_window_title(self.handle, id, buf.as_mut_ptr() as *mut c_char, buf.len() as u32);
        }
        let end = buf.iter().position(|&b| b == 0).unwrap_or(buf.len());
        String::from_utf8_lossy(&buf[..end]).into_owned()
    }

    pub fn window_count(&self) -> usize {
        unsafe { tl_window_count(self.handle) as usize }
    }
}

impl Default for DeLogic {
    fn default() -> Self {
        Self::new()
    }
}

impl Drop for DeLogic {
    fn drop(&mut self) {
        unsafe { tl_de_free(self.handle) }
    }
}
