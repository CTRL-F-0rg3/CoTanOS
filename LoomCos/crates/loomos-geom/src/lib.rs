// crates/loomcos-geom/src/lib.rs

#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
pub struct LoomGeomPoint {
    pub x: f32,
    pub y: f32,
}

#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
pub struct LoomGeomRect {
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32,
}

#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum LoomShapeType {
    RoundedBox = 0,
    TitleBar = 1,
    ButtonClose = 2,
    ButtonMin = 3,
    ButtonMax = 4,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct LoomWindowBoxMetrics {
    pub titlebar_height: f32,
    pub border_width: f32,
    pub corner_radius: f32,
    pub button_size: f32,
    pub button_spacing: f32,
}

impl LoomGeomRect {
    pub fn new(x: f32, y: f32, width: f32, height: f32) -> Self {
        Self { x, y, width, height }
    }

    pub fn right(&self) -> f32 {
        self.x + self.width
    }

    pub fn bottom(&self) -> f32 {
        self.y + self.height
    }

    pub fn contains(&self, px: f32, py: f32) -> bool {
        px >= self.x && px < self.right() && py >= self.y && py < self.bottom()
    }

    pub fn intersects(&self, other: &LoomGeomRect) -> bool {
        self.x < other.right() && self.right() > other.x &&
        self.y < other.bottom() && self.bottom() > other.y
    }

    pub fn intersection(&self, other: &LoomGeomRect) -> Option<LoomGeomRect> {
        if !self.intersects(other) {
            return None;
        }
        let x = self.x.max(other.x);
        let y = self.y.max(other.y);
        let right = self.right().min(other.right());
        let bottom = self.bottom().min(other.bottom());

        Some(LoomGeomRect::new(x, y, right - x, bottom - y))
    }

    pub fn union(&self, other: &LoomGeomRect) -> LoomGeomRect {
        let x = self.x.min(other.x);
        let y = self.y.min(other.y);
        let right = self.right().max(other.right());
        let bottom = self.bottom().max(other.bottom());

        LoomGeomRect::new(x, y, right - x, bottom - y)
    }
}

// --- C-ABI FFI Exports ---

#[no_mangle]
pub extern "C" fn loom_geom_rect_contains_point(rect: *const LoomGeomRect, px: f32, py: f32) -> bool {
    if rect.is_null() {
        return false;
    }
    unsafe { (*rect).contains(px, py) }
}

#[no_mangle]
pub extern "C" fn loom_geom_rect_intersects(a: *const LoomGeomRect, b: *const LoomGeomRect) -> bool {
    if a.is_null() || b.is_null() {
        return false;
    }
    unsafe { (*a).intersects(&*b) }
}

#[no_mangle]
pub extern "C" fn loom_geom_rect_intersection(
    a: *const LoomGeomRect, 
    b: *const LoomGeomRect, 
    out_result: *mut LoomGeomRect
) -> bool {
    if a.is_null() || b.is_null() || out_result.is_null() {
        return false;
    }
    unsafe {
        if let Some(res) = (*a).intersection(&*b) {
            *out_result = res;
            true
        } else {
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn loom_geom_rect_union(
    a: *const LoomGeomRect, 
    b: *const LoomGeomRect, 
    out_result: *mut LoomGeomRect
) {
    if a.is_null() || b.is_null() || out_result.is_null() {
        return;
    }
    unsafe {
        *out_result = (*a).union(&*b);
    }
}

#[no_mangle]
pub extern "C" fn loom_geom_calc_titlebar(
    window_bounds: *const LoomGeomRect,
    metrics: *const LoomWindowBoxMetrics,
    out_titlebar: *mut LoomGeomRect
) {
    if window_bounds.is_null() || metrics.is_null() || out_titlebar.is_null() {
        return;
    }
    unsafe {
        let wb = &*window_bounds;
        let m = &*metrics;
        // Titlebar sits at the top of the window, accounting for borders
        let x = wb.x + m.border_width;
        let y = wb.y + m.border_width;
        let width = (wb.width - (m.border_width * 2.0)).max(0.0);
        let height = m.titlebar_height;
        *out_titlebar = LoomGeomRect::new(x, y, width, height);
    }
}

#[no_mangle]
pub extern "C" fn loom_geom_calc_button(
    titlebar_bounds: *const LoomGeomRect,
    metrics: *const LoomWindowBoxMetrics,
    btn_type: LoomShapeType,
    out_button: *mut LoomGeomRect
) {
    if titlebar_bounds.is_null() || metrics.is_null() || out_button.is_null() {
        return;
    }
    unsafe {
        let tb = &*titlebar_bounds;
        let m = &*metrics;

        // Calculate button index from right side (Close is rightmost, then Max, then Min)
        let index_from_right = match btn_type {
            LoomShapeType::ButtonClose => 1.0,
            LoomShapeType::ButtonMax => 2.0,
            LoomShapeType::ButtonMin => 3.0,
            _ => 1.0,
        };

        let btn_w = m.button_size;
        let btn_h = m.button_size.min(tb.height - 4.0);
        
        // Vertically center the button inside the titlebar
        let y = tb.y + (tb.height - btn_h) * 0.5;
        // Align buttons from the right edge of the titlebar
        let total_offset = index_from_right * (btn_w + m.button_spacing);
        let x = tb.right() - total_offset - m.button_spacing;

        *out_button = LoomGeomRect::new(x, y, btn_w, btn_h);
    }
}