#include "base/cotan_shim.h"
#include "base/display.h"

#define VGA_FB_PHYS_ADDR 0xfd000000

static framebuffer_t vga_fb;
static int vga_initialized = 0;

int qemu_vga_init(unsigned int width, unsigned int height) {
    if (vga_initialized) return 0;

    vga_fb.width = width;
    vga_fb.height = height;
    vga_fb.format = PIXEL_FORMAT_XRGB8888;
    vga_fb.stride = width * 4;
    vga_fb.size = (size_t)(vga_fb.stride * height);
    vga_fb.phys_addr = VGA_FB_PHYS_ADDR;
    vga_fb.virt_addr = (void*)VGA_FB_PHYS_ADDR;

    printf("[vga] Framebuffer initialized: %ux%u @ phys=0x%lx, size=%u bytes\n",
           width, height, (unsigned long)vga_fb.phys_addr, (unsigned int)vga_fb.size);
    
    vga_initialized = 1;
    return 0;
}

void qemu_vga_draw_test_pattern(void) {
    if (!vga_initialized) return;

    unsigned int *fb = (unsigned int *)vga_fb.virt_addr;
    unsigned int width = vga_fb.width;
    unsigned int height = vga_fb.height;
    unsigned int bar_width = width / 8;
    
    unsigned int colors[8] = {
        0x00000000, 0x000000FF, 0x0000FF00, 0x0000FFFF,
        0x00FF0000, 0x00FF00FF, 0x00FFFF00, 0x00FFFFFF
    };

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            unsigned int color_idx = x / bar_width;
            if (color_idx > 7) color_idx = 7;
            fb[y * width + x] = colors[color_idx];
        }
    }
    printf("[vga] Test pattern drawn.\n");
}
