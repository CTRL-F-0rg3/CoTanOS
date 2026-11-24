/*
 * Minimal DRM structures for CoTanOS
 * Stub implementations to satisfy Linux DRM code dependencies
 */

#ifndef _DRM_STUB_H
#define _DRM_STUB_H

#include "cotan_shim.h"

/* Forward declarations */
struct drm_device;
struct drm_connector;
struct drm_display_mode;

/* Display mode structure (simplified from Linux) */
struct drm_display_mode {
    /* Source of mode */
    enum type {
        DRM_MODE_TYPE_BUILTIN = 0,
        DRM_MODE_TYPE_CLOCK_C,
        DRM_MODE_TYPE_CRTC_C,
        DRM_MODE_TYPE_PREFERRED,
        DRM_MODE_TYPE_DEFAULT,
        DRM_MODE_TYPE_USERDEF,
        DRM_MODE_TYPE_DRIVER,
    } type;
    
    /* Display info */
    char name[32];
    int mode_flags;
    
    /* Horizontal timing */
    int hdisplay;
    int hsync_start;
    int hsync_end;
    int htotal;
    int hskew;
    
    /* Vertical timing */
    int vdisplay;
    int vsync_start;
    int vsync_end;
    int vtotal;
    int vscan;
    
    /* Timing info */
    unsigned int clock; /* kHz */
    unsigned int vrefresh; /* Hz */
    
    /* Aspect ratio */
    enum hdmi_picture_aspect {
        HDMI_PICTURE_ASPECT_NONE = 0,
        HDMI_PICTURE_ASPECT_4_3,
        HDMI_PICTURE_ASPECT_16_9,
        HDMI_PICTURE_ASPECT_64_27,
        HDMI_PICTURE_ASPECT_256_135,
    } picture_aspect;
    
    /* List head */
    struct list_head head;
};

/* Mode flags */
#define DRM_MODE_FLAG_PHSYNC			(1<<0)
#define DRM_MODE_FLAG_NHSYNC			(1<<1)
#define DRM_MODE_FLAG_PVSYNC			(1<<2)
#define DRM_MODE_FLAG_NVSYNC			(1<<3)
#define DRM_MODE_FLAG_INTERLACE			(1<<4)
#define DRM_MODE_FLAG_DBLSCAN			(1<<5)
#define DRM_MODE_FLAG_CSYNC			(1<<6)
#define DRM_MODE_FLAG_PCSYNC			(1<<7)
#define DRM_MODE_FLAG_NCSYNC			(1<<8)
#define DRM_MODE_FLAG_HSKEW			(1<<9)
#define DRM_MODE_FLAG_BCAST			(1<<10)
#define DRM_MODE_FLAG_PIXMUX			(1<<11)
#define DRM_MODE_FLAG_DBLCLK			(1<<12)
#define DRM_MODE_FLAG_CLKDIV2			(1<<13)

/* EDID structure */
struct edid {
    uint8_t header[8];
    uint8_t mfg_id[2];
    uint8_t prod_code[2];
    uint32_t serial_number;
    uint8_t mfg_week;
    uint8_t mfg_year;
    uint8_t version;
    uint8_t revision;
    /* ... more fields ... */
} __attribute__((packed));

/* Helper functions */
static inline bool drm_mode_equal(const struct drm_display_mode *m1,
                                  const struct drm_display_mode *m2) {
    return m1->hdisplay == m2->hdisplay &&
           m1->vdisplay == m2->vdisplay &&
           m1->clock == m2->clock;
}

static inline void drm_mode_set_name(struct drm_display_mode *mode) {
    snprintf(mode->name, sizeof(mode->name), "%dx%d",
             mode->hdisplay, mode->vdisplay);
}

#endif /* _DRM_STUB_H */
