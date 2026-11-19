/*
 * ============================================================================
 *  STOR_PCI.C - PCIe config space access & device enumeration
 *  Architecture: RISC-V (RV32 / RV64)
 *
 *  Block 1:  ECAM backend + legacy stub + BAR decode + caps walk
 *
 *  Provides:
 *    - stor_pci_init_ecam()   - primary path (MMIO, PCIe 3.0 spec layout)
 *    - stor_pci_init_legacy() - CF8/CFC stub (unimplemented on plain RV)
 *    - stor_pci_init_custom() - for platforms with custom bus bridges
 *    - stor_pci_read/write{8,16,32}()
 *    - stor_pci_enumerate()   - class scan across all buses
 *    - stor_pci_get_bar()     - decodes 32/64-bit MEM, IO, size, prefetch
 *    - stor_pci_find_capability()      - PCI cap list walk
 *    - stor_pci_find_ext_capability()  - PCIe extended cap walk
 *
 *  ECAM addressing (PCIe 3.0 §7.2.2):
 *    addr = ECAM_base + (bus << 20) + (dev << 15) + (fn << 12) + off
 *  Every function occupies 4 KiB of the 256 MiB-per-bus ECAM window.
 *
 *  Notes:
 *    - BAR probing temporarily writes 0xFFFFFFFF (requires the target BAR
 *      to be fully decoded by HW). Caller MUST have run this before any
 *      other driver code touches the device.
 *    - For 64-bit MEM BARs, the following BAR index holds the upper half;
 *      caller MUST advance bar_idx by 2 when iterating.
 * ============================================================================
 */

#include "storage_core.h"

/* ========================================================================== */
/* GLOBAL STATE                                                                */
/* ========================================================================== */

static const stor_pci_ops_t *g_pci_ops = NULL;

typedef struct {
    vaddr_t  ecam_virt;
    paddr_t  ecam_phys;
    size_t   ecam_size;
    u8       bus_start;
    u8       bus_end;
    bool     active;
} stor_pci_ecam_state_t;

static stor_pci_ecam_state_t g_ecam = {0};

/* ========================================================================== */
/* ECAM BACKEND                                                                */
/* ========================================================================== */

static int stor_pci_ecam_xlate(u8 bus, u8 dev, u8 fn, u16 off, vaddr_t *out)
{
    if (!g_ecam.active) return STOR_ERR_NOT_READY;
    if (bus < g_ecam.bus_start || bus > g_ecam.bus_end) return STOR_ERR_RANGE;
    if (dev > 31 || fn > 7)   return STOR_ERR_INVAL;
    if (off > 0x0FFF)         return STOR_ERR_RANGE;

    vaddr_t base_off = ((vaddr_t)(bus - g_ecam.bus_start) << 20)
                     | ((vaddr_t)dev << 15)
                     | ((vaddr_t)fn  << 12)
                     | (vaddr_t)off;
    if (base_off + 4 > g_ecam.ecam_size) return STOR_ERR_RANGE;

    *out = g_ecam.ecam_virt + base_off;
    return STOR_OK;
}

static int stor_pci_ecam_read32(u8 bus, u8 dev, u8 fn, u16 off, u32 *out)
{
    vaddr_t a;
    int rc = stor_pci_ecam_xlate(bus, dev, fn, off, &a);
    if (rc != STOR_OK) return rc;
    *out = stor_readl((const volatile void __iomem *)a);
    return STOR_OK;
}

static int stor_pci_ecam_write32(u8 bus, u8 dev, u8 fn, u16 off, u32 v)
{
    vaddr_t a;
    int rc = stor_pci_ecam_xlate(bus, dev, fn, off, &a);
    if (rc != STOR_OK) return rc;
    stor_writel(v, (volatile void __iomem *)a);
    return STOR_OK;
}

static const stor_pci_ops_t g_ecam_ops = {
    .read32  = stor_pci_ecam_read32,
    .write32 = stor_pci_ecam_write32,
};

/* ========================================================================== */
/* LEGACY CF8/CFC BACKEND (stub)                                               */
/* ========================================================================== */
/* Plain RISC-V has no port I/O. A platform exposing x86-style 0xCF8/0xCFC via
 * SBI or a custom CSR bridge may override this with stor_pci_init_custom(). */

static int stor_pci_legacy_read32(u8 bus, u8 dev, u8 fn, u16 off, u32 *out)
{
    (void)bus; (void)dev; (void)fn; (void)off;
    if (out) *out = 0xFFFFFFFFu;
    return STOR_ERR_NOT_SUPPORTED;
}

