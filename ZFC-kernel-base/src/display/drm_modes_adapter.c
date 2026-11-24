/*
 * VESA Standard Display Modes Generator
 * Adapted from Linux DRM (drivers/gpu/drm/drm_modes.c)
 */

#include "base/cotan_shim.h"
#include "base/display.h"

struct vesa_mode_db {
    uint16_t hactive, vactive;
    uint32_t clock_khz;
    uint16_t hsync_start, hsync_end, htotal;
    uint16_t vsync_start, vsync_end, vtotal;
    uint32_t flags;
};

static const struct vesa_mode_db vesa_60hz_modes[] = {
    { 640,  480,  25175,  656,  752,  800,  490,  492,  525, 0 },
    { 800,  600,  40000,  840,  968, 1056,  601,  605,  628, 0 },
    { 1024, 768,  65000, 1048, 1184, 1344, 771,  777,  806, 0 },
    { 1280, 720,  74250, 1390, 1430, 1650, 750,  755,  750, 0 },
    { 1280, 1024, 108000, 1328, 1440, 1688, 1038, 1043, 1066, 0 },
    { 1920, 1080, 148500, 2008, 2052, 2200, 1084, 1089, 1125, 0 },
};

#define VESA_60HZ_MODES_COUNT (sizeof(vesa_60hz_modes) / sizeof(vesa_60hz_modes[0]))

int drm_add_standard_modes(edid_t *edid) {
    size_t i;
    display_mode_t *new_modes;
    size_t current_count = edid->mode_count;
    size_t new_count = current_count + VESA_60HZ_MODES_COUNT;

    new_modes = (display_mode_t *)realloc(edid->modes, new_count * sizeof(display_mode_t));
    if (!new_modes) return -1;
    edid->modes = new_modes;

    for (i = 0; i < VESA_60HZ_MODES_COUNT; i++) {
        const struct vesa_mode_db *v = &vesa_60hz_modes[i];
        display_mode_t *m = &edid->modes[current_count + i];

        m->hactive = v->hactive;
        m->vactive = v->vactive;
        m->htotal = v->htotal;
        m->vtotal = v->vtotal;
        m->hsync_start = v->hsync_start;
        m->hsync_end = v->hsync_end;
        m->vsync_start = v->vsync_start;
        m->vsync_end = v->vsync_end;
        m->pixel_clock_khz = v->clock_khz;
        m->refresh_rate = 60;
        m->flags = v->flags;

        snprintf(m->name, sizeof(m->name), "%ux%u@60", v->hactive, v->vactive);
    }

    edid->mode_count = new_count;
    if (!edid->preferred_mode && edid->mode_count > 2) {
        edid->preferred_mode = &edid->modes[2];
    }
    return 0;
}
