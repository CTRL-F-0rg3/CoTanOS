/*
 * Minimal libdrm implementation for CoTanOS userspace
 * Supports all symbols required by Mesa virgl driver.
 */

#define _GNU_SOURCE
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <drm/drm.h>
#include <drm/virtgpu_drm.h>

#ifndef SYS_virtgpu_ioctl
#define SYS_virtgpu_ioctl 400
#endif

/* Define drmDevice struct (normally in xf86drm.h) */
struct drmDevice {
    char *name;
    char *bus_id;
    int fd;
    uint64_t available_nodes;
    char *nodes[8];
};

/* ===== Core ioctl wrapper ===== */
int drmIoctl(int fd, unsigned long request, void *arg) {
    return syscall(SYS_virtgpu_ioctl, fd, request, arg);
}

int drmCommandWriteRead(int fd, unsigned long drmCommandIndex, 
                        void *data, unsigned long size) {
    (void)fd; (void)drmCommandIndex; (void)data; (void)size;
    return -1; 
}

int drmCommandWrite(int fd, unsigned long drmCommandIndex, void *data, unsigned long size) {
    return drmIoctl(fd, _IOC(_IOC_WRITE, DRM_IOCTL_BASE, drmCommandIndex, size), data);
}

int drmCommandRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size) {
    return drmIoctl(fd, _IOC(_IOC_READ, DRM_IOCTL_BASE, drmCommandIndex, size), data);
}

/* ===== Version functions ===== */
struct drm_version *drmGetVersion(int fd) {
    struct drm_version *v = calloc(1, sizeof(*v));
    if (!v) return NULL;
    v->version_major = 1;
    v->version_minor = 0;
    v->version_patchlevel = 0;
    v->name_len = 6;
    v->name = strdup("virgl");
    v->date_len = 11;
    v->date = strdup("20240101");
    v->desc_len = 20;
    v->desc = strdup("CoTanOS virtio-gpu");
    (void)fd;
    return v;
}

void drmFreeVersion(struct drm_version *version) {
    if (version) {
        free(version->name);
        free(version->date);
        free(version->desc);
        free(version);
    }
}

/* ===== Device functions ===== */
int drmGetDevice2(int fd, uint32_t flags, struct drmDevice **device) {
    struct drmDevice *d = calloc(1, sizeof(*d));
    if (!d) return -1;
    d->fd = fd;
    d->name = strdup("virgl");
    d->bus_id = strdup("virtio:00:00.0");
    *device = d;
    (void)flags;
    return 0;
}

int drmGetDevice(int fd, struct drmDevice **device) {
    return drmGetDevice2(fd, 0, device);
}

void drmFreeDevice(struct drmDevice **device) {
    if (device && *device) {
        free((*device)->name);
        free((*device)->bus_id);
        free(*device);
        *device = NULL;
    }
}

int drmGetDevices2(uint32_t flags, struct drmDevice **devices, unsigned int max_devices) {
    (void)flags;
    if (max_devices > 0 && devices) {
        struct drmDevice *d = calloc(1, sizeof(*d));
        if (!d) return 0;
        d->name = strdup("virgl");
        d->bus_id = strdup("virtio:00:00.0");
        devices[0] = d;
        return 1;
    }
    return 0;
}

int drmGetDevices(struct drmDevice **devices, unsigned int max_devices) {
    return drmGetDevices2(0, devices, max_devices);
}

int drmDevicesEqual(const struct drmDevice *a, const struct drmDevice *b) {
    if (!a || !b) return 0;
    if (a->fd == b->fd) return 1;
    if (a->bus_id && b->bus_id && strcmp(a->bus_id, b->bus_id) == 0) return 1;
    return 0;
}

char *drmGetRenderDeviceNameFromFd(int fd) {
    (void)fd;
    return strdup("/dev/dri/renderD128");
}

char *drmGetPrimaryDeviceNameFromFd(int fd) {
    (void)fd;
    return strdup("/dev/dri/card0");
}

/* ===== PRIME functions ===== */
int drmPrimeHandleToFD(int fd, uint32_t handle, uint32_t flags, int *prime_fd) {
    (void)fd; (void)handle; (void)flags;
    *prime_fd = -1;
    return -1;
}

int drmPrimeFDToHandle(int fd, int prime_fd, uint32_t *handle) {
    (void)fd; (void)prime_fd;
    *handle = 0;
    return -1;
}

int drmCloseBufferHandle(int fd, uint32_t handle) {
    (void)fd; (void)handle;
    return 0;
}

/* ===== Cap functions ===== */
int drmGetCap(int fd, uint64_t capability, uint64_t *value) {
    (void)fd; (void)capability;
    *value = 0;
    return 0;
}

int drmSetClientCap(int fd, uint64_t capability, uint64_t value) {
    (void)fd; (void)capability; (void)value;
    return 0;
}

/* ===== GEM functions ===== */
int drmGemClose(int fd, uint32_t handle) {
    (void)fd; (void)handle;
    return 0;
}

int drmGemFlink(int fd, uint32_t handle, uint32_t *name) {
    (void)fd; (void)handle;
    *name = 0;
    return -1;
}

/* ===== Auth functions ===== */
int drmAuthMagic(int fd, uint32_t magic) {
    (void)fd; (void)magic;
    return 0;
}

int drmGetMagic(int fd, uint32_t *magic) {
    (void)fd;
    *magic = 0x12345678;
    return 0;
}

/* ===== Open/Close wrappers ===== */
int drmOpen(const char *name, const char *busid) {
    (void)name; (void)busid;
    return -1;
}

void drmClose(int fd) {
    close(fd);
}

/* ===== Stub helpers ===== */
int drmSetMaster(int fd) { (void)fd; return 0; }
int drmDropMaster(int fd) { (void)fd; return 0; }
int drmIsMaster(int fd) { (void)fd; return 1; }

int drmModeGetResources(int fd) { (void)fd; return 0; }
void drmModeFreeResources(void *r) { (void)r; }