static int stor_pci_legacy_write32(u8 bus, u8 dev, u8 fn, u16 off, u32 v)
{
    (void)bus; (void)dev; (void)fn; (void)off; (void)v;
    return STOR_ERR_NOT_SUPPORTED;
}

static const stor_pci_ops_t g_legacy_ops = {
    .read32  = stor_pci_legacy_read32,
    .write32 = stor_pci_legacy_write32,
};

/* ========================================================================== */
/* INIT / SHUTDOWN                                                             */
/* ========================================================================== */

int stor_pci_init_ecam(paddr_t ecam_phys, size_t ecam_size,
                       u8 bus_start, u8 bus_end)
{
    if (ecam_size == 0)          return STOR_ERR_INVAL;
    if (bus_end < bus_start)     return STOR_ERR_INVAL;

    size_t min_size = (size_t)(bus_end - bus_start + 1) << 20;  /* 1 MiB/bus */
    if (ecam_size < min_size)    return STOR_ERR_INVAL;

    vaddr_t virt = 0;
    int rc = stor_mmio_map(ecam_phys, ecam_size, &virt);
    if (rc != STOR_OK) return rc;

    g_ecam.ecam_phys = ecam_phys;
    g_ecam.ecam_virt = virt;
    g_ecam.ecam_size = ecam_size;
    g_ecam.bus_start = bus_start;
    g_ecam.bus_end   = bus_end;
    g_ecam.active    = true;

    g_pci_ops = &g_ecam_ops;

    stor_info("PCI: ECAM mapped phys=0x%lx size=0x%lx buses=%u..%u",
              (unsigned long)ecam_phys, (unsigned long)ecam_size,
              (unsigned)bus_start, (unsigned)bus_end);
    return STOR_OK;
}

int stor_pci_init_legacy(void)
{
    g_pci_ops = &g_legacy_ops;
    stor_warn("PCI: legacy CF8/CFC backend installed (stub)");
    return STOR_OK;
}

int stor_pci_init_custom(const stor_pci_ops_t *ops)
{
    if (!ops || !ops->read32 || !ops->write32) return STOR_ERR_INVAL;
    g_pci_ops = ops;
    return STOR_OK;
}

int stor_pci_shutdown(void)
{
    if (g_ecam.active) {
        stor_mmio_unmap(g_ecam.ecam_virt, g_ecam.ecam_size);
        stor_mem_set(&g_ecam, 0, sizeof(g_ecam));
    }
    g_pci_ops = NULL;
    return STOR_OK;
}

bool stor_pci_ready(void) { return g_pci_ops != NULL; }

/* ========================================================================== */
/* ACCESSORS                                                                  */
/* ========================================================================== */

int stor_pci_read32(u8 bus, u8 dev, u8 fn, u16 off, u32 *out)
{
    if (!g_pci_ops || !g_pci_ops->read32) return STOR_ERR_NOT_READY;
    if (!out || (off & 0x3))              return STOR_ERR_INVAL;
    return g_pci_ops->read32(bus, dev, fn, off, out);
}

int stor_pci_write32(u8 bus, u8 dev, u8 fn, u16 off, u32 v)
{
    if (!g_pci_ops || !g_pci_ops->write32) return STOR_ERR_NOT_READY;
    if (off & 0x3)                        return STOR_ERR_INVAL;
    return g_pci_ops->write32(bus, dev, fn, off, v);
}

int stor_pci_read16(u8 bus, u8 dev, u8 fn, u16 off, u16 *out)
{
    if (!out || (off & 0x1)) return STOR_ERR_INVAL;
    u32 v;
    int rc = stor_pci_read32(bus, dev, fn, (u16)(off & ~0x3u), &v);
    if (rc != STOR_OK) return rc;
    *out = (u16)((v >> ((off & 2) ? 16 : 0)) & 0xFFFFu);
    return STOR_OK;
}

int stor_pci_write16(u8 bus, u8 dev, u8 fn, u16 off, u16 val)
{
    if (off & 0x1) return STOR_ERR_INVAL;
    u32 cur;
    int rc = stor_pci_read32(bus, dev, fn, (u16)(off & ~0x3u), &cur);
    if (rc != STOR_OK) return rc;
    u32 shift = (off & 2) ? 16 : 0;
    cur = (cur & ~(0xFFFFu << shift)) | ((u32)val << shift);
    return stor_pci_write32(bus, dev, fn, (u16)(off & ~0x3u), cur);
}

