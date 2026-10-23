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
    // Existing render helpers
    pub fn loomcos_cpu_blit_clipped(
        dest: *mut u32,
        dest_w: c_int,
        dest_h: c_int,
        src: *const u32,
        src_w: c_int,
        src_h: c_int,
        pos_x: c_int,
        pos_y: c_int,
        clip: *const LoomcosRect,
    );

    // New Alpha Blending functions from alpha_blend.cpp
    pub fn loomcos_cpu_blend_surface(
        dest: *mut u32,
        dest_w: c_int,
        dest_h: c_int,
        src: *const u32,
        src_w: c_int,
        src_h: c_int,
        pos_x: c_int,
        pos_y: c_int,
        global_alpha: u8,
    );

    pub fn loomcos_cpu_blend_selective(
        dest: *mut u32,
        dest_w: c_int,
        dest_h: c_int,
        src: *const u32,
        src_w: c_int,
        src_h: c_int,
        pos_x: c_int,
        pos_y: c_int,
        color_key: u32,
        alpha_threshold: u8,
    );
}