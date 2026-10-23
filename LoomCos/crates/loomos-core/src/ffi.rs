// crates/loomcos-core/src/ffi.rs
use std::os::raw::c_int;

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct LoomcosRect {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub height: i32,
}

extern "C" {
    pub fn loomcos_cpu_blend_surface(
        dest: *mut u32, dest_w: c_int, dest_h: c_int,
        src: *const u32, src_w: c_int, src_h: c_int,
        pos_x: c_int, pos_y: c_int, global_alpha: u8,
    );

    pub fn loomcos_cpu_blend_selective(
        dest: *mut u32, dest_w: c_int, dest_h: c_int,
        src: *const u32, src_w: c_int, src_h: c_int,
        pos_x: c_int, pos_y: c_int, color_key: u32, alpha_threshold: u8,
    );

    // Added: Clear specific regions with a solid color
    pub fn loom_cpu_fill_solid_rect(
        dest: *mut u32, dest_w: c_int, dest_h: c_int,
        rect_x: c_int, rect_y: c_int, rect_w: c_int, rect_h: c_int,
        color: u32,
    );
}