int stor_pci_read8(u8 bus, u8 dev, u8 fn, u16 off, u8 *out)
{
    if (!out) return STOR_ERR_INVAL;
    u32 v;
    int rc = stor_pci_read32(bus, dev, fn, (u16)(off & ~0x3u), &v);
    if (rc != STOR_OK) return rc;
    *out = (u8)((v >> ((off & 3) * 8)) & 0xFFu);
    return STOR_OK;
}

int stor_pci_write8(u8 bus, u8 dev, u8 fn, u16 off, u8 val)
{
    u32 cur;
    int rc = stor_pci_read32(bus, dev, fn, (u16)(off & ~0x3u), &cur);
    if (rc != STOR_OK) return rc;
    u32 shift = (off & 3) * 8;
    cur = (cur & ~(0xFFu << shift)) | ((u32)val << shift);
    return stor_pci_write32(bus, dev, fn, (u16)(off & ~0x3u), cur);
}

/* ========================================================================== */
/* DEVICE DISCOVERY                                                           */
/* ========================================================================== */

static int stor_pci_read_device(u8 bus, u8 dev, u8 fn, stor_pci_device_t *out)
{
    u32 v;
    int rc;

    rc = stor_pci_read32(bus, dev, fn, 0x00, &v);
    if (rc != STOR_OK) return rc;
    u16 vid = (u16)(v & 0xFFFFu);
    if (vid == 0xFFFFu || vid == 0x0000u) return STOR_ERR_NO_DEVICE;

    stor_mem_set(out, 0, sizeof(*out));
    out->bus = bus;
    out->dev = dev;
    out->fn  = fn;
    out->vendor_id = vid;
    out->device_id = (u16)(v >> 16);

    rc = stor_pci_read32(bus, dev, fn, 0x08, &v);
    if (rc != STOR_OK) return rc;
    out->revision   = (u8)( v        & 0xFFu);
    out->prog_if    = (u8)((v >> 8)  & 0xFFu);
    out->subclass   = (u8)((v >> 16) & 0xFFu);
    out->class_code = (u8)((v >> 24) & 0xFFu);

    rc = stor_pci_read32(bus, dev, fn, 0x0C, &v);
    if (rc != STOR_OK) return rc;
    out->header_type = (u8)((v >> 16) & 0xFFu);

    /* Subsystem IDs live at 0x2C only for header_type 0x00 (endpoint) */
    if ((out->header_type & 0x7F) == 0x00) {
        rc = stor_pci_read32(bus, dev, fn, 0x2C, &v);
        if (rc == STOR_OK) {
            out->subsystem_vendor_id = (u16)(v & 0xFFFFu);
            out->subsystem_id        = (u16)(v >> 16);
        }
    }

    (void)stor_pci_read8(bus, dev, fn, 0x3C, &out->irq_line);
    return STOR_OK;
}

int stor_pci_enumerate(u8 class_code, u8 subclass, u8 prog_if,
                       stor_pci_device_t *out, u32 max_out, u32 *found)
{
    if (found) *found = 0;
    if (!out || max_out == 0) return STOR_ERR_INVAL;
    if (!g_pci_ops)           return STOR_ERR_NOT_READY;

    u8 bus_start = g_ecam.active ? g_ecam.bus_start : 0;
    u8 bus_end   = g_ecam.active ? g_ecam.bus_end   : 255;

    u32 n = 0;
    for (u32 bus = bus_start; bus <= bus_end; bus++) {
        for (u32 dev = 0; dev < 32; dev++) {
            for (u32 fn = 0; fn < 8; fn++) {
                stor_pci_device_t d;
                int rc = stor_pci_read_device((u8)bus, (u8)dev, (u8)fn, &d);
                if (rc != STOR_OK) {
                    if (fn == 0) break;     /* fn 0 gone -> no more */
                    continue;
                }
                if (class_code != 0xFF && d.class_code != class_code) continue;
                if (subclass   != 0xFF && d.subclass   != subclass)   continue;
                if (prog_if    != 0xFF && d.prog_if    != prog_if)    continue;

                if (n < max_out) out[n] = d;
                n++;

                /* Single-function device: do not probe fn 1..7 */
                if (fn == 0 && !(d.header_type & 0x80)) break;
            }
        }
    }
    if (found) *found = n;
    return (n > 0) ? STOR_OK : STOR_ERR_NO_DEVICE;
}

