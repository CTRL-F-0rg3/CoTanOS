// crates/loomcos-cpu/src/lib.rs

use std::os::raw::{c_int, c_void};

extern "C" {
    pub fn loom_asm_fast_fill(dest: *mut u32, color: u32, count: usize);
    pub fn loom_asm_fast_copy_nt(dest: *mut u32, src: *const u32, dword_count: usize);
    pub fn loom_cpu_accelerated_blend(
        dest: *mut u32,
        dest_w: c_int,
        dest_h: c_int,
        src: *const u32,
        src_w: c_int,
        src_h: c_int,
        pos_x: c_int,
        pos_y: c_int,
    );

    // Noise-driven stochastic scheduler bindings
    pub fn loom_noise_scheduler_create(screen_w: c_int, screen_h: c_int, tile_size: c_int) -> *mut c_void;
    pub fn loom_noise_scheduler_destroy(scheduler: *mut c_void);
    pub fn loom_noise_update_activity_map(scheduler: *mut c_void, current_fb: *const u32, previous_fb: *const u32);
    pub fn loom_noise_sparse_blit_pass(scheduler: *mut c_void, dest_fb: *mut u32, source_fb: *const u32, frame_counter: u32);
}

pub struct NoiseTileRenderer {
    width: i32,
    height: i32,
    scheduler: *mut c_void,
    framebuffer: Vec<u32>,
    previous_framebuffer: Vec<u32>,
    frame_counter: u32,
}

impl NoiseTileRenderer {
    pub fn new(width: i32, height: i32, tile_size: i32) -> Self {
        let size = (width * height) as usize;
        let scheduler = unsafe { loom_noise_scheduler_create(width, height, tile_size) };
        Self {
            width,
            height,
            scheduler,
            framebuffer: vec![0; size],
            previous_framebuffer: vec![0; size],
            frame_counter: 0,
        }
    }

    pub fn execute_noise_sparse_pass(&mut self, source: &[u32]) {
        self.frame_counter = self.frame_counter.wrapping_add(1);
        
        unsafe {
            // Update activity map comparing current screen state against previous frame
            loom_noise_update_activity_map(
                self.scheduler,
                source.as_ptr(),
                self.previous_framebuffer.as_ptr(),
            );

            // Execute non-linear noise sparse refresh pass
            loom_noise_sparse_blit_pass(
                self.scheduler,
                self.framebuffer.as_mut_ptr(),
                source.as_ptr(),
                self.frame_counter,
            );
        }

        // Copy current to previous for the next frame delta check
        self.previous_framebuffer.copy_from_slice(source);
    }

    pub fn framebuffer(&self) -> &[u32] {
        &self.framebuffer
    }
}

impl Drop for NoiseTileRenderer {
    fn drop(&mut self) {
        if !self.scheduler.is_null() {
            unsafe {
                loom_noise_scheduler_destroy(self.scheduler);
            }
        }
    }
}