/*
 * Display device wrapper functions for CoTanOS
 */

#include "base/display.h"

/* Initialize display device */
int display_init(display_device_t *dev) {
    if (!dev || !dev->ops) {
        return -1;
    }
    
    /* Device-specific initialization can be added here */
    return 0;
}

/* Set display mode */
int display_set_mode(display_device_t *dev, const display_mode_t *mode) {
    if (!dev || !dev->ops || !dev->ops->set_mode) {
        return -1;
    }
    
    return dev->ops->set_mode(dev->priv, mode);
}

/* Create framebuffer */
int display_create_fb(display_device_t *dev, uint32_t width, uint32_t height,
                     pixel_format_t format, framebuffer_t *fb) {
    if (!dev || !dev->ops || !dev->ops->create_framebuffer) {
        return -1;
    }
    
    return dev->ops->create_framebuffer(dev->priv, width, height, format, fb);
}

/* Page flip */
int display_page_flip(display_device_t *dev, const framebuffer_t *fb) {
    if (!dev || !dev->ops || !dev->ops->page_flip) {
        return -1;
    }
    
    return dev->ops->page_flip(dev->priv, fb);
}

/* Wait for vertical blank */
int display_wait_vblank(display_device_t *dev) {
    if (!dev || !dev->ops || !dev->ops->wait_vblank) {
        return -1;
    }
    
    return dev->ops->wait_vblank(dev->priv);
}