/* ========================================================================== */
/* BAR DECODING                                                               */
/* ========================================================================== */
/*
 * Standard algorithm: read orig, write 0xFFFFFFFF, read back mask, restore
 * orig. size = (~mask + 1) after clearing low attribute bits.
 *
 * WARNING: caller must ensure no other driver is bound to the device.
 * For 64-bit MEM BAR, upper half occupies (bar_idx + 1); caller must skip it.
 */

int stor_pci_get_bar(u8 bus, u8 dev, u8 fn, u8 bar_idx, stor_pci_bar_t *out)
{
    if (!out || bar_idx > 5) return STOR_ERR_INVAL;
    stor_mem_set(out, 0, sizeof(*out));

    u16 off = (u16)(0x10 + bar_idx * 4);
    u32 orig;
    int rc = stor_pci_read32(bus, dev, fn, off, &orig);
    if (rc != STOR_OK) return rc;
    if (orig == 0) return STOR_ERR_NO_DEVICE;   /* Unimplemented BAR */

    out->is_io = (orig & 0x01u) != 0;

    /* -------------------- IO BAR (bit[0]=1) -------------------- */
    if (out->is_io) {
        out->base_phys = (u64)(orig & 0xFFFFFFFCu);

        rc = stor_pci_write32(bus, dev, fn, off, 0xFFFFFFFFu);
        if (rc != STOR_OK) return rc;
        u32 mask;
        rc = stor_pci_read32(bus, dev, fn, off, &mask);
        (void)stor_pci_write32(bus, dev, fn, off, orig);
        if (rc != STOR_OK) return rc;

        u32 size_mask = (~mask) & 0xFFFFFFFCu;
        out->size = size_mask ? (u64)((~size_mask + 1) & 0xFFFFFFFFu) : 0;
        return STOR_OK;
    }

    /* -------------------- MEM BAR -------------------- */
    u32 mem_type = (orig >> 1) & 0x03u;
    out->is_prefetchable = (orig & 0x08u) != 0;
    out->is_64bit        = (mem_type == 0x02u);

    if (out->is_64bit && bar_idx < 5) {
        u32 orig_hi;
        rc = stor_pci_read32(bus, dev, fn, (u16)(off + 4), &orig_hi);
        if (rc != STOR_OK) return rc;

        rc = stor_pci_write32(bus, dev, fn, off, 0xFFFFFFFFu);
        if (rc == STOR_OK)
            rc = stor_pci_write32(bus, dev, fn, (u16)(off + 4), 0xFFFFFFFFu);
        if (rc != STOR_OK) {
            (void)stor_pci_write32(bus, dev, fn, off, orig);
            return rc;
        }

        u32 mask_lo = 0, mask_hi = 0;
        (void)stor_pci_read32(bus, dev, fn, off, &mask_lo);
        (void)stor_pci_read32(bus, dev, fn, (u16)(off + 4), &mask_hi);

        (void)stor_pci_write32(bus, dev, fn, (u16)(off + 4), orig_hi);
        (void)stor_pci_write32(bus, dev, fn, off, orig);

        u64 full_mask = ((u64)mask_hi << 32) | (mask_lo & 0xFFFFFFF0u);
        if (full_mask == 0xFFFFFFFFFFFFFFFFULL) out->size = 0;
        else                                    out->size = (~full_mask) + 1u;

        out->base_phys = ((u64)orig_hi << 32) | (orig & 0xFFFFFFF0u);
        return STOR_OK;
    }

    /* 32-bit MEM BAR */
    out->base_phys = (u64)(orig & 0xFFFFFFF0u);

    rc = stor_pci_write32(bus, dev, fn, off, 0xFFFFFFFFu);
    if (rc != STOR_OK) return rc;
    u32 mask;
    rc = stor_pci_read32(bus, dev, fn, off, &mask);
    (void)stor_pci_write32(bus, dev, fn, off, orig);
    if (rc != STOR_OK) return rc;

    u32 size_mask = (~mask) & 0xFFFFFFF0u;
    out->size = size_mask ? (u64)((~size_mask + 1) & 0xFFFFFFFFu) : 0;
    return STOR_OK;
}

/* ========================================================================== */
/* COMMAND REGISTER HELPERS                                                   */
/* ========================================================================== */

#define PCI_CMD_IO_SPACE        0x0001u
#define PCI_CMD_MEM_SPACE       0x0002u
#define PCI_CMD_BUS_MASTER      0x0004u
#define PCI_CMD_INTX_DISABLE    0x0400u

static int stor_pci_cmd_set(u8 bus, u8 dev, u8 fn, u16 bits)
{
    u16 cmd;
    int rc = stor_pci_read16(bus, dev, fn, 0x04, &cmd);
    if (rc != STOR_OK) return rc;
    return stor_pci_write16(bus, dev, fn, 0x04, (u16)(cmd | bits));
}

static int stor_pci_cmd_clear(u8 bus, u8 dev, u8 fn, u16 bits)
{
    u16 cmd;
    int rc = stor_pci_read16(bus, dev, fn, 0x04, &cmd);
    if (rc != STOR_OK) return rc;
    return stor_pci_write16(bus, dev, fn, 0x04, (u16)(cmd & ~bits));
}

int stor_pci_enable_mem(u8 bus, u8 dev, u8 fn)
{ return stor_pci_cmd_set(bus, dev, fn, PCI_CMD_MEM_SPACE); }

int stor_pci_enable_busmaster(u8 bus, u8 dev, u8 fn)
{ return stor_pci_cmd_set(bus, dev, fn, PCI_CMD_BUS_MASTER); }

int stor_pci_enable_intx_disable(u8 bus, u8 dev, u8 fn)
{ return stor_pci_cmd_set(bus, dev, fn, PCI_CMD_INTX_DISABLE); }

/* ========================================================================== */
/* CAPABILITY WALKS                                                           */
/* ========================================================================== */

int stor_pci_find_capability(u8 bus, u8 dev, u8 fn, u8 cap_id, u8 *out_off)
{
    u16 status;
    int rc = stor_pci_read16(bus, dev, fn, 0x06, &status);
    if (rc != STOR_OK) return rc;
    if (!(status & 0x0010u)) return STOR_ERR_NOT_SUPPORTED;   /* No cap list */

    u8 ptr;
    rc = stor_pci_read8(bus, dev, fn, 0x34, &ptr);
    if (rc != STOR_OK) return rc;
    if (ptr < 0x40) return STOR_ERR_NOT_SUPPORTED;

    for (u32 guard = 0; ptr != 0 && guard < 48; guard++) {
        u8 id;
        rc = stor_pci_read8(bus, dev, fn, ptr, &id);
        if (rc != STOR_OK) return rc;
        if (id == cap_id) {
            if (out_off) *out_off = ptr;
            return STOR_OK;
        }
        u8 next;
        rc = stor_pci_read8(bus, dev, fn, (u16)(ptr + 1), &next);
        if (rc != STOR_OK) return rc;
        if (next <= ptr) return STOR_ERR_PROTOCOL;            /* Malformed */
        ptr = next;
    }
    return STOR_ERR_NOT_SUPPORTED;
}

int stor_pci_find_ext_capability(u8 bus, u8 dev, u8 fn, u16 cap_id, u16 *out_off)
{
    u32 off = 0x100;
    for (u32 guard = 0; off < 0x1000 && guard < 64; guard++) {
        u32 hdr;
        int rc = stor_pci_read32(bus, dev, fn, (u16)off, &hdr);
        if (rc != STOR_OK) return rc;
        u16 id = (u16)(hdr & 0xFFFFu);
        if (id == 0x0000u || id == 0xFFFFu) return STOR_ERR_NOT_SUPPORTED;
        if (id == cap_id) {
            if (out_off) *out_off = (u16)off;
            return STOR_OK;
        }
        u32 next = (hdr >> 20) & 0xFFFu;
        if (next < 0x100u || next <= off) return STOR_ERR_NOT_SUPPORTED;
        off = next;
    }
    return STOR_ERR_NOT_SUPPORTED;
}

/* ========================================================================== */
/* DIAGNOSTICS                                                                */
/* ========================================================================== */

__maybe_unused static void stor_pci_dump_device(const stor_pci_device_t *d)
{
    if (!d) return;
    stor_info("PCI %02x:%02x.%x  [%04x:%04x]  class=%02x/%02x/if%02x  "
              "sub=[%04x:%04x]  hdr=%02x irq=%u",
              d->bus, d->dev, d->fn,
              d->vendor_id, d->device_id,
              d->class_code, d->subclass, d->prog_if,
              d->subsystem_vendor_id, d->subsystem_id,
              d->header_type, d->irq_line);
}

/* ========================================================================== */
/* END OF BLOCK 1                                                             */
/* ========================================================================== */
