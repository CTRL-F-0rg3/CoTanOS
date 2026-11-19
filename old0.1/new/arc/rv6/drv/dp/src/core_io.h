/*
 * ============================================================================
 *  PROPRIETARY LOW-LEVEL SYSTEM FOUNDATION - DP/HDMI UNIFIED CORE HEADER
 *  Architecture: RISC-V (RV32 / RV64)
 *  Language: C11 / C++17 Compatible
 *  Purpose: Bare-metal MMIO, DPCD, AUX CH, Link Training, MSA, EDID, CEA-861,
 *           HDMI infoframes, HDCP, DSC, MST, PSR, Audio, timings & helpers.
 * ============================================================================
 */

#ifndef DP_CORE_H
#define DP_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* 1. COMPILER MODIFIERS                                                       */
/* ========================================================================== */

#ifndef __iomem
#define __iomem
#endif
#ifndef __force
#define __force
#endif
#ifndef __packed
#define __packed __attribute__((packed))
#endif
#ifndef __aligned
#define __aligned(x) __attribute__((aligned(x)))
#endif
#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif
#ifndef __noinline
#define __noinline __attribute__((noinline))
#endif
#ifndef __maybe_unused
#define __maybe_unused __attribute__((unused))
#endif
#ifndef __unused
#define __unused __attribute__((unused))
#endif
#ifndef __fallthrough
#define __fallthrough __attribute__((fallthrough))
#endif
#ifndef __noreturn
#define __noreturn __attribute__((noreturn))
#endif
#ifndef __weak
#define __weak __attribute__((weak))
#elif defined(__GNUC__)
#undef __weak
#define __weak __attribute__((weak))
#endif
#ifndef likely
#define likely(x)   __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif
#ifndef offsetof
#define offsetof(t, m) __builtin_offsetof(t, m)
#endif
#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
#endif
#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(cond) ((void)sizeof(char[1 - 2*!!(cond)]))
#endif
#ifndef static_assert
#define static_assert _Static_assert
#endif

/* ========================================================================== */
/* 2. BASIC TYPE ALIASES                                                       */
/* ========================================================================== */

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef uintptr_t paddr_t;
typedef uintptr_t vaddr_t;
typedef uint64_t  dma_addr_t;
typedef size_t    usize;
typedef ssize_t   ssize;

typedef u8  __u8;
typedef u16 __u16;
typedef u32 __u32;
typedef u64 __u64;
typedef s8  __s8;
typedef s16 __s16;
typedef s32 __s32;
typedef s64 __s64;

typedef u16 __le16;
typedef u16 __be16;
typedef u32 __le32;
typedef u32 __be32;
typedef u64 __le64;
typedef u64 __be64;

typedef u8  dpcd_addr_t;
typedef u16 dpcd_word_t;

typedef u32 dp_reg_t;
typedef u32 dp_offset_t;

typedef unsigned int uint;

#ifndef NULL
#define NULL ((void *)0)
#endif

/* ========================================================================== */
/* 3. BIT MANIPULATION PRIMITIVES                                              */
/* ========================================================================== */

#define BIT(nr)              (1UL << (nr))
#define BIT_ULL(nr)          (1ULL << (nr))
#define BITS_PER_LONG        (sizeof(unsigned long) * 8)
#define BITS_PER_LONG_LONG   (sizeof(unsigned long long) * 8)

#define GENMASK(h, l) \
    (((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
#define GENMASK_ULL(h, l) \
    (((~0ULL) << (l)) & (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))

#define FIELD_GET(mask, reg)  (((reg) & (mask)) / ((mask) & ~((mask) << 1)))
#define FIELD_PREP(mask, val) (((val) * ((mask) & ~((mask) << 1))) & (mask))

#define _DP_BF_MASK(h, l) (((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
#define DP_BF(name, h, l) static inline __always_unused u32 \
    DP_GET_##name(u32 v) { return (v & _DP_BF_MASK(h, l)) >> (l); } \
    static inline __always_unused u32 \
    DP_SET_##name(u32 v, u32 x) { return (v & ~_DP_BF_MASK(h, l)) | \
        (((x) << (l)) & _DP_BF_MASK(h, l)); }

#define __always_unused __attribute__((unused))

static inline u32 dp_bf_get(u32 val, u32 mask)
{
    u32 shift = 0;
    u32 m = mask;
    while (m && !(m & 1)) { m >>= 1; shift++; }
    return (val & mask) >> shift;
}

static inline u32 dp_bf_prep(u32 val, u32 mask)
{
    u32 shift = 0;
    u32 m = mask;
    while (m && !(m & 1)) { m >>= 1; shift++; }
    return (val << shift) & mask;
}

#define dp_bf_set(val, mask, field) \
    (((val) & ~(mask)) | dp_bf_prep((field), (mask)))

static inline u32 dp_roundup_pow2(u32 x)
{
    if (x <= 1) return 1;
    x--;
    x |= x >> 1; x |= x >> 2; x |= x >> 4; x |= x >> 8; x |= x >> 16;
    return x + 1;
}

static inline bool dp_is_pow2(u32 x) { return (x && !(x & (x - 1))); }

static inline u32 dp_align_up(u32 v, u32 a) { return (v + a - 1) & ~(a - 1); }
static inline u32 dp_align_down(u32 v, u32 a) { return v & ~(a - 1); }
static inline bool dp_align_check(u32 v, u32 a) { return (v & (a - 1)) == 0; }

static inline u32 dp_min_u32(u32 a, u32 b) { return a < b ? a : b; }
static inline u32 dp_max_u32(u32 a, u32 b) { return a > b ? a : b; }
static inline u32 dp_clamp_u32(u32 v, u32 lo, u32 hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline u32 dp_popcount32(u32 x)
{
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0f0f0f0fu;
    return (x * 0x01010101u) >> 24;
}

static inline u32 dp_clz32(u32 x)
{
    if (!x) return 32;
    u32 n = 0;
    if (!(x & 0xFFFF0000u)) { n += 16; x <<= 16; }
    if (!(x & 0xFF000000u)) { n += 8;  x <<= 8;  }
    if (!(x & 0xF0000000u)) { n += 4;  x <<= 4;  }
    if (!(x & 0xC0000000u)) { n += 2;  x <<= 2;  }
    if (!(x & 0x80000000u)) { n += 1; }
    return n;
}

static inline u32 dp_ctz32(u32 x)
{
    if (!x) return 32;
    u32 n = 0;
    if (!(x & 0x0000FFFFu)) { n += 16; x >>= 16; }
    if (!(x & 0x000000FFu)) { n += 8;  x >>= 8;  }
    if (!(x & 0x0000000Fu)) { n += 4;  x >>= 4;  }
    if (!(x & 0x00000003u)) { n += 2;  x >>= 2;  }
    if (!(x & 0x00000001u)) { n += 1; }
    return n;
}

static inline u32 dp_ilog2(u32 x) { return 31 - dp_clz32(x); }

static inline u64 dp_mul_div_u64(u64 val, u64 mul, u64 div)
{
    if (!div) return 0;
    return (val * mul) / div;
}

static inline u32 dp_abs_diff_u32(u32 a, u32 b) { return a > b ? a - b : b - a; }

static inline void dp_mem_copy(void *dst, const void *src, usize n)
{
    u8 *d = (u8 *)dst; const u8 *s = (const u8 *)src;
    while (n--) *d++ = *s++;
}

static inline void dp_mem_set(void *dst, u8 v, usize n)
{
    u8 *d = (u8 *)dst;
    while (n--) *d++ = v;
}

static inline int dp_mem_cmp(const void *a, const void *b, usize n)
{
    const u8 *x = (const u8 *)a; const u8 *y = (const u8 *)b;
    while (n--) { if (*x != *y) return (int)*x - (int)*y; x++; y++; }
    return 0;
}

/* ========================================================================== */
/* 4. BYTE ORDER HELPERS                                                       */
/* ========================================================================== */

static inline u16 dp_bswap16(u16 x) { return (u16)((x >> 8) | (x << 8)); }
static inline u32 dp_bswap32(u32 x)
{
    return ((x & 0xFF000000u) >> 24) | ((x & 0x00FF0000u) >> 8) |
           ((x & 0x0000FF00u) << 8)  | ((x & 0x000000FFu) << 24);
}
static inline u64 dp_bswap64(u64 x)
{
    return ((x & 0xFF00000000000000ULL) >> 56) |
           ((x & 0x00FF000000000000ULL) >> 40) |
           ((x & 0x0000FF0000000000ULL) >> 24) |
           ((x & 0x000000FF00000000ULL) >>  8) |
           ((x & 0x00000000FF000000ULL) <<  8) |
           ((x & 0x0000000000FF0000ULL) << 24) |
           ((x & 0x000000000000FF00ULL) << 40) |
           ((x & 0x00000000000000FFULL) << 56);
}

#define cpu_to_le16(x) ((u16)(x))
#define cpu_to_le32(x) ((u32)(x))
#define cpu_to_le64(x) ((u64)(x))
#define le16_to_cpu(x) ((u16)(x))
#define le32_to_cpu(x) ((u32)(x))
#define le64_to_cpu(x) ((u64)(x))

#define cpu_to_be16(x) dp_bswap16((u16)(x))
#define cpu_to_be32(x) dp_bswap32((u32)(x))
#define cpu_to_be64(x) dp_bswap64((u64)(x))
#define be16_to_cpu(x) dp_bswap16((u16)(x))
#define be32_to_cpu(x) dp_bswap32((u32)(x))
#define be64_to_cpu(x) dp_bswap64((u64)(x))

/* ========================================================================== */
/* 5. MEMORY BARRIERS & FENCE PRIMITIVES                                       */
/* ========================================================================== */

#define riscv_mb()      __asm__ __volatile__ ("fence iorw, iorw" : : : "memory")
#define riscv_wmb()     __asm__ __volatile__ ("fence ow, ow"     : : : "memory")
#define riscv_rmb()     __asm__ __volatile__ ("fence ir, ir"     : : : "memory")
#define riscv_smp_mb()  __asm__ __volatile__ ("fence rw, rw"     : : : "memory")
#define riscv_smp_rmb() __asm__ __volatile__ ("fence r, r"       : : : "memory")
#define riscv_smp_wmb() __asm__ __volatile__ ("fence w, w"       : : : "memory")
#define riscv_fence_i() __asm__ __volatile__ ("fence.i"          : : : "memory")

#define __io_br()       do {} while (0)
#define __io_ar()       __asm__ __volatile__ ("fence iorw, iorw" : : : "memory")
#define __io_bw()       __asm__ __volatile__ ("fence iorw, iorw" : : : "memory")
#define __io_aw()       do {} while (0)

#define rmb()   riscv_rmb()
#define wmb()   riscv_wmb()
#define mb()    riscv_mb()
#define smp_mb()  riscv_smp_mb()
#define smp_rmb() riscv_smp_rmb()
#define smp_wmb() riscv_smp_wmb()

#define barrier() __asm__ __volatile__("" : : : "memory")

#define READ_ONCE(x) (*(volatile __typeof__(x) *)&(x))
#define WRITE_ONCE(x, v) (*(volatile __typeof__(x) *)&(x) = (v))

/* ========================================================================== */
/* 6. RAW MMIO INTRINSICS                                                      */
/* ========================================================================== */

static inline void __raw_writeb(u8 v, volatile void __iomem *a)
{ __asm__ __volatile__ ("sb %0, 0(%1)" : : "r"(v), "r"(a) : "memory"); }

static inline void __raw_writew(u16 v, volatile void __iomem *a)
{ __asm__ __volatile__ ("sh %0, 0(%1)" : : "r"(v), "r"(a) : "memory"); }

static inline void __raw_writel(u32 v, volatile void __iomem *a)
{ __asm__ __volatile__ ("sw %0, 0(%1)" : : "r"(v), "r"(a) : "memory"); }

static inline void __raw_writeq(u64 v, volatile void __iomem *a)
{ __asm__ __volatile__ ("sd %0, 0(%1)" : : "r"(v), "r"(a) : "memory"); }

static inline u8 __raw_readb(const volatile void __iomem *a)
{ u8 v; __asm__ __volatile__ ("lb %0, 0(%1)" : "=r"(v) : "r"(a) : "memory"); return v; }

static inline u16 __raw_readw(const volatile void __iomem *a)
{ u16 v; __asm__ __volatile__ ("lh %0, 0(%1)" : "=r"(v) : "r"(a) : "memory"); return v; }

static inline u32 __raw_readl(const volatile void __iomem *a)
{ u32 v; __asm__ __volatile__ ("lw %0, 0(%1)" : "=r"(v) : "r"(a) : "memory"); return v; }

static inline u64 __raw_readq(const volatile void __iomem *a)
{ u64 v; __asm__ __volatile__ ("ld %0, 0(%1)" : "=r"(v) : "r"(a) : "memory"); return v; }

/* ========================================================================== */
/* 7. SYNCHRONIZED MMIO ACCESSORS                                              */
/* ========================================================================== */

static inline u8  readb(const volatile void __iomem *a) { u8 v;  __io_br(); v = __raw_readb(a); __io_ar(); return v; }
static inline u16 readw(const volatile void __iomem *a) { u16 v; __io_br(); v = __raw_readw(a); __io_ar(); return v; }
static inline u32 readl(const volatile void __iomem *a) { u32 v; __io_br(); v = __raw_readl(a); __io_ar(); return v; }
static inline u64 readq(const volatile void __iomem *a) { u64 v; __io_br(); v = __raw_readq(a); __io_ar(); return v; }

static inline void writeb(u8 v,  volatile void __iomem *a) { __io_bw(); __raw_writeb(v, a); __io_aw(); }
static inline void writew(u16 v, volatile void __iomem *a) { __io_bw(); __raw_writew(v, a); __io_aw(); }
static inline void writel(u32 v, volatile void __iomem *a) { __io_bw(); __raw_writel(v, a); __io_aw(); }
static inline void writeq(u64 v, volatile void __iomem *a) { __io_bw(); __raw_writeq(v, a); __io_aw(); }

/* ========================================================================== */
/* 8. RELAXED MMIO ACCESSORS                                                   */
/* ========================================================================== */

#define readb_relaxed(c)    __raw_readb(c)
#define readw_relaxed(c)    __raw_readw(c)
#define readl_relaxed(c)    __raw_readl(c)
#define readq_relaxed(c)    __raw_readq(c)

#define writeb_relaxed(v,c) __raw_writeb((v), (c))
#define writew_relaxed(v,c) __raw_writew((v), (c))
#define writel_relaxed(v,c) __raw_writel((v), (c))
#define writeq_relaxed(v,c) __raw_writeq((v), (c))

/* ========================================================================== */
/* 9. BLOCK I/O ROUTINES                                                       */
/* ========================================================================== */

static inline void readsb(const volatile void __iomem *a, void *b, unsigned n)
{ u8 *p = (u8 *)b;  while (n--) *p++ = __raw_readb(a); }
static inline void readsw(const volatile void __iomem *a, void *b, unsigned n)
{ u16 *p = (u16 *)b; while (n--) *p++ = __raw_readw(a); }
static inline void readsl(const volatile void __iomem *a, void *b, unsigned n)
{ u32 *p = (u32 *)b; while (n--) *p++ = __raw_readl(a); }
static inline void readsq(const volatile void __iomem *a, void *b, unsigned n)
{ u64 *p = (u64 *)b; while (n--) *p++ = __raw_readq(a); }

static inline void writesb(volatile void __iomem *a, const void *b, unsigned n)
{ const u8 *p = (const u8 *)b;  while (n--) __raw_writeb(*p++, a); }
static inline void writesw(volatile void __iomem *a, const void *b, unsigned n)
{ const u16 *p = (const u16 *)b; while (n--) __raw_writew(*p++, a); }
static inline void writesl(volatile void __iomem *a, const void *b, unsigned n)
{ const u32 *p = (const u32 *)b; while (n--) __raw_writel(*p++, a); }
static inline void writesq(volatile void __iomem *a, const void *b, unsigned n)
{ const u64 *p = (const u64 *)b; while (n--) __raw_writeq(*p++, a); }

#define ioread8(c)      readb(c)
#define ioread16(c)     readw(c)
#define ioread32(c)     readl(c)
#define ioread64(c)     readq(c)
#define iowrite8(v,c)   writeb((v), (c))
#define iowrite16(v,c)  writew((v), (c))
#define iowrite32(v,c)  writel((v), (c))
#define iowrite64(v,c)  writeq((v), (c))

#define ioread8_rep(c,b,n)   readsb(c,b,n)
#define ioread16_rep(c,b,n)  readsw(c,b,n)
#define ioread32_rep(c,b,n)  readsl(c,b,n)
#define iowrite8_rep(c,b,n)  writesb(c,b,n)
#define iowrite16_rep(c,b,n) writesw(c,b,n)
#define iowrite32_rep(c,b,n) writesl(c,b,n)

/* ========================================================================== */
/* 10. REGISTER ACCESS HELPERS (DP/HDMI)                                       */
/* ========================================================================== */

static inline u32 dp_reg_read(vaddr_t base, u32 off)
{ return readl((const volatile void __iomem *)(base + off)); }

static inline void dp_reg_write(vaddr_t base, u32 off, u32 v)
{ writel(v, (volatile void __iomem *)(base + off)); }

static inline void dp_reg_update(vaddr_t base, u32 off, u32 mask, u32 val)
{
    u32 cur = dp_reg_read(base, off);
    cur &= ~mask;
    cur |= (val & mask);
    dp_reg_write(base, off, cur);
}

static inline void dp_reg_set_bits(vaddr_t base, u32 off, u32 bits)
{ dp_reg_write(base, off, dp_reg_read(base, off) | bits); }

static inline void dp_reg_clear_bits(vaddr_t base, u32 off, u32 bits)
{ dp_reg_write(base, off, dp_reg_read(base, off) & ~bits); }

static inline void dp_reg_set_field(vaddr_t base, u32 off, u32 mask, u32 field)
{ dp_reg_update(base, off, mask, dp_bf_prep(field, mask)); }

static inline u32 dp_reg_get_field(vaddr_t base, u32 off, u32 mask)
{ return dp_bf_get(dp_reg_read(base, off), mask); }

/* ========================================================================== */
/* 11. CPU CONTROL & CSR INSPECTION                                            */
/* ========================================================================== */

static inline void cpu_halt(void)       { __asm__ __volatile__ ("wfi"); }
static inline void cpu_breakpoint(void) { __asm__ __volatile__ ("ebreak"); }
static inline void cpu_nop(void)        { __asm__ __volatile__ ("nop"); }
static inline void cpu_pause(void)      { __asm__ __volatile__ ("pause"); }
static inline void cpu_fence_i(void)    { riscv_fence_i(); }
static inline void cpu_wfi(void)        { __asm__ __volatile__ ("wfi"); }
static inline void cpu_mret(void)       { __asm__ __volatile__ ("mret"); }
static inline void cpu_sret(void)       { __asm__ __volatile__ ("sret"); }

static inline u64 riscv_read_time(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, time" : "=r"(v)); return v; }

static inline u64 riscv_read_cycle(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, cycle" : "=r"(v)); return v; }

static inline u64 riscv_read_instret(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, instret" : "=r"(v)); return v; }

static inline u64 riscv_read_mhartid(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, mhartid" : "=r"(v)); return v; }

static inline u64 riscv_read_mstatus(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, mstatus" : "=r"(v)); return v; }

static inline void riscv_write_mstatus(u64 v)
{ __asm__ __volatile__ ("csrw mstatus, %0" : : "r"(v)); }

static inline u64 riscv_read_mie(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, mie" : "=r"(v)); return v; }

static inline void riscv_write_mie(u64 v)
{ __asm__ __volatile__ ("csrw mie, %0" : : "r"(v)); }

static inline void riscv_write_mtvec(u64 v)
{ __asm__ __volatile__ ("csrw mtvec, %0" : : "r"(v)); }

static inline void riscv_mie_enable(u32 bit)
{ riscv_write_mie(riscv_read_mie() | (1ULL << bit)); }

static inline void riscv_mie_disable(u32 bit)
{ riscv_write_mie(riscv_read_mie() & ~(1ULL << bit)); }

#define RISCV_IRQ_M_SOFT     3
#define RISCV_IRQ_M_TIMER    7
#define RISCV_IRQ_M_EXT      11

/* ========================================================================== */
/* 12. ATOMIC OPERATIONS                                                       */
/* ========================================================================== */

static inline u32 atomic_read_32(volatile u32 *a)
{
    u32 v;
    __asm__ __volatile__ ("amoswap.w.aqrl %0, zero, (%1)" : "=r"(v) : "r"(a) : "memory");
    return v;
}

static inline void atomic_write_32(volatile u32 *a, u32 v)
{
    __asm__ __volatile__ ("amoswap.w.aqrl zero, %0, (%1)" : : "r"(v), "r"(a) : "memory");
}

static inline u32 atomic_fetch_add_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoadd.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static inline u32 atomic_fetch_sub_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoadd.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"((u32)(-(s32)v)) : "memory");
    return old;
}

static inline u32 atomic_fetch_and_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoand.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static inline u32 atomic_fetch_or_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoor.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static inline u32 atomic_fetch_xor_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoxor.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static inline u32 atomic_xchg_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoswap.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static inline bool atomic_cmpxchg_32(volatile u32 *a, u32 old_v, u32 new_v)
{
    u32 prev;
    __asm__ __volatile__ (
        "1: lr.w.aqrl %0, (%1)\n"
        "   bne %0, %2, 2f\n"
        "   sc.w.aqrl %0, %3, (%1)\n"
        "   bnez %0, 1b\n"
        "2:\n"
        : "=&r"(prev) : "r"(a), "r"(old_v), "r"(new_v) : "memory");
    return prev == old_v;
}

static inline u32 atomic_add_return_32(volatile u32 *a, u32 v)
{ return atomic_fetch_add_32(a, v) + v; }

static inline u32 atomic_sub_return_32(volatile u32 *a, u32 v)
{ return atomic_fetch_sub_32(a, v) - v; }

static inline bool atomic_test_and_set_32(volatile u32 *a, u32 bit)
{
    u32 old = atomic_fetch_or_32(a, (1u << bit));
    return (old & (1u << bit)) != 0;
}

static inline void atomic_clear_bit_32(volatile u32 *a, u32 bit)
{ (void)atomic_fetch_and_32(a, ~(1u << bit)); }

static inline bool atomic_test_bit_32(volatile u32 *a, u32 bit)
{ return (atomic_read_32(a) & (1u << bit)) != 0; }

static inline u64 atomic_read_64(volatile u64 *a)
{
    u64 v;
    __asm__ __volatile__ ("amoswap.d.aqrl %0, zero, (%1)" : "=r"(v) : "r"(a) : "memory");
    return v;
}

static inline void atomic_write_64(volatile u64 *a, u64 v)
{
    __asm__ __volatile__ ("amoswap.d.aqrl zero, %0, (%1)" : : "r"(v), "r"(a) : "memory");
}

static inline u64 atomic_fetch_add_64(volatile u64 *a, u64 v)
{
    u64 old;
    __asm__ __volatile__ ("amoadd.d.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

/* ========================================================================== */
/* 13. SPIN LOCKS                                                              */
/* ========================================================================== */

typedef struct { volatile u32 lock; } dp_spinlock_t;

#define DP_SPINLOCK_INIT { 0 }

static inline void dp_spin_lock_init(dp_spinlock_t *l) { l->lock = 0; }

static inline void dp_spin_lock(dp_spinlock_t *l)
{
    while (atomic_test_and_set_32(&l->lock, 0) == 0)
        while (READ_ONCE(l->lock)) cpu_pause();
    riscv_mb();
}

static inline bool dp_spin_trylock(dp_spinlock_t *l)
{
    if (atomic_test_and_set_32(&l->lock, 0) == 0) { riscv_mb(); return true; }
    return false;
}

static inline void dp_spin_unlock(dp_spinlock_t *l)
{
    riscv_mb();
    WRITE_ONCE(l->lock, 0);
    riscv_mb();
}

typedef struct { volatile u32 count; dp_spinlock_t lock; } dp_mutex_t;

#define DP_MUTEX_INIT { 0, DP_SPINLOCK_INIT }

static inline void dp_mutex_init(dp_mutex_t *m)
{
    m->count = 1; dp_spin_lock_init(&m->lock);
}

static inline void dp_mutex_lock(dp_mutex_t *m)
{
    for (;;) {
        dp_spin_lock(&m->lock);
        if (m->count) { m->count = 0; dp_spin_unlock(&m->lock); break; }
        dp_spin_unlock(&m->lock);
        cpu_pause();
    }
    riscv_mb();
}

static inline bool dp_mutex_trylock(dp_mutex_t *m)
{
    bool ok = false;
    dp_spin_lock(&m->lock);
    if (m->count) { m->count = 0; ok = true; }
    dp_spin_unlock(&m->lock);
    if (ok) riscv_mb();
    return ok;
}

static inline void dp_mutex_unlock(dp_mutex_t *m)
{
    riscv_mb();
    dp_spin_lock(&m->lock);
    m->count = 1;
    dp_spin_unlock(&m->lock);
}

typedef struct { volatile u32 count; volatile u32 waiters; dp_spinlock_t lock; } dp_semaphore_t;

#define DP_SEMAPHORE_INIT(n) { (n), 0, DP_SPINLOCK_INIT }

static inline void dp_sem_init(dp_semaphore_t *s, u32 n)
{ s->count = n; s->waiters = 0; dp_spin_lock_init(&s->lock); }

static inline void dp_sem_down(dp_semaphore_t *s)
{
    for (;;) {
        dp_spin_lock(&s->lock);
        if (s->count) { s->count--; dp_spin_unlock(&s->lock); break; }
        s->waiters++;
        dp_spin_unlock(&s->lock);
        cpu_pause();
    }
    riscv_mb();
}

static inline bool dp_sem_trydown(dp_semaphore_t *s)
{
    bool ok = false;
    dp_spin_lock(&s->lock);
    if (s->count) { s->count--; ok = true; }
    dp_spin_unlock(&s->lock);
    if (ok) riscv_mb();
    return ok;
}

static inline void dp_sem_up(dp_semaphore_t *s)
{
    riscv_mb();
    dp_spin_lock(&s->lock);
    s->count++;
    dp_spin_unlock(&s->lock);
}

/* ========================================================================== */
/* 14. TIME & DELAY                                                            */
/* ========================================================================== */

static inline void dp_udelay(u32 usec)
{
    u64 start = riscv_read_time();
    u64 target = start + (u64)usec;
    while (riscv_read_time() < target) cpu_nop();
}

static inline void dp_mdelay(u32 msec)
{
    u64 start = riscv_read_time();
    u64 target = start + (u64)msec * 1000ULL;
    while (riscv_read_time() < target) cpu_pause();
}

static inline void dp_wait_us(u32 us) { dp_udelay(us); }
static inline void dp_wait_ms(u32 ms) { dp_mdelay(ms); }

static inline bool dp_wait_until_us(bool (*cond)(void *), void *ctx, u32 timeout_us)
{
    u64 start = riscv_read_time();
    while (!cond(ctx)) {
        if ((riscv_read_time() - start) > (u64)timeout_us) return false;
        cpu_nop();
    }
    return true;
}

static inline bool dp_poll_reg_bits_eq(vaddr_t base, u32 off, u32 mask, u32 expected,
                                       u32 timeout_us)
{
    u64 start = riscv_read_time();
    while (1) {
        u32 v = dp_reg_read(base, off) & mask;
        if (v == expected) return true;
        if ((riscv_read_time() - start) > (u64)timeout_us) return false;
        cpu_nop();
    }
}

static inline bool dp_poll_reg_bits_ne(vaddr_t base, u32 off, u32 mask, u32 expected,
                                       u32 timeout_us)
{
    u64 start = riscv_read_time();
    while (1) {
        u32 v = dp_reg_read(base, off) & mask;
        if (v != expected) return true;
        if ((riscv_read_time() - start) > (u64)timeout_us) return false;
        cpu_nop();
    }
}

/* ========================================================================== */
/* 15. LOGGING & ERROR HANDLING                                                */
/* ========================================================================== */

#ifndef DP_LOG_LEVEL
#define DP_LOG_LEVEL 4
#endif

#define DP_LOG_ERR      0
#define DP_LOG_WARN     1
#define DP_LOG_INFO     2
#define DP_LOG_DBG      3
#define DP_LOG_TRACE    4

typedef void (*dp_log_fn_t)(int level, const char *fmt, ...);
extern dp_log_fn_t dp_log_fn;

#define dp_log(level, fmt, ...) \
    do { if ((level) <= DP_LOG_LEVEL && dp_log_fn) \
        dp_log_fn((level), fmt, ##__VA_ARGS__); } while (0)

#define dp_err(fmt, ...)   dp_log(DP_LOG_ERR,   "[DP ERR] " fmt, ##__VA_ARGS__)
#define dp_warn(fmt, ...)  dp_log(DP_LOG_WARN,  "[DP WRN] " fmt, ##__VA_ARGS__)
#define dp_info(fmt, ...)  dp_log(DP_LOG_INFO,  "[DP INF] " fmt, ##__VA_ARGS__)
#define dp_dbg(fmt, ...)   dp_log(DP_LOG_DBG,   "[DP DBG] " fmt, ##__VA_ARGS__)
#define dp_trace(fmt, ...) dp_log(DP_LOG_TRACE, "[DP TRC] " fmt, ##__VA_ARGS__)

/* Error codes */
#define DP_OK               0
#define DP_ERR_GENERIC      (-1)
#define DP_ERR_TIMEOUT      (-2)
#define DP_ERR_INVAL        (-3)
#define DP_ERR_NOMEM        (-4)
#define DP_ERR_BUSY         (-5)
#define DP_ERR_AUX_NACK     (-6)
#define DP_ERR_AUX_DEFER    (-7)
#define DP_ERR_AUX_CRC      (-8)
#define DP_ERR_LINK_TRAIN   (-9)
#define DP_ERR_EDID         (-10)
#define DP_ERR_NO_DEVICE    (-11)
#define DP_ERR_NOT_SUPPORTED (-12)
#define DP_ERR_PROTOCOL     (-13)
#define DP_ERR_HDCP         (-14)

/* ========================================================================== */
/* 16. DPCD REGISTER MAP (DisplayPort Configuration Data)                      */
/* ========================================================================== */

#define DP_DPCD_REV                    0x00000
#define DP_DPCD_REV_10                 0x10
#define DP_DPCD_REV_11                 0x11
#define DP_DPCD_REV_12                 0x12
#define DP_DPCD_REV_13                 0x13
#define DP_DPCD_REV_14                 0x14

#define DP_MAX_LINK_RATE               0x00001
#define DP_MAX_LANE_COUNT              0x00002
#define   DP_MAX_LANE_COUNT_MASK       0x1F
#define   DP_ENHANCED_FRAME_CAP        (1 << 7)
#define   DP_TPS3_SUPPORTED            (1 << 6)
#define   DP_MAX_DOWNSPREAD            0x00003
#define   DP_NO_AUX_HANDSHAKE_LINK_TRAINING (1 << 6)
#define DP_NORP                        0x00004
#define   DP_DOWNSTREAMPORT_PRESENT    (1 << 0)
#define   DP_MSA_TIMING_PAR_IGNORED    (1 << 6)
#define DP_DOWN_STREAM_PORT_COUNT      0x00007
#define   DP_OUI_SUPPORT               (1 << 7)

#define DP_RECEIVE_PORT_0_CAP          0x00008
#define   DP_LOCAL_EDID_PRESENT        (1 << 1)
#define   DP_ASSOCIATED_TO_PRECEDING_PORT (1 << 5)

#define DP_I2C_SPEED_CAP               0x0000C
#define DP_EDP_CONFIG_CAP              0x0000D
#define   DP_ALTERNATE_SCRAMBLER_RESET_CAP (1 << 0)
#define   DP_FRAMING_CHANGE_CAP        (1 << 1)
#define   DP_DPCD_DISPLAY_CONTROL_CAPABLE (1 << 3)

#define DP_TRAINING_AUX_RD_INTERVAL    0x0000E
#define DP_ADAPTER_CAP                 0x0000F

#define DP_FAUX_CAP                    0x00020
#define DP_MSTM_CAP                    0x00021
#define   DP_MST_CAP                   (1 << 0)

#define DP_NUM_AUDIO_ENDPOINTS         0x00022
#define DP_NUM_AUDIO_ENDPOINTS_MASK    0x0F
#define DP_NUM_AUDIO_ENDPOINTS_SHIFT   0

#define DP_GUID                        0x00030

#define DP_DPRX_FEATURE_ENUMERATION    0x00021

#define DP_DOWN_STREAM_PORT_0_CAP      0x00080
#define DP_DOWN_STREAM_PORT_1_CAP      0x00081
#define DP_DOWN_STREAM_PORT_2_CAP      0x00082
#define DP_DOWN_STREAM_PORT_3_CAP      0x00083

#define DP_SINK_COUNT                  0x00200
#define DP_SINK_COUNT_MASK             0x3F
#define DP_SINK_COUNT_ESI              0x00202

#define DP_DEVICE_SERVICE_IRQ_VECTOR   0x00201
#define   DP_REMOTE_CONTROL_CMD_SERVICE (1 << 0)
#define   DP_AUTOMATED_TEST_REQUEST    (1 << 1)
#define   DP_CP_IRQ                    (1 << 2)
#define   DP_MCCS_IRQ                  (1 << 3)
#define   DP_SINK_SPECIFIC_IRQ         (1 << 6)

#define DP_LANE0_1_STATUS              0x00202
#define DP_LANE2_3_STATUS              0x00203
#define   DP_LANE_CR_DONE              (1 << 0)
#define   DP_LANE_CHANNEL_EQ_DONE      (1 << 1)
#define   DP_LANE_SYMBOL_LOCKED        (1 << 2)
#define   DP_LANE_CR_DONE_SHIFT        0
#define   DP_LANE_CHANNEL_EQ_DONE_SHIFT 1
#define   DP_LANE_SYMBOL_LOCKED_SHIFT  2

#define DP_LANE_ALIGN_STATUS_UPDATED   0x00204
#define   DP_INTERLANE_ALIGN_DONE      (1 << 0)
#define   DP_DOWNSTREAM_PORT_STATUS_CHANGED (1 << 6)
#define   DP_LINK_STATUS_UPDATED       (1 << 7)

#define DP_SINK_STATUS                 0x00205
#define   DP_RX_PORT_0_LINK_STATUS     (1 << 0)
#define   DP_RX_PORT_1_LINK_STATUS     (1 << 1)
#define   DP_SINK_STATUS_ESI           (1 << 2)

#define DP_ADJUST_REQUEST_LANE0_1      0x00206
#define DP_ADJUST_REQUEST_LANE2_3      0x00207
#define   DP_ADJUST_VOLTAGE_SWING_LANE0_MASK  0x03
#define   DP_ADJUST_PRE_EMPHASIS_LANE0_MASK   0x0C
#define   DP_ADJUST_VOLTAGE_SWING_LANE0_SHIFT 0
#define   DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT  2
#define   DP_ADJUST_VOLTAGE_SWING_LANE1_MASK  0x30
#define   DP_ADJUST_PRE_EMPHASIS_LANE1_MASK   0xC0
#define   DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT 4
#define   DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT  6

#define DP_TRAINING_PATTERN_SET        0x00102
#define   DP_TRAINING_PATTERN_1        0x01
#define   DP_TRAINING_PATTERN_2        0x02
#define   DP_TRAINING_PATTERN_3        0x03
#define   DP_TRAINING_PATTERN_4        0x04
#define   DP_TRAINING_PATTERN_DISABLE  0x00
#define   DP_LINK_QUAL_PATTERN_DISABLE (0 << 2)
#define   DP_LINK_QUAL_PATTERN_ERROR   (1 << 2)
#define   DP_LINK_QUAL_PATTERN_PRBS7   (2 << 2)
#define   DP_LINK_QUAL_PATTERN_80BIT   (3 << 2)
#define   DP_LINK_QUAL_PATTERN_HBR2_EYE (4 << 2)
#define   DP_RECOVERED_CLOCK_OUT_EN    (1 << 4)
#define   DP_LINK_SCRAMBLING_DISABLE   (1 << 5)
#define   DP_SYMBOL_ERROR_COUNT_SEL_SHIFT 6

#define DP_TRAINING_LANE0_SET          0x00103
#define DP_TRAINING_LANE1_SET          0x00104
#define DP_TRAINING_LANE2_SET          0x00105
#define DP_TRAINING_LANE3_SET          0x00106
#define   DP_TRAIN_VOLTAGE_SWING_MASK  0x03
#define   DP_TRAIN_VOLTAGE_SWING_SHIFT 0
#define   DP_TRAIN_MAX_SWING_REACHED   (1 << 2)
#define   DP_TRAIN_PRE_EMPHASIS_MASK   0x18
#define   DP_TRAIN_PRE_EMPHASIS_SHIFT  3
#define   DP_TRAIN_PRE_EMPHASIS_LEVEL_SHIFT 3
#define   DP_TRAIN_MAX_PRE_EMPHASIS_REACHED (1 << 5)

#define DP_DOWNSPREAD_CTRL             0x00107
#define   DP_SPREAD_AMP_0_5            (1 << 4)
#define   DP_MSA_TIMING_PAR_IGNORE_EN  (1 << 7)

#define DP_MAIN_LINK_CHANNEL_CODING_SET 0x00108
#define   DP_SET_ANSI_8B10B            (1 << 0)
#define   DP_SET_ANSI_128B132B         (1 << 1)

#define DP_DOWN_STREAM_PORT_COUNT_REG  0x00109

#define DP_MSTM_CTRL                   0x00111
#define   DP_MST_EN                    (1 << 0)
#define   DP_UP_REQ_EN                 (1 << 1)
#define   DP_UPSTREAM_IS_SRC           (1 << 2)

#define DP_LINK_BW_SET                 0x00100
#define DP_LANE_COUNT_SET              0x00101
#define   DP_LANE_COUNT_SET_MASK       0x1F
#define   DP_ENHANCED_FRAME_EN         (1 << 7)
#define   DP_POST_LT_ADJ_REQ_LANE_0_1  0x00206
#define   DP_POST_LT_ADJ_REQ_LANE_2_3  0x00207

#define DP_PSR_STATUS                  0x00200
#define   DP_PSR_ENABLE                (1 << 0)
#define   DP_PSR_MAIN_LINK_ACTIVE      (1 << 1)
#define   DP_PSR_MAIN_LINK_ACTIVE_SHIFT 1

#define DP_PAYLOAD_ALLOCATE_SET        0x001C0
#define DP_PAYLOAD_ALLOCATE_START_TIME_SLOT 0x001C1
#define DP_PAYLOAD_ALLOCATE_TIME_SLOT_COUNT 0x001C2

#define DP_LINK_STATUS                  0x00200
#define DP_SINK_COUNT_0                0x00200

/* DPCD link configuration */
#define DP_LINK_CONFIGURATION          0x00100
#define DP_LINK_BW_SET_REG             0x00100
#define DP_LANE_COUNT_SET_REG          0x00101
#define DP_ENHANCED_FRAME_EN_REG       0x00101

/* DP sink power */
#define DP_SET_POWER                   0x00600
#define   DP_SET_POWER_D0              (0x01)
#define   DP_SET_POWER_D3              (0x02)
#define   DP_SET_POWER_MASK            0x03

#define DP_PSR_EN_CFG                  0x00170
#define DP_PSR_MAIN_LINK_ACTIVE_REG    0x00171

#define DP_EDP_CONFIGURATION_SET       0x0010A

#define DP_SINK_SPECIFIC_STATUS        0x02000

/* Audio */
#define DP_AUDIO_LATENCY               0x0000F
#define DP_AUDIO_DELAY                 0x00010

#define DP_LINK_BW_SET_1_62           0x06
#define DP_LINK_BW_SET_2_70           0x0A
#define DP_LINK_BW_SET_5_40           0x14
#define DP_LINK_BW_SET_8_10           0x1E

/* HDMI specific DPCD for DP-to-HDMI converters */
#define DP_HDMI_VIDEO_TIMING           0x00300
#define DP_HDMI_VIDEO_TIMING_1         0x00300
#define DP_HDMI_VIDEO_TIMING_2         0x00301
#define DP_HDMI_VIDEO_TIMING_3         0x00302
#define DP_HDMI_VIDEO_TIMING_4         0x00303
#define DP_HDMI_VIDEO_TIMING_5         0x00304
#define DP_HDMI_VIDEO_TIMING_6         0x00305
#define DP_HDMI_VIDEO_TIMING_7         0x00306
#define DP_HDMI_VIDEO_TIMING_8         0x00307

#define DP_HDMI_EDID_CTRL              0x0030A
#define DP_HDMI_EDID_I2C_ADDR          0x0030B
#define DP_HDMI_EDID_DATA_START        0x0030C
#define DP_HDMI_EDID_DATA_END          0x0030F

/* HDCP */
#define DP_HDCP_BCAPS                  0x00680
#define DP_HDCP_BSTATUS                0x00681
#define DP_HDCP_BINFO                  0x00682
#define DP_HDCP_KSV_FIFO               0x00683
#define DP_HDCP_AKSV                   0x00688
#define DP_HDCP_AN                    0x00689
#define DP_HDCP_V_PRIME               0x0068D
#define DP_HDCP_BCAPS2                 0x00680

/* HDCP 2.2 */
#define DP_HDCP_2_2_BCAPS              0x00692
#define   DP_HDCP_2_2_CAPABLE          (1 << 0)
#define   DP_HDCP_2_2_ENCRYPTED        (1 << 1)
#define   DP_HDCP_2_2_REPEATER         (1 << 2)

/* DSC */
#define DP_DSC_SUPPORT                 0x00060
#define   DP_DSC_SUPPORT_BIT           (1 << 0)
#define DP_DSC_REV                     0x00061
#define DP_DSC_RC_BUF_BLK_SIZE         0x00062
#define DP_DSC_RC_BUF_SIZE             0x00063
#define DP_DSC_SLICE_CAPABILITIES_1    0x00064
#define DP_DSC_LINE_BUF_BIT_DEPTH      0x00065
#define DP_DSC_BLK_PREDICTION_SUPPORT  0x00066
#define DP_DSC_MAX_BITS_PER_PIXEL_LOW  0x00067
#define DP_DSC_MAX_BITS_PER_PIXEL_HI   0x00068
#define DP_DSC_DEC_COLOR_FORMAT_CAP    0x00069
#define DP_DSC_DEC_COLOR_DEPTH_CAP     0x0006A
#define DP_DSC_PEAK_THROUGHPUT         0x0006B
#define DP_DSC_MAX_SLICE_WIDTH         0x0006C
#define DP_DSC_MAX_BPP                 0x0006D
#define DP_DSC_DECODER_VER             0x0006E

#define DP_DSC_ENABLE                  0x00160
#define   DP_DSC_ENABLE_BIT            (1 << 0)
#define DP_DSC_CFG                     0x00161
#define DP_DSC_INPUT_PIXEL_FORMAT      0x00162
#define DP_DSC_MAJOR_VER               0x00163
#define DP_DSC_MINOR_VER               0x00164

/* Panel Self Refresh */
#define DP_PSR_SUPPORT                 0x00070
#define DP_PSR_CAPS                    0x00071

/* ALPM (Advanced Link Power Management) */
#define DP_ALPM_CAPABILITY             0x00000

/* Branch device */
#define DP_BRANCH_DEVICE_ID            0x00050
#define DP_BRANCH_HW_REV               0x00051
#define DP_BRANCH_SW_REV               0x00052

/* OUI */
#define DP_SINK_OUI                    0x00030
#define DP_BRANCH_OUI                  0x00050

/* ========================================================================== */
/* 17. DP AUX CHANNEL                                                          */
/* ========================================================================== */

#define DP_AUX_CMD_NATIVE_WRITE        0x08
#define DP_AUX_CMD_NATIVE_READ         0x09
#define DP_AUX_CMD_I2C_WRITE           0x00
#define DP_AUX_CMD_I2C_READ            0x01
#define DP_AUX_CMD_I2C_WRITE_STATUS    0x02
#define DP_AUX_CMD_I2C_MOT             0x04
#define DP_AUX_CMD_I2C_READ_STATUS     0x05

#define DP_AUX_REPLY_ACK                0x00
#define DP_AUX_REPLY_NACK               0x01
#define DP_AUX_REPLY_DEFER              0x02
#define DP_AUX_REPLY_I2C_NACK           0x04
#define DP_AUX_REPLY_I2C_DEFER          0x08

#define DP_AUX_MAX_PAYLOAD_BYTES        16
#define DP_AUX_MAX_REPLY_BYTES          16
#define DP_AUX_MAX_MSG_BYTES            (DP_AUX_MAX_PAYLOAD_BYTES + 4)

#define DP_AUX_TIMEOUT_US_DEFAULT       10000
#define DP_AUX_TIMEOUT_US_NATIVE        3000
#define DP_AUX_TIMEOUT_US_I2C           50000
#define DP_AUX_RETRY_COUNT_DEFAULT      3
#define DP_AUX_REPLY_MAX_RETRIES        7

#define DP_AUX_ADDR_DPCD                0x0000
#define DP_AUX_ADDR_I2C_EDID            0x50
#define DP_AUX_ADDR_I2C_HDCP            0x3A

typedef struct __packed {
    u8  command;
    u8  address[3];
    u8  length;
    u8  data[DP_AUX_MAX_PAYLOAD_BYTES];
} dp_aux_request_t;

typedef struct __packed {
    u8  reply;
    u8  data[DP_AUX_MAX_REPLY_BYTES];
} dp_aux_reply_t;

typedef enum {
    DP_AUX_TRANSACTION_COMPLETE = 0,
    DP_AUX_TRANSACTION_PENDING,
    DP_AUX_TRANSACTION_FAILED,
    DP_AUX_TRANSACTION_DEFERRED,
    DP_AUX_TRANSACTION_NACK,
    DP_AUX_TRANSACTION_TIMEOUT,
    DP_AUX_TRANSACTION_INVALID
} dp_aux_status_t;

/* ========================================================================== */
/* 18. DP LINK TRAINING                                                        */
/* ========================================================================== */

#define DP_LT_MAX_PATTERN_1_ITER        10
#define DP_LT_MAX_PATTERN_2_ITER        10
#define DP_LT_MAX_PATTERN_3_ITER        10
#define DP_LT_MAX_CLOCK_RECOVERY_TRIES  5
#define DP_LT_MAX_CHANNEL_EQ_TRIES      5
#define DP_LT_LINK_RATE_TABLE_SIZE      4

#define DP_LT_VOLTAGE_SWING_0           0x00
#define DP_LT_VOLTAGE_SWING_1           0x01
#define DP_LT_VOLTAGE_SWING_2           0x02
#define DP_LT_VOLTAGE_SWING_3           0x03

#define DP_LT_PRE_EMPHASIS_0            0x00
#define DP_LT_PRE_EMPHASIS_1            0x01
#define DP_LT_PRE_EMPHASIS_2            0x02
#define DP_LT_PRE_EMPHASIS_3            0x03

typedef struct {
    u32 voltage_swing;
    u32 pre_emphasis;
} dp_lane_setting_t;

typedef struct {
    dp_lane_setting_t lanes[4];
} dp_link_training_settings_t;

typedef struct {
    u32 max_link_rate;
    u32 max_lane_count;
    bool enhanced_frame_cap;
    bool tps3_supported;
    bool no_aux_handshake;
    bool downstream_port_present;
} dp_link_caps_t;

typedef enum {
    DP_LINK_TRAINING_CR_PHASE = 0,
    DP_LINK_TRAINING_EQ_PHASE,
    DP_LINK_TRAINING_PHY_RSEN,
    DP_LINK_TRAINING_SUCCESS,
    DP_LINK_TRAINING_FAIL_CR,
    DP_LINK_TRAINING_FAIL_EQ,
    DP_LINK_TRAINING_FAIL_OVER_MAX,
    DP_LINK_TRAINING_FAIL_INVALID
} dp_link_training_state_t;

/* CR/EQ lookup tables for voltage swing + pre-emphasis */
typedef struct {
    u8 train_set;
    u8 sw[4];
    u8 pre[4];
} dp_link_phy_pattern_t;

/* ========================================================================== */
/* 19. DP MSA (Main Stream Attributes)                                         */
/* ========================================================================== */

#define DP_MSA_SDP                   0
#define DP_MSA_HDCP                  1

typedef struct __packed {
    u32 pixel_clock_khz;
    u16 hactive;
    u16 hblank;
    u16 hfront_porch;
    u16 hsync_width;
    u16 vactive;
    u16 vblank;
    u16 vfront_porch;
    u16 vsync_width;
    u16 hstart;
    u16 vstart;
    u16 hsync_polarity;
    u16 vsync_polarity;
    u8  component_depth;
    u8  color_format;
    u8  dynamic_range;
    u8  bpc;
    bool interlaced;
} dp_msa_t;

typedef struct __packed {
    u32 misc0;
    u32 misc1;
    u32 htotal;
    u32 hstart;
    u32 vtotal;
    u32 vstart;
    u32 hsw;
    u32 vsw;
    u32 mvid;
    u32 nvid;
} dp_msa_hw_regs_t;

/* MSA MISC0 bits */
#define DP_MSA_MISC0_STEREO_POLARITY  (1 << 7)
#define DP_MSA_MISC0_STEREO_MASK      (0x03 << 5)
#define DP_MSA_MISC0_STEREO_SHIFT     5
#define DP_MSA_MISC0_SYNC_CLK         (1 << 4)
#define DP_MSA_MISC0_FRAME_START      (1 << 3)
#define DP_MSA_MISC0_COLOR_FMT_MASK   (0x07 << 0)
#define DP_MSA_MISC0_COLOR_FMT_SHIFT  0

/* Color formats */
#define DP_MSA_COLOR_RGB              0x00
#define DP_MSA_COLOR_YUV444           0x01
#define DP_MSA_COLOR_YUV422           0x02
#define DP_MSA_COLOR_YUV420           0x03
#define DP_MSA_COLOR_RAW              0x04

/* Component bit depths */
#define DP_MSA_MISC1_BPC_MASK         (0x07 << 5)
#define DP_MSA_MISC1_BPC_SHIFT        5
#define DP_MSA_MISC1_BPC_6            0
#define DP_MSA_MISC1_BPC_8            1
#define DP_MSA_MISC1_BPC_10           2
#define DP_MSA_MISC1_BPC_12           3
#define DP_MSA_MISC1_BPC_16           4

#define DP_MSA_MISC1_DYNRANGE_MASK    (0x03 << 3)
#define DP_MSA_MISC1_DYNRANGE_SHIFT   3
#define DP_MSA_MISC1_DYNRANGE_VESA    0
#define DP_MSA_MISC1_DYNRANGE_CEA     1

#define DP_MSA_MISC1_YUV420_VBLANK_MASK (0x03 << 0)
#define DP_MSA_MISC1_YUV420_VBLANK_SHIFT 0

/* ========================================================================== */
/* 20. DP SECONDARY DATA PACKETS (SDP)                                         */
/* ========================================================================== */

#define DP_SDP_HEADER_SIZE            4
#define DP_SDP_MAX_PAYLOAD            32
#define DP_SDP_MAX_SIZE               (DP_SDP_HEADER_SIZE + DP_SDP_MAX_PAYLOAD)

#define DP_SDP_TYPE_AUDIO_STREAM      0x02
#define DP_SDP_TYPE_EXT              0x04
#define DP_SDP_TYPE_AUDIO_TIMESTAMP   0x07
#define DP_SDP_TYPE_VSC              0x05
#define DP_SDP_TYPE_ISRC             0x07
#define DP_SDP_TYPE_AVI              0x04
#define DP_SDP_TYPE_GCP              0x03
#define DP_SDP_TYPE_SPD              0x04
#define DP_SDP_TYPE_DRM              0x01
#define DP_SDP_TYPE_MPG              0x01
#define DP_SDP_TYPE_RESERVED         0x00

typedef struct __packed {
    u8  header[DP_SDP_HEADER_SIZE];
    u8  payload[DP_SDP_MAX_PAYLOAD];
} dp_sdp_t;

/* AVI InfoFrame */
typedef struct __packed {
    u8  header;
    u8  checksum;
    u8  version;
    u8  length;
    u8  ycc_quant;
    u8  ycc;
    u8  active_format;
    u8  bar_info;
    u8  picture_aspect;
    u8  colorimetry;
    u8  scaling;
    u8  rgb_quant;
    u8  ext_colorimetry;
    u8  itc;
    u8  video_format;
    u8  pixel_repeat;
    u8  line_offset;
    u8  top_bar;
    u8  bottom_bar;
    u8  left_bar;
    u8  right_bar;
} dp_avi_infoframe_t;

/* Audio InfoFrame */
typedef struct __packed {
    u8  header;
    u8  checksum;
    u8  version;
    u8  length;
    u8  audio_channel_count;
    u8  sample_rate;
    u8  sample_size;
    u8  coding_type;
    u8  channel_alloc;
    u8  downmix_inhibit;
} dp_audio_infoframe_t;

/* Generic InfoFrame packet header */
#define DP_INFOFRAME_HEADER            0x82
#define DP_INFOFRAME_VERSION           0x02
#define DP_INFOFRAME_AVI               0x02
#define DP_INFOFRAME_AUDIO             0x04
#define DP_INFOFRAME_MPEG              0x05
#define DP_INFOFRAME_VENDOR            0x01
#define DP_INFOFRAME_SPD               0x03

/* Audio infoframe channel count */
#define DP_AUDIO_CC_2CH                0x01
#define DP_AUDIO_CC_3CH                0x02
#define DP_AUDIO_CC_4CH                0x03
#define DP_AUDIO_CC_5CH                0x04
#define DP_AUDIO_CC_6CH                0x05
#define DP_AUDIO_CC_7CH                0x06
#define DP_AUDIO_CC_8CH                0x07

/* Audio sample rate */
#define DP_AUDIO_FREQ_32KHZ            0x01
#define DP_AUDIO_FREQ_44_1KHZ          0x02
#define DP_AUDIO_FREQ_48KHZ            0x03
#define DP_AUDIO_FREQ_88_2KHZ          0x04
#define DP_AUDIO_FREQ_96KHZ            0x05
#define DP_AUDIO_FREQ_176_4KHZ         0x06
#define DP_AUDIO_FREQ_192KHZ           0x07

/* Audio sample size */
#define DP_AUDIO_SAMPLE_16BIT          0x01
#define DP_AUDIO_SAMPLE_20BIT          0x02
#define DP_AUDIO_SAMPLE_24BIT          0x03

/* Audio coding types */
#define DP_AUDIO_CODING_REF_STREAM     0x00
#define DP_AUDIO_CODING_IEC61937        0x01
#define DP_AUDIO_CODING_AC3            0x02
#define DP_AUDIO_CODING_MPEG1          0x03
#define DP_AUDIO_CODING_MP3            0x04
#define DP_AUDIO_CODING_MPEG2          0x05
#define DP_AUDIO_CODING_AAC            0x06
#define DP_AUDIO_CODING_DTS            0x07
#define DP_AUDIO_CODING_ATRAC          0x08
#define DP_AUDIO_CODING_DSD            0x09
#define DP_AUDIO_CODING_EAC3           0x0A
#define DP_AUDIO_CODING_DTS_HD         0x0B
#define DP_AUDIO_CODING_MAT            0x0C
#define DP_AUDIO_CODING_DST            0x0D
#define DP_AUDIO_CODING_WMA_PRO        0x0E
#define DP_AUDIO_CODING_EXTENSION      0x0F

/* ========================================================================== */
/* 21. DP STATE MACHINE & ENUMS                                                */
/* ========================================================================== */

typedef enum {
    DP_STATE_RESET          = 0x00,
    DP_STATE_DETECTED       = 0x01,
    DP_STATE_AUX_READY      = 0x02,
    DP_STATE_LINK_TRAIN_CR  = 0x03,
    DP_STATE_LINK_TRAIN_EQ  = 0x04,
    DP_STATE_LINK_READY     = 0x05,
    DP_STATE_STREAM_READY   = 0x06,
    DP_STATE_ACTIVE_STREAM  = 0x07,
    DP_STATE_SUSPEND        = 0x08,
    DP_STATE_RESUME         = 0x09,
    DP_STATE_ERROR_FAULT    = 0xFF
} dp_state_machine_t;

typedef enum {
    DP_RATE_RBR   = 0x06,
    DP_RATE_HBR   = 0x0A,
    DP_RATE_HBR2  = 0x14,
    DP_RATE_HBR3  = 0x1E,
    DP_RATE_UHBR10 = 0x01,
    DP_RATE_UHBR20 = 0x02,
    DP_RATE_UHBR13_5 = 0x03
} dp_link_bandwidth_t;

typedef enum {
    DP_LANE_COUNT_1 = 1,
    DP_LANE_COUNT_2 = 2,
    DP_LANE_COUNT_4 = 4
} dp_lane_count_t;

typedef enum {
    DP_PIXEL_FORMAT_RGB      = 0,
    DP_PIXEL_FORMAT_YUV444   = 1,
    DP_PIXEL_FORMAT_YUV422   = 2,
    DP_PIXEL_FORMAT_YUV420   = 3,
    DP_PIXEL_FORMAT_RAW      = 4
} dp_pixel_format_t;

typedef enum {
    DP_BPC_6  = 6,
    DP_BPC_8  = 8,
    DP_BPC_10 = 10,
    DP_BPC_12 = 12,
    DP_BPC_16 = 16
} dp_bpc_t;

typedef enum {
    DP_COLORIMETRY_RGB_709       = 0,
    DP_COLORIMETRY_RGB_601       = 1,
    DP_COLORIMETRY_BT709         = 2,
    DP_COLORIMETRY_xvYCC601      = 3,
    DP_COLORIMETRY_xvYCC709      = 4,
    DP_COLORIMETRY_sYCC601       = 5,
    DP_COLORIMETRY_ADOBE_YCC601  = 6,
    DP_COLORIMETRY_ADOBE_RGB     = 7,
    DP_COLORIMETRY_BT2020_CYCC   = 8,
    DP_COLORIMETRY_BT2020_YCC    = 9,
    DP_COLORIMETRY_BT2020_RGB    = 10
} dp_colorimetry_t;

typedef enum {
    DP_DRM_STATIC   = 0,
    DP_DRM_DYNAMIC  = 1
} dp_dynamic_range_t;

/* ========================================================================== */
/* 22. DP AUDIO                                                                */
/* ========================================================================== */

#define DP_AUDIO_CHANNELS_MAX          8
#define DP_AUDIO_SAMPLE_RATES_MAX      8
#define DP_AUDIO_SDP_SIZE              32
#define DP_AUDIO_TIMESTAMP_SDP_SIZE    4

typedef struct __packed {
    u8  coding_type;
    u8  channel_count;
    u8  sample_rate;
    u8  sample_size;
} dp_audio_format_t;

typedef struct __packed {
    u8  channels;
    u8  sample_rate;
    u8  sample_size;
    u8  coding_type;
    u8  channel_allocation;
    u8  level_shift;
    bool downmix_inhibit;
} dp_audio_infoframe_params_t;

/* ELD (EDID Like Data) */
#define DP_ELD_MAX_SAD 15

typedef struct __packed {
    u8  version;
    u8  baseline_len;
    u8  speaker_allocation[3];
    u8  reserved[4];
    u8  port_id;
    u8  names_len;
    u8  connector_type;
    bool supports_ai;
    u8  audio_sync_delay;
    u8  audio_latency;
    u8  hdmi_vsdb_present;
    u8  hdmi_vsdb_len;
    u8  cec_address;
    u8  hdmi_vsdb_flags;
    u8  sad_count;
    u8  sads[DP_ELD_MAX_SAD * 3];
    u8  monitor_name[DP_ELD_MAX_SAD * 2];
} dp_eld_t;

/* Short Audio Descriptor (SAD) */
typedef struct __packed {
    u8 format;
    u8 channels;
    u8 sample_rates;
    u8 byte3;
} dp_sad_t;

#define DP_SAD_FORMAT_LPCM             0x01
#define DP_SAD_FORMAT_AC3              0x02
#define DP_SAD_FORMAT_MPEG1            0x03
#define DP_SAD_FORMAT_MP3              0x04
#define DP_SAD_FORMAT_MPEG2            0x05
#define DP_SAD_FORMAT_AAC_LC           0x06
#define DP_SAD_FORMAT_DTS              0x07
#define DP_SAD_FORMAT_ATRAC            0x08
#define DP_SAD_FORMAT_DSD              0x09
#define DP_SAD_FORMAT_EAC3             0x0A
#define DP_SAD_FORMAT_DTS_HD           0x0B
#define DP_SAD_FORMAT_MAT              0x0C
#define DP_SAD_FORMAT_DST              0x0D
#define DP_SAD_FORMAT_WMA_PRO          0x0E

/* ========================================================================== */
/* 23. DP MULTI-STREAM TRANSPORT (MST)                                         */
/* ========================================================================== */

#define DP_MST_MAX_STREAMS              64
#define DP_MST_MAX_VC_PAYLOAD           63
#define DP_MST_MAX_LINK_COUNT           8
#define DP_MST_SIDE_BAND_MSG_DATA_SIZE  48

#define DP_MST_LINK_ADDRESS(rad, link) \
    ((((rad) & 0xFF) << 8) | ((link) & 0xFF))

#define DP_MST_RELATIVE_ADDRESS_MASK    0xFF
#define DP_MST_LINK_COUNT_MASK          0xFF

#define DP_MST_MESSAGE_LINK_ADDRESS     0x00
#define DP_MST_MESSAGE_CONNECTION_STATUS 0x01
#define DP_MST_MESSAGE_RESOURCE_STATUS  0x02
#define DP_MST_MESSAGE_ENUM_PATH_RES    0x10
#define DP_MST_MESSAGE_ALLOCATE_PAYLOAD 0x20
#define DP_MST_MESSAGE_ENUM_PATH_RES_REPLY 0x30
#define DP_MST_MESSAGE_QUERY_PAYLOAD    0x40
#define DP_MST_MESSAGE_CLEAR_PAYLOAD_ID 0x50
#define DP_MST_MESSAGE_REMOTE_CMD       0x60

#define DP_MST_VC_PAYLOAD_ID_NULL       0x00
#define DP_MST_VC_PAYLOAD_ID_MAX        0x3F

#define DP_MST_LINK_BW_0                0x00
#define DP_MST_LINK_BW_1                0x01
#define DP_MST_LINK_BW_2                0x02
#define DP_MST_LINK_BW_3                0x03
#define DP_MST_LINK_BW_4                0x04
#define DP_MST_LINK_BW_5                0x05
#define DP_MST_LINK_BW_6                0x06
#define DP_MST_LINK_BW_7                0x07

typedef struct __packed {
    u8  port_number;
    u8  logical_or_physical;
    u8  ddc_port;
    u8  edid_present;
    u8  dp_present;
    u8  legacy_device;
    u8  dpcd_rev;
    u8  mcs;
    u16 peer_device_type;
    u8  num_sdp_streams;
    u8  num_sdp_stream_sinks;
    u8  bpp;
    u8  lane_count;
} dp_mst_port_info_t;

typedef struct __packed {
    u8  guid[16];
    u32 payloads;
    u32 payload_bw;
    u32 vc_payload_id;
    u32 link_count_total;
} dp_mst_branch_info_t;

typedef struct {
    u8  link_address[2];
    u8  message_type;
    u8  data[DP_MST_SIDE_BAND_MSG_DATA_SIZE];
} dp_mst_sideband_msg_t;

/* ========================================================================== */
/* 24. DP HDCP                                                                 */
/* ========================================================================== */

#define DP_HDCP_KSV_LEN                 5
#define DP_HDCP_KSV_FIFO_MAX            128
#define DP_HDCP_BSTATUS_MAX             (DP_HDCP_KSV_FIFO_MAX / DP_HDCP_KSV_LEN)

#define DP_HDCP_BSTATUS_B0              (1 << 0)
#define DP_HDCP_BSTATUS_B1              (1 << 1)
#define DP_HDCP_BSTATUS_B2              (1 << 2)

typedef struct __packed {
    u8  bcaps;
    u8  bstatus;
    u16 binfo;
    u8  ksv[DP_HDCP_KSV_LEN];
    u8  ksv_fifo[DP_HDCP_KSV_FIFO_MAX];
} dp_hdcp_state_t;

typedef struct __packed {
    u8  bcaps;
    u8  bstatus[2];
    u8  bksv[DP_HDCP_KSV_LEN];
    u32 ri;
    u8  r0[8];
    u8  v[20];
} dp_hdcp2_2_state_t;

#define DP_HDCP1_AKSV_LEN               5
#define DP_HDCP1_AN_LEN                 8
#define DP_HDCP1_V_LEN                  20
#define DP_HDCP2_RXSTATUS_LEN           2
#define DP_HDCP2_STREAMID_TYPE_0        0x00
#define DP_HDCP2_STREAMID_TYPE_1        0x01

#define DP_HDCP_AUTH_SUCCESS            0
#define DP_HDCP_AUTH_FAIL               (-1)
#define DP_HDCP_AUTH_REVOKED            (-2)
#define DP_HDCP_AUTH_NO_STREAM          (-3)

/* ========================================================================== */
/* 25. DP DSC (Display Stream Compression)                                     */
/* ========================================================================== */

#define DP_DSC_MAX_SLICES               32
#define DP_DSC_MAX_SLICE_WIDTH          4096
#define DP_DSC_MIN_BPP                  8

typedef enum {
    DP_DSC_BPP_8 = 8,
    DP_DSC_BPP_10 = 10,
    DP_DSC_BPP_12 = 12,
    DP_DSC_BPP_16 = 16
} dp_dsc_bpp_t;

typedef enum {
    DP_DSC_COLOR_RGB = 0,
    DP_DSC_COLOR_YUV444 = 1,
    DP_DSC_COLOR_YUV422 = 2,
    DP_DSC_COLOR_YUV420 = 3
} dp_dsc_color_format_t;

typedef enum {
    DP_DSC_LINEBUF_DEPTH_9 = 0,
    DP_DSC_LINEBUF_DEPTH_10 = 1,
    DP_DSC_LINEBUF_DEPTH_11 = 2,
    DP_DSC_LINEBUF_DEPTH_12 = 3,
    DP_DSC_LINEBUF_DEPTH_13 = 4,
    DP_DSC_LINEBUF_DEPTH_14 = 5
} dp_dsc_linebuf_depth_t;

typedef struct __packed {
    u8  dsc_version_major;
    u8  dsc_version_minor;
    u8  pps_identifier;
    u8  bits_per_component;
    u8  linebuf_depth;
    u8  block_pred_enable;
    u8  convert_rgb;
    u8  simple_422;
    u8  vbr_enable;
    u16 bits_per_pixel;
    u16 pic_width;
    u16 pic_height;
    u16 slice_width;
    u16 slice_height;
    u16 chunk_size;
    u16 initial_xmit_delay;
    u16 initial_dec_delay;
    u16 initial_scale_value;
    u16 scale_increment_interval;
    u16 scale_decrement_interval;
    u16 first_line_bpg_offset;
    u16 nfl_bpg_offset;
    u16 slice_bpg_offset;
    u16 initial_offset;
    u16 final_offset;
    u16 flatness_min_qp;
    u16 flatness_max_qp;
    u16 rc_model_size;
    u8  rc_edge_factor;
    u8  rc_quant_incr_limit0;
    u8  rc_quant_incr_limit1;
    u8  rc_tgt_offset_lo;
    u8  rc_tgt_offset_hi;
    u8  rc_buf_thresh[14];
    u8  rc_range_params[15][2];
} dp_dsc_pps_t;

#define DP_DSC_PPS_SIZE                 128

/* ========================================================================== */
/* 26. PSR / ALPM                                                              */
/* ========================================================================== */

#define DP_PSR_STATE_DISABLED           0
#define DP_PSR_STATE_INACTIVE           1
#define DP_PSR_STATE_ACTIVE             2
#define DP_PSR_STATE_EXIT               3

#define DP_PSR_MAINLINK_FULL             0
#define DP_PSR_MAINLINK_OFF              1

#define DP_PSR_SINK_SUPPORTED            (1 << 0)
#define DP_PSR_SETUP_TIME_330            (0 << 1)
#define DP_PSR_SETUP_TIME_275            (1 << 1)
#define DP_PSR_SETUP_TIME_220            (2 << 1)
#define DP_PSR_SETUP_TIME_165            (3 << 1)
#define DP_PSR_SETUP_TIME_110            (4 << 1)
#define DP_PSR_SETUP_TIME_55             (5 << 1)
#define DP_PSR_SETUP_TIME_0              (6 << 1)
#define DP_PSR_SETUP_TIME_SHIFT          1

#define DP_PSR_FRAME_CAPTURE             (1 << 4)
#define DP_PSR_Y_COORDINATE              (1 << 5)
#define DP_PSR_LINE_CAPTURE_INDICATION   (1 << 6)

#define DP_ALPM_ENABLE                   (1 << 0)
#define DP_ALPM_LOCK_ERROR_IRQ_HPD_ENABLE (1 << 1)
#define DP_ALPM_AUX_LESS_WAKEUP_ENABLE   (1 << 2)

/* ========================================================================== */
/* 27. DP HOT PLUG DETECT                                                      */
/* ========================================================================== */

#define DP_HPD_LONG_PULSE_THRESHOLD_MS   2
#define DP_HPD_IRQ_PULSE_MIN_US          500
#define DP_HPD_IRQ_PULSE_MAX_US          1000
#define DP_HPD_DEBOUNCE_MS               100

#define DP_IRQ_HPD                       (1 << 0)
#define DP_IRQ_HPD_LONG                  (1 << 1)
#define DP_IRQ_DPCD_STATUS_CHANGED       (1 << 2)

/* ========================================================================== */
/* 28. DP HARDWARE REGISTER MAPS                                               */
/* ========================================================================== */

typedef struct __packed {
    volatile u32 HW_VERSION;
    volatile u32 GLOBAL_CAPABILITIES;
    volatile u32 SYSTEM_CONTROL;
    volatile u32 SYSTEM_STATUS;
    volatile u32 INTERRUPT_MASK;
    volatile u32 INTERRUPT_STATUS;
    volatile u32 AUX_CH_CTL;
    volatile u32 AUX_CH_ADDR;
    volatile u32 AUX_CH_DATA;
    volatile u32 LINK_TRAINING_CTL;
    volatile u32 LANE_STATUS[4];
    volatile u32 LANE_SKEW[4];
    volatile u32 VOLTAGE_SWING_LEVEL[4];
    volatile u32 PRE_EMPHASIS_LEVEL[4];
    volatile u32 MSA_MISC0;
    volatile u32 MSA_MISC1;
    volatile u32 HORIZONTAL_TOTAL;
    volatile u32 HORIZONTAL_START;
    volatile u32 HORIZONTAL_ACTIVE;
    volatile u32 HORIZONTAL_BLANKING;
    volatile u32 VERTICAL_TOTAL;
    volatile u32 VERTICAL_START;
    volatile u32 VERTICAL_ACTIVE;
    volatile u32 VERTICAL_BLANKING;
    volatile u32 PIXEL_CLOCK_RATE;
    volatile u32 AUDIO_CONTROL;
    volatile u32 AUDIO_STATUS;
    volatile u32 STREAM_ASSIGNMENT;
    volatile u32 DP_RESERVED[192];
} dp_hardware_registers_t;

/* HDMI hardware register map */
typedef struct __packed {
    volatile u32 HDMI_HW_VERSION;
    volatile u32 HDMI_GLOBAL_CAPS;
    volatile u32 HDMI_SYSTEM_CONTROL;
    volatile u32 HDMI_SYSTEM_STATUS;
    volatile u32 HDMI_INTERRUPT_MASK;
    volatile u32 HDMI_INTERRUPT_STATUS;
    volatile u32 HDMI_DDC_CTL;
    volatile u32 HDMI_DDC_ADDR;
    volatile u32 HDMI_DDC_DATA;
    volatile u32 HDMI_TMDS_CTL;
    volatile u32 HDMI_TMDS_CLK;
    volatile u32 HDMI_PIXEL_REPEAT;
    volatile u32 HDMI_COLOR_DEPTH;
    volatile u32 HDMI_COLOR_FORMAT;
    volatile u32 HDMI_VIDEO_MODE;
    volatile u32 HDMI_H_SYNC;
    volatile u32 HDMI_V_SYNC;
    volatile u32 HDMI_H_ACTIVE;
    volatile u32 HDMI_V_ACTIVE;
    volatile u32 HDMI_H_TOTAL;
    volatile u32 HDMI_V_TOTAL;
    volatile u32 HDMI_AUDIO_CTL;
    volatile u32 HDMI_AUDIO_CH_STATUS;
    volatile u32 HDMI_AUDIO_DATA;
    volatile u32 HDMI_INFOFRAME_CTL;
    volatile u32 HDMI_INFOFRAME_DATA;
    volatile u32 HDMI_HDCP_CTL;
    volatile u32 HDMI_HDCP_STATUS;
    volatile u32 HDMI_CEC_CTL;
    volatile u32 HDMI_CEC_DATA;
    volatile u32 HDMI_RESERVED[160];
} hdmi_hardware_registers_t;

/* ========================================================================== */
/* 29. EDID STRUCTURES                                                         */
/* ========================================================================== */

#define EDID_BLOCK_SIZE                 128
#define EDID_MAX_EXTENSIONS             8
#define EDID_HEADER_SIZE                8
#define EDID_DTD_COUNT                  4
#define EDID_DESCRIPTOR_SIZE            18
#define EDID_CEA861_EXT_TAG             0x02
#define EDID_VTB_EXT_TAG                0x10
#define EDID_DISPLAYID_EXT_TAG          0x70

#define EDID_HEADER_PATTERN { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 }

typedef struct __packed {
    u16 pixel_clock;        /* 10 kHz units */
    u8  hactive_lo;
    u8  hblank_lo;
    u8  hactive_hblank_hi;
    u8  vactive_lo;
    u8  vblank_lo;
    u8  vactive_vblank_hi;
    u8  hfront_porch_lo;
    u8  hsync_pulse_lo;
    u8  vsync_offset_pulse_lo;
    u8  hsync_vsync_offset_pulse_hi;
    u8  hsize_lo;
    u8  vsize_lo;
    u8  hsize_vsize_hi;
    u8  hborder;
    u8  vborder;
    u8  flags;
} edid_detailed_timing_t;

#define EDID_DTD_INTERLACED            (1 << 7)
#define EDID_DTD_STEREO_MASK           (0x03 << 5)
#define EDID_DTD_STEREO_SHIFT          5
#define EDID_DTD_SYNC_TYPE_MASK        (0x03 << 3)
#define EDID_DTD_SYNC_TYPE_SHIFT       3
#define EDID_DTD_SYNC_SEPARATE         (1 << 2)
#define EDID_DTD_SYNC_POSITIVE_HSYNC   (1 << 1)
#define EDID_DTD_SYNC_POSITIVE_VSYNC   (1 << 0)

typedef struct __packed {
    u16 pixel_clock;
    u16 hactive;
    u16 hblank;
    u16 vactive;
    u16 vblank;
    u16 hfront_porch;
    u16 hsync_pulse;
    u16 vfront_porch;
    u16 vsync_pulse;
    bool hsync_positive;
    bool vsync_positive;
    bool interlaced;
} edid_timing_t;

typedef struct __packed {
    u8  header_pattern[8];
    u16 manufacturer_id;
    u16 product_code;
    u32 serial_number;
    u8  production_week;
    u8  production_year;
    u8  edid_major;
    u8  edid_minor;
    u8  input_parameters;
    u8  max_h_size_cm;
    u8  max_v_size_cm;
    u8  display_gamma;
    u8  feature_support;
    u8  chromaticity[10];
    u8  established_timings[3];
    u8  standard_timings[8][2];
    edid_detailed_timing_t detailed_timings[4];
    u8  ext_block_count;
    u8  checksum;
} edid_base_block_t;

#define EDID_INPUT_DIGITAL             (1 << 7)
#define EDID_INPUT_DFP1X               (1 << 0)
#define EDID_INPUT_BIT_DEPTH_MASK      (0x07 << 4)
#define EDID_INPUT_BIT_DEPTH_SHIFT     4
#define EDID_INPUT_INTERFACE_MASK      (0x0F << 0)
#define EDID_INPUT_INTERFACE_HDMI_A    (0x01)
#define EDID_INPUT_INTERFACE_HDMI_B    (0x02)
#define EDID_INPUT_INTERFACE_MDDI      (0x03)
#define EDID_INPUT_INTERFACE_DP        (0x05)

#define EDID_FEATURE_STANDBY           (1 << 7)
#define EDID_FEATURE_SUSPEND           (1 << 6)
#define EDID_FEATURE_ACTIVE_OFF        (1 << 5)
#define EDID_FEATURE_DISPLAY_TYPE_MASK (0x03 << 3)
#define EDID_FEATURE_DISPLAY_TYPE_SHIFT 3
#define EDID_FEATURE_RGB               (1 << 2)
#define EDID_FEATURE_PREFERRED_TIMING  (1 << 1)
#define EDID_FEATURE_GFT               (1 << 0)
#define EDID_FEATURE_CONTINUOUS_FREQ   (1 << 0)

/* Detailed timing descriptor types */
#define EDID_DTD_TAG_MONITOR_NAME      0xFC
#define EDID_DTD_TAG_MONITOR_RANGE      0xFD
#define EDID_DTD_TAG_SERIAL_NUMBER     0xFF
#define EDID_DTD_TAG_ASCII_STRING      0xFE
#define EDID_DTD_TAG_ADDITIONAL_WT     0xFB
#define EDID_DTD_TAG_COLOR_POINT       0xFB

typedef struct __packed {
    u16 min_vrate;
    u16 max_vrate;
    u16 min_hrate;
    u16 max_hrate;
    u16 max_pixel_clock_mhz10;
    u8  timing_support;
} edid_monitor_range_t;

typedef struct __packed {
    u8  tag;
    u8  flag1;
    u8  flag2;
    u8  flag3;
    u8  data[13];
} edid_descriptor_t;

/* CEA-861 extension block */
typedef struct __packed {
    u8  tag;
    u8  revision;
    u8  dtd_offset;
    u8  flags;
    u8  data[123];
    u8  checksum;
} cea861_extension_t;

#define CEA861_REV_1_3                0x03
#define CEA861_REV_1_4                0x04
#define CEA861_FLAG_UNDERSCAN         (1 << 7)
#define CEA861_FLAG_BASIC_AUDIO       (1 << 6)
#define CEA861_FLAG_YCC444            (1 << 5)
#define CEA861_FLAG_YCC422            (1 << 4)
#define CEA861_FLAG_AUDIO             (1 << 3)

/* CEA-861 data block tags */
#define CEA861_DB_TAG_AUDIO           0x01
#define CEA861_DB_TAG_VIDEO           0x02
#define CEA861_DB_TAG_VENDOR          0x03
#define CEA861_DB_TAG_SPEAKER         0x04
#define CEA861_DB_TAG_VESA            0x05
#define CEA861_DB_TAG_COLORIMETRY     0x07

/* CEA-861 Video Data Block (VIC) */
#define CEA861_VIC_MASK               0x7F
#define CEA861_VIC_NATIVE             (1 << 7)

/* CEA-861 HDMI Vendor Specific Data Block */
#define HDMI_IEEE_OUI                 0x000C03
#define HDMI_VSDB_LENGTH_1            5
#define HDMI_VSDB_LENGTH_2            6

typedef struct __packed {
    u8  block_id;
    u8  length;
    u8  oui[3];
    u8  phys_addr_lo;
    u8  phys_addr_hi;
    u8  deep_color_flags;
    u8  max_tmds_clock;
    u8  latency_present;
    u8  hdmi_video_present;
    u8  content_type_flags;
    u8  video_field_seq;
    u8  hdmi_vic_len;
    u8  hdmi_vic[8];
} hdmi_vsdb_t;

#define HDMI_VSDB_DC_Y444              (1 << 3)
#define HDMI_VSDB_DC_30BIT             (1 << 4)
#define HDMI_VSDB_DC_36BIT             (1 << 5)
#define HDMI_VSDB_DC_48BIT             (1 << 6)

/* Extended resolution / Colorimetry / HDR blocks */
#define CEA861_EXT_TAG_COLORIMETRY    0x05
#define CEA861_EXT_TAG_HDR_STATIC     0x06
#define CEA861_EXT_TAG_HDR_DYNAMIC    0x07
#define CEA861_EXT_TAG_VIDEO_CAP      0x00
#define CEA861_EXT_TAG_VCDB           0x00

/* ========================================================================== */
/* 30. DISPLAYID                                                               */
/* ========================================================================== */

#define DISPLAYID_HEADER_PATTERN       0x12
#define DISPLAYID_MAX_DATA_BLOCKS      32
#define DISPLAYID_BLOCK_SIZE           128

typedef struct __packed {
    u8  header[5];
    u8  block_count;
    u8  block_size;
    u8  checksum;
} displayid_header_t;

/* ========================================================================== */
/* 31. HDMI INFOFRAMES & REGISTERS                                             */
/* ========================================================================== */

#define HDMI_INFOFRAME_PKT_TYPE_NULL       0x00
#define HDMI_INFOFRAME_PKT_TYPE_AUDIO      0x01
#define HDMI_INFOFRAME_PKT_TYPE_AVI        0x02
#define HDMI_INFOFRAME_PKT_TYPE_SPD        0x03
#define HDMI_INFOFRAME_PKT_TYPE_MS         0x04
#define HDMI_INFOFRAME_PKT_TYPE_VS         0x05
#define HDMI_INFOFRAME_PKT_TYPE_ACP        0x06
#define HDMI_INFOFRAME_PKT_TYPE_ISRC1      0x07
#define HDMI_INFOFRAME_PKT_TYPE_ISRC2      0x08
#define HDMI_INFOFRAME_PKT_TYPE_GMD        0x09

#define HDMI_INFOFRAME_SIZE                14
#define HDMI_INFOFRAME_HEADER_SIZE         4

typedef struct __packed {
    u8  pkt_type;
    u8  version;
    u8  length;
    u8  checksum;
    u8  payload[HDMI_INFOFRAME_SIZE];
} hdmi_infoframe_t;

/* HDMI AVI InfoFrame payload */
typedef struct __packed {
    u8  checksum;
    u8  rgb_ycc;
    u8  active_format_aspect;
    u8  video_format;
    u8  ycc_quant_range;
    u8  pixel_repetition;
    u8  n_17_16;
    u8  n_15_8;
    u8  n_7_0;
    u8  m_7_0;
    u8  itc;
    u8  extended_colorimetry;
    u8  rgb_quantization;
    u8  non_uniform_scaling;
    u8  video_format_ext;
    u8  top_bar_lo;
    u8  top_bar_hi;
    u8  bottom_bar_lo;
    u8  bottom_bar_hi;
    u8  left_bar_lo;
    u8  left_bar_hi;
    u8  right_bar_lo;
    u8  right_bar_hi;
} hdmi_avi_infoframe_payload_t;

/* HDMI audio infoframe */
typedef struct __packed {
    u8  checksum;
    u8  audio_channel_count;
    u8  audio_coding_type;
    u8  sample_rate;
    u8  sample_size;
    u8  channel_alloc;
    u8  level_shift;
    u8  downmix_inhibit;
} hdmi_audio_infoframe_payload_t;

/* HDMI Video format */
#define HDMI_VIDEO_FORMAT_MASK             0x7F
#define HDMI_VIDEO_FORMAT_SHIFT            0
#define HDMI_VIDEO_FORMAT_EXT_MASK         0x7F
#define HDMI_VIDEO_FORMAT_EXT_SHIFT        0

/* HDMI color formats */
#define HDMI_COLOR_FORMAT_RGB              0
#define HDMI_COLOR_FORMAT_YUV422           1
#define HDMI_COLOR_FORMAT_YUV444           2
#define HDMI_COLOR_FORMAT_YUV420           3

/* HDMI color depth */
#define HDMI_COLOR_DEPTH_24                0
#define HDMI_COLOR_DEPTH_30                1
#define HDMI_COLOR_DEPTH_36                2
#define HDMI_COLOR_DEPTH_48                3

/* HDMI pixel repetition */
#define HDMI_PIXEL_REPEAT_NONE             0
#define HDMI_PIXEL_REPEAT_2X               1
#define HDMI_PIXEL_REPEAT_3X               2
#define HDMI_PIXEL_REPEAT_4X               3
#define HDMI_PIXEL_REPEAT_5X               4
#define HDMI_PIXEL_REPEAT_6X               5
#define HDMI_PIXEL_REPEAT_7X               6
#define HDMI_PIXEL_REPEAT_8X               7
#define HDMI_PIXEL_REPEAT_9X               8
#define HDMI_PIXEL_REPEAT_10X              9

/* HDMI TMDS clock */
#define HDMI_TMDS_CLOCK_MIN_KHZ            25000
#define HDMI_TMDS_CLOCK_MAX_KHZ            340000
#define HDMI_TMDS_CLOCK_MAX_HDMI_2_0_KHZ   600000

/* HDMI CEC */
#define HDMI_CEC_MAX_MSG_SIZE              16
#define HDMI_CEC_BROADCAST_ADDR            0x0F
#define HDMI_CEC_ADDR_TV                   0x00
#define HDMI_CEC_ADDR_RECORDING_1          0x01
#define HDMI_CEC_ADDR_RECORDING_2          0x02
#define HDMI_CEC_ADDR_TUNER_1              0x03
#define HDMI_CEC_ADDR_PLAYBACK_1           0x04
#define HDMI_CEC_ADDR_AUDIO_SYSTEM         0x05
#define HDMI_CEC_ADDR_TUNER_2              0x06
#define HDMI_CEC_ADDR_TUNER_3              0x07
#define HDMI_CEC_ADDR_PLAYBACK_2           0x08
#define HDMI_CEC_ADDR_RECORDING_3          0x09
#define HDMI_CEC_ADDR_TUNER_4              0x0A
#define HDMI_CEC_ADDR_PLAYBACK_3           0x0B
#define HDMI_CEC_ADDR_UNREGISTERED         0x0F

/* HDMI EDID */
#define HDMI_EDID_I2C_ADDR                 0x50
#define HDMI_DDC_SEGMENT_ADDR              0x30

/* HDMI HDCP */
#define HDMI_HDCP_KSV_LEN                  5
#define HDMI_HDCP_BSTATUS_LEN              2
#define HDMI_HDCP_MAX_KSV                  127

/* ========================================================================== */
/* 32. VIDEO TIMING CONFIGURATIONS                                             */
/* ========================================================================== */

typedef struct __packed {
    u32 pixel_clock;
    u16 hactive;
    u16 hblank;
    u16 vactive;
    u16 vblank;
    u16 hsync_offset;
    u16 hsync_pulse;
    u16 vsync_offset;
    u16 vsync_pulse;
    u8  refresh_hz;
    bool interlaced;
    bool hsync_positive;
    bool vsync_positive;
} dp_timing_spec_t;

typedef struct {
    u32 pixel_clock_khz;
    u32 htotal;
    u32 vtotal;
    u32 refresh_num;
    u32 refresh_den;
    u32 interlaced;
    u32 hfront_porch;
    u32 hsync_width;
    u32 hback_porch;
    u32 vfront_porch;
    u32 vsync_width;
    u32 vback_porch;
    u32 hsync_positive;
    u32 vsync_positive;
    u32 flags;
} dp_drm_mode_t;

#define DP_DRM_MODE_FLAG_PHSYNC         (1 << 0)
#define DP_DRM_MODE_FLAG_NHSYNC         (1 << 1)
#define DP_DRM_MODE_FLAG_PVSYNC         (1 << 2)
#define DP_DRM_MODE_FLAG_NVSYNC         (1 << 3)
#define DP_DRM_MODE_FLAG_INTERLACE      (1 << 4)
#define DP_DRM_MODE_FLAG_DBLSCAN        (1 << 5)
#define DP_DRM_MODE_FLAG_CSYNC          (1 << 6)
#define DP_DRM_MODE_FLAG_PCSYNC         (1 << 7)
#define DP_DRM_MODE_FLAG_NCSYNC         (1 << 8)
#define DP_DRM_MODE_FLAG_HSKEW          (1 << 9)
#define DP_DRM_MODE_FLAG_BCAST          (1 << 10)
#define DP_DRM_MODE_FLAG_PIXMUX         (1 << 11)
#define DP_DRM_MODE_FLAG_DBLCLK         (1 << 12)
#define DP_DRM_MODE_FLAG_CLKDIV2        (1 << 13)

/* ========================================================================== */
/* 33. CONTROLLER / STREAM STRUCTURES                                          */
/* ========================================================================== */

typedef struct {
    vaddr_t base_virtual_addr;
    paddr_t base_physical_addr;
    size_t  mapped_size;
    dp_state_machine_t current_state;
    dp_link_bandwidth_t bandwidth;
    dp_lane_count_t lanes;
    u32 active_pixel_clock;
    bool is_mst_active;
    bool initialized;
} dp_controller_t;

typedef struct {
    u8  stream_id;
    u8  vc_payload_id;
    dp_pixel_format_t pixel_format;
    u8  bpc;
    u8  colorimetry;
    u8  dynamic_range;
    dp_timing_spec_t timing;
    bool enabled;
    bool audio_enabled;
} dp_stream_t;

typedef struct {
    dp_controller_t *ctrl;
    dp_stream_t streams[DP_MST_MAX_STREAMS];
    u32 stream_count;
    dp_aux_status_t last_aux_status;
    dp_link_training_state_t lt_state;
    dp_link_caps_t sink_caps;
    dp_link_training_settings_t lt_settings;
    dp_eld_t eld;
    dp_hdcp_state_t hdcp;
    dp_dsc_pps_t dsc_pps;
    bool dsc_enabled;
    dp_spinlock_t lock;
} dp_driver_state_t;

typedef struct {
    u32 hdisplay;
    u32 hsync_start;
    u32 hsync_end;
    u32 htotal;
    u32 vdisplay;
    u32 vsync_start;
    u32 vsync_end;
    u32 vtotal;
    u32 clock_khz;
    u32 flags;
} hdmi_video_timing_t;

/* ========================================================================== */
/* 34. EDID PARSING HELPERS                                                    */
/* ========================================================================== */

static inline u16 edid_manufacturer_id(const edid_base_block_t *b)
{
    u16 raw = be16_to_cpu(b->manufacturer_id);
    u16 id = 0;
    id |= (raw & 0x7C00) >> 10;
    id |= (raw & 0x03E0) >> 5;
    id |= (raw & 0x001F);
    id = ((id & 0xFF) << 8) | ((id >> 8) & 0xFF);
    return id;
}

static inline u16 edid_decode_manufacturer(u16 raw)
{
    char a = (char)(((raw >> 10) & 0x1F) + 'A' - 1);
    char b = (char)(((raw >> 5)  & 0x1F) + 'A' - 1);
    char c = (char)(( raw        & 0x1F) + 'A' - 1);
    return (u16)(((u16)a << 8) | ((u16)b << 4) | (u16)c);
}

static inline bool edid_check_header(const edid_base_block_t *b)
{
    static const u8 hdr[8] = EDID_HEADER_PATTERN;
    return dp_mem_cmp(b->header_pattern, hdr, 8) == 0;
}

static inline u8 edid_checksum(const u8 *block, usize len)
{
    u8 sum = 0;
    for (usize i = 0; i < len; i++) sum += block[i];
    return (u8)(0 - sum);
}

static inline u32 edid_dtd_pixel_clock_khz(const edid_detailed_timing_t *d)
{
    return (u32)le16_to_cpu(d->pixel_clock) * 10;
}

static inline u32 edid_dtd_hactive(const edid_detailed_timing_t *d)
{
    return ((u32)d->hactive_lo) | (((u32)(d->hactive_hblank_hi & 0xF0)) << 4);
}
static inline u32 edid_dtd_hblank(const edid_detailed_timing_t *d)
{
    return ((u32)d->hblank_lo) | (((u32)(d->hactive_hblank_hi & 0x0F)) << 8);
}
static inline u32 edid_dtd_vactive(const edid_detailed_timing_t *d)
{
    return ((u32)d->vactive_lo) | (((u32)(d->vactive_vblank_hi & 0xF0)) << 4);
}
static inline u32 edid_dtd_vblank(const edid_detailed_timing_t *d)
{
    return ((u32)d->vblank_lo) | (((u32)(d->vactive_vblank_hi & 0x0F)) << 8);
}

/* ========================================================================== */
/* 35. TIMING CALCULATION HELPERS                                              */
/* ========================================================================== */

static inline u32 dp_calc_htotal(const dp_timing_spec_t *t)
{
    return (u32)t->hactive + (u32)t->hblank;
}

static inline u32 dp_calc_vtotal(const dp_timing_spec_t *t)
{
    return (u32)t->vactive + (u32)t->vblank;
}

static inline u32 dp_calc_refresh_hz(const dp_timing_spec_t *t)
{
    u64 total_pixels = (u64)dp_calc_htotal(t) * (u64)dp_calc_vtotal(t);
    if (!total_pixels) return 0;
    u64 v = ((u64)t->pixel_clock * 1000ULL) / total_pixels;
    if (t->interlaced) v *= 2;
    return (u32)v;
}

static inline u32 dp_calc_pixel_clock_khz(const dp_timing_spec_t *t)
{
    u64 total_pixels = (u64)dp_calc_htotal(t) * (u64)dp_calc_vtotal(t);
    return (u32)((total_pixels * (u64)t->refresh_hz) / 1000ULL);
}

static inline u32 dp_calc_bpp(u8 bpc, dp_pixel_format_t fmt)
{
    u32 bits = bpc;
    switch (fmt) {
    case DP_PIXEL_FORMAT_RGB:
    case DP_PIXEL_FORMAT_YUV444: return bits * 3;
    case DP_PIXEL_FORMAT_YUV422: return bits * 2;
    case DP_PIXEL_FORMAT_YUV420: return (bits * 3) / 2;
    default: return bits * 3;
    }
}

static inline u64 dp_calc_bitrate(const dp_timing_spec_t *t, u8 bpc, dp_pixel_format_t fmt)
{
    u64 px = (u64)t->pixel_clock * 1000ULL;
    return px * dp_calc_bpp(bpc, fmt);
}

static inline u64 dp_lane_bitrate_kbps(dp_link_bandwidth_t bw, dp_lane_count_t lanes)
{
    u32 per_lane;
    switch (bw) {
    case DP_RATE_RBR:  per_lane = 1620000; break;
    case DP_RATE_HBR:  per_lane = 2700000; break;
    case DP_RATE_HBR2: per_lane = 5400000; break;
    case DP_RATE_HBR3: per_lane = 8100000; break;
    default: per_lane = 0;
    }
    return (u64)per_lane * (u32)lanes;
}

static inline bool dp_check_bw_fits(const dp_timing_spec_t *t, u8 bpc,
                                    dp_pixel_format_t fmt,
                                    dp_link_bandwidth_t bw, dp_lane_count_t lanes)
{
    u64 needed = dp_calc_bitrate(t, bpc, fmt);
    u64 avail  = dp_lane_bitrate_kbps(bw, lanes) * 8ULL * 8ULL / 10ULL;
    return needed < (avail * 9ULL / 10ULL);
}

static inline dp_link_bandwidth_t dp_lowest_bw_for(const dp_timing_spec_t *t,
                                                    u8 bpc, dp_pixel_format_t fmt,
                                                    dp_lane_count_t lanes)
{
    static const dp_link_bandwidth_t rates[] = {
        DP_RATE_RBR, DP_RATE_HBR, DP_RATE_HBR2, DP_RATE_HBR3
    };
    for (u32 i = 0; i < 4; i++) {
        if (dp_check_bw_fits(t, bpc, fmt, rates[i], lanes)) return rates[i];
    }
    return DP_RATE_HBR3;
}

/* ========================================================================== */
/* 36. MSA HARDWARE PACKING HELPERS                                            */
/* ========================================================================== */

static inline u32 dp_build_msa_misc0(u32 color_fmt, bool sync_clk, bool frame_start)
{
    u32 v = 0;
    v |= (color_fmt & 0x07);
    if (sync_clk)    v |= DP_MSA_MISC0_SYNC_CLK;
    if (frame_start) v |= DP_MSA_MISC0_FRAME_START;
    return v;
}

static inline u32 dp_build_msa_misc1(u8 bpc, u8 dyn_range, u8 yuv420_vblank)
{
    u32 v = 0;
    u32 bpc_code = 0;
    switch (bpc) {
    case 6:  bpc_code = DP_MSA_MISC1_BPC_6;  break;
    case 8:  bpc_code = DP_MSA_MISC1_BPC_8;  break;
    case 10: bpc_code = DP_MSA_MISC1_BPC_10; break;
    case 12: bpc_code = DP_MSA_MISC1_BPC_12; break;
    case 16: bpc_code = DP_MSA_MISC1_BPC_16; break;
    default: bpc_code = DP_MSA_MISC1_BPC_8;  break;
    }
    v |= ((bpc_code << DP_MSA_MISC1_BPC_SHIFT) & DP_MSA_MISC1_BPC_MASK);
    v |= ((dyn_range & 0x03) << DP_MSA_MISC1_DYNRANGE_SHIFT) & DP_MSA_MISC1_DYNRANGE_MASK;
    v |= ((yuv420_vblank & 0x03) << DP_MSA_MISC1_YUV420_VBLANK_SHIFT);
    return v;
}

/* ========================================================================== */
/* 37. CONTROLLER HELPERS                                                      */
/* ========================================================================== */

static inline void dp_controller_init(dp_controller_t *ctrl, paddr_t phys,
                                       vaddr_t virt, size_t size)
{
    if (!ctrl) return;
    ctrl->base_physical_addr = phys;
    ctrl->base_virtual_addr = virt;
    ctrl->mapped_size = size;
    ctrl->current_state = DP_STATE_RESET;
    ctrl->bandwidth = DP_RATE_HBR;
    ctrl->lanes = DP_LANE_COUNT_4;
    ctrl->active_pixel_clock = 0;
    ctrl->is_mst_active = false;
    ctrl->initialized = true;
}

static inline u32 dp_core_read_reg(const dp_controller_t *ctrl, u32 offset)
{
    if (!ctrl || !ctrl->initialized) return 0;
    return readl((const volatile void __iomem *)(ctrl->base_virtual_addr + offset));
}

static inline void dp_core_write_reg(const dp_controller_t *ctrl, u32 offset, u32 val)
{
    if (!ctrl || !ctrl->initialized) return;
    writel(val, (volatile void __iomem *)(ctrl->base_virtual_addr + offset));
}

static inline bool dp_controller_is_ready(const dp_controller_t *ctrl)
{
    return ctrl && ctrl->initialized &&
           ctrl->current_state != DP_STATE_RESET &&
           ctrl->current_state != DP_STATE_ERROR_FAULT;
}

static inline void dp_controller_shutdown(dp_controller_t *ctrl)
{
    if (!ctrl) return;
    ctrl->current_state = DP_STATE_RESET;
    ctrl->active_pixel_clock = 0;
    ctrl->initialized = false;
}

/* ========================================================================== */
/* 38. AUX CHANNEL OPERATIONS                                                  */
/* ========================================================================== */

static inline u32 dp_aux_build_addr(u32 addr)
{
    return ((addr & 0x000FFFFF) << 0);
}

static inline void dp_aux_encode_header(dp_aux_request_t *req, u8 cmd, u32 addr, u8 len)
{
    req->command = cmd;
    req->address[0] = (u8)(addr & 0xFF);
    req->address[1] = (u8)((addr >> 8) & 0xFF);
    req->address[2] = (u8)((addr >> 16) & 0x0F);
    req->length = len;
}

static inline bool dp_aux_reply_acked(u8 reply)
{
    return (reply & 0x0F) == DP_AUX_REPLY_ACK ||
           (reply & 0x0F) == DP_AUX_REPLY_I2C_NACK;
}

static inline bool dp_aux_reply_deferred(u8 reply)
{
    return ((reply & 0x0F) == DP_AUX_REPLY_DEFER) ||
           ((reply & 0x0F) == DP_AUX_REPLY_I2C_DEFER);
}

static inline bool dp_aux_reply_nacked(u8 reply)
{
    return (reply & 0x0F) == DP_AUX_REPLY_NACK ||
           (reply & 0x0F) == DP_AUX_REPLY_I2C_NACK;
}

/* ========================================================================== */
/* 39. LINK TRAINING HELPERS                                                   */
/* ========================================================================== */

static inline u8 dp_lt_voltage_swing_to_hw(u32 level)
{
    switch (level) {
    case 0: return 0x00;
    case 1: return 0x01;
    case 2: return 0x02;
    case 3: return 0x03;
    default: return 0x00;
    }
}

static inline u8 dp_lt_pre_emphasis_to_hw(u32 level)
{
    switch (level) {
    case 0: return 0x00;
    case 1: return 0x01;
    case 2: return 0x02;
    case 3: return 0x03;
    default: return 0x00;
    }
}

static inline u8 dp_lt_build_lane_set(u32 vswing, u32 preemp, bool max_swing, bool max_preemp)
{
    u8 v = 0;
    v |= (dp_lt_voltage_swing_to_hw(vswing) & DP_TRAIN_VOLTAGE_SWING_MASK);
    v |= ((dp_lt_pre_emphasis_to_hw(preemp) << DP_TRAIN_PRE_EMPHASIS_SHIFT)
          & DP_TRAIN_PRE_EMPHASIS_MASK);
    if (max_swing)  v |= DP_TRAIN_MAX_SWING_REACHED;
    if (max_preemp) v |= DP_TRAIN_MAX_PRE_EMPHASIS_REACHED;
    return v;
}

static inline bool dp_lt_lane_cr_done(u8 status)
{
    return (status & DP_LANE_CR_DONE) != 0;
}
static inline bool dp_lt_lane_eq_done(u8 status)
{
    return (status & DP_LANE_CHANNEL_EQ_DONE) != 0;
}
static inline bool dp_lt_lane_symbol_locked(u8 status)
{
    return (status & DP_LANE_SYMBOL_LOCKED) != 0;
}

static inline bool dp_lt_all_lanes_cr_done(u8 lane01, u8 lane23, dp_lane_count_t n)
{
    u8 need = (n == DP_LANE_COUNT_1) ? 0x01 :
              (n == DP_LANE_COUNT_2) ? 0x03 : 0x0F;
    u8 got = ((lane01 & 0x01) ? 1 : 0) | ((lane01 & 0x10) ? 2 : 0) |
             ((lane23 & 0x01) ? 4 : 0) | ((lane23 & 0x10) ? 8 : 0);
    return (got & need) == need;
}

static inline bool dp_lt_all_lanes_eq_done(u8 lane01, u8 lane23, dp_lane_count_t n)
{
    u8 need = (n == DP_LANE_COUNT_1) ? 0x01 :
              (n == DP_LANE_COUNT_2) ? 0x03 : 0x0F;
    u8 got = ((lane01 & DP_LANE_CHANNEL_EQ_DONE) ? 1 : 0) |
             ((lane01 & (DP_LANE_CHANNEL_EQ_DONE << 4)) ? 2 : 0) |
             ((lane23 & DP_LANE_CHANNEL_EQ_DONE) ? 4 : 0) |
             ((lane23 & (DP_LANE_CHANNEL_EQ_DONE << 4)) ? 8 : 0);
    return (got & need) == need;
}

static inline void dp_lt_decode_adjust(u8 adj_lane01, u8 adj_lane23,
                                        u32 vs[4], u32 pe[4])
{
    vs[0] = adj_lane01 & DP_ADJUST_VOLTAGE_SWING_LANE0_MASK;
    pe[0] = (adj_lane01 & DP_ADJUST_PRE_EMPHASIS_LANE0_MASK) >>
            DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT;
    vs[1] = (adj_lane01 & DP_ADJUST_VOLTAGE_SWING_LANE1_MASK) >>
            DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT;
    pe[1] = (adj_lane01 & DP_ADJUST_PRE_EMPHASIS_LANE1_MASK) >>
            DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT;
    vs[2] = adj_lane23 & DP_ADJUST_VOLTAGE_SWING_LANE0_MASK;
    pe[2] = (adj_lane23 & DP_ADJUST_PRE_EMPHASIS_LANE0_MASK) >>
            DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT;
    vs[3] = (adj_lane23 & DP_ADJUST_VOLTAGE_SWING_LANE1_MASK) >>
            DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT;
    pe[3] = (adj_lane23 & DP_ADJUST_PRE_EMPHASIS_LANE1_MASK) >>
            DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT;
}

/* ========================================================================== */
/* 40. DPCD BIT RATE HELPERS                                                   */
/* ========================================================================== */

static inline u32 dp_bw_to_khz(dp_link_bandwidth_t bw)
{
    switch (bw) {
    case DP_RATE_RBR:  return 1620000;
    case DP_RATE_HBR:  return 2700000;
    case DP_RATE_HBR2: return 5400000;
    case DP_RATE_HBR3: return 8100000;
    case DP_RATE_UHBR10: return 10000000;
    case DP_RATE_UHBR20: return 20000000;
    case DP_RATE_UHBR13_5: return 13500000;
    default: return 0;
    }
}

static inline const char *dp_bw_to_str(dp_link_bandwidth_t bw)
{
    switch (bw) {
    case DP_RATE_RBR: return "RBR";
    case DP_RATE_HBR: return "HBR";
    case DP_RATE_HBR2: return "HBR2";
    case DP_RATE_HBR3: return "HBR3";
    case DP_RATE_UHBR10: return "UHBR10";
    case DP_RATE_UHBR20: return "UHBR20";
    case DP_RATE_UHBR13_5: return "UHBR13.5";
    default: return "UNKNOWN";
    }
}

static inline const char *dp_state_to_str(dp_state_machine_t s)
{
    switch (s) {
    case DP_STATE_RESET: return "RESET";
    case DP_STATE_DETECTED: return "DETECTED";
    case DP_STATE_AUX_READY: return "AUX_READY";
    case DP_STATE_LINK_TRAIN_CR: return "LT_CR";
    case DP_STATE_LINK_TRAIN_EQ: return "LT_EQ";
    case DP_STATE_LINK_READY: return "LINK_READY";
    case DP_STATE_STREAM_READY: return "STREAM_READY";
    case DP_STATE_ACTIVE_STREAM: return "ACTIVE_STREAM";
    case DP_STATE_SUSPEND: return "SUSPEND";
    case DP_STATE_RESUME: return "RESUME";
    case DP_STATE_ERROR_FAULT: return "FAULT";
    default: return "UNKNOWN";
    }
}

/* ========================================================================== */
/* 41. HDMI TIMING HELPERS                                                     */
/* ========================================================================== */

static inline u32 hdmi_htotal(const hdmi_video_timing_t *t) { return t->htotal; }
static inline u32 hdmi_vtotal(const hdmi_video_timing_t *t) { return t->vtotal; }

static inline u32 hdmi_pixel_clock_khz(const hdmi_video_timing_t *t)
{
    u64 tot = (u64)hdmi_htotal(t) * (u64)hdmi_vtotal(t) * 60ULL;
    return (u32)(tot / 1000ULL);
}

static inline u32 hdmi_tmds_clock_khz(const hdmi_video_timing_t *t, u8 bpp)
{
    return (u32)(((u64)hdmi_pixel_clock_khz(t) * bpp) / 24ULL);
}

static inline bool hdmi_tmds_clock_valid(u32 tmds_khz)
{
    return tmds_khz >= HDMI_TMDS_CLOCK_MIN_KHZ && tmds_khz <= HDMI_TMDS_CLOCK_MAX_KHZ;
}

/* ========================================================================== */
/* 42. AUDIO HELPERS                                                           */
/* ========================================================================== */

static inline u32 dp_audio_sample_rate_hz(u8 code)
{
    switch (code) {
    case DP_AUDIO_FREQ_32KHZ:    return 32000;
    case DP_AUDIO_FREQ_44_1KHZ:  return 44100;
    case DP_AUDIO_FREQ_48KHZ:    return 48000;
    case DP_AUDIO_FREQ_88_2KHZ:  return 88200;
    case DP_AUDIO_FREQ_96KHZ:    return 96000;
    case DP_AUDIO_FREQ_176_4KHZ: return 176400;
    case DP_AUDIO_FREQ_192KHZ:   return 192000;
    default: return 0;
    }
}

static inline u32 dp_audio_sample_size_bits(u8 code)
{
    switch (code) {
    case DP_AUDIO_SAMPLE_16BIT: return 16;
    case DP_AUDIO_SAMPLE_20BIT: return 20;
    case DP_AUDIO_SAMPLE_24BIT: return 24;
    default: return 0;
    }
}

static inline u32 dp_audio_calc_bandwidth(u8 channels, u8 sample_rate, u8 sample_size)
{
    u32 hz   = dp_audio_sample_rate_hz(sample_rate);
    u32 bits = dp_audio_sample_size_bits(sample_size);
    return channels * hz * bits;
}

/* ========================================================================== */
/* 43. HDCP HELPERS                                                            */
/* ========================================================================== */

static inline bool dp_hdcp_ksv_valid(const u8 *ksv)
{
    u32 ones = 0;
    for (int i = 0; i < DP_HDCP_KSV_LEN; i++)
        ones += dp_popcount32(ksv[i]);
    return ones == 20;
}

static inline u32 dp_hdcp_bstatus_depth(u8 bstatus)
{
    return (bstatus & 0x7F);
}

static inline bool dp_hdcp_bstatus_max_cascade(u8 bstatus)
{
    return (bstatus & 0x80) != 0;
}

/* ========================================================================== */
/* 44. INTERRUPT / IRQ HELPERS                                                 */
/* ========================================================================== */

#define DP_IRQ_HPD                    (1 << 0)
#define DP_IRQ_HPD_IRQ                (1 << 1)
#define DP_IRQ_AUX_DONE               (1 << 2)
#define DP_IRQ_AUX_ERROR              (1 << 3)
#define DP_IRQ_LT_DONE                (1 << 4)
#define DP_IRQ_LT_FAIL                (1 << 5)
#define DP_IRQ_VIDEO_FIFO_UNDERRUN    (1 << 6)
#define DP_IRQ_AUDIO_FIFO_UNDERRUN    (1 << 7)
#define DP_IRQ_HDCP                   (1 << 8)
#define DP_IRQ_MST                    (1 << 9)
#define DP_IRQ_PSR                    (1 << 10)
#define DP_IRQ_DSC                    (1 << 11)

typedef struct {
    u32 status;
    u32 mask;
    u32 errors;
    u32 irq_count;
} dp_irq_state_t;

static inline void dp_irq_init(dp_irq_state_t *i)
{
    i->status = 0; i->mask = 0; i->errors = 0; i->irq_count = 0;
}

static inline void dp_irq_mask(dp_irq_state_t *i, u32 bits)   { i->mask &= ~bits; }
static inline void dp_irq_unmask(dp_irq_state_t *i, u32 bits) { i->mask |= bits; }
static inline bool dp_irq_pending(const dp_irq_state_t *i, u32 bit) { return (i->status & bit) != 0; }
static inline void dp_irq_ack(dp_irq_state_t *i, u32 bit) { i->status &= ~bit; }
static inline void dp_irq_set(dp_irq_state_t *i, u32 bit) { i->status |= bit; i->irq_count++; }

/* ========================================================================== */
/* 45. CRC / CHECKSUM HELPERS                                                  */
/* ========================================================================== */

static inline u8 dp_crc8(const u8 *data, usize n, u8 poly, u8 init)
{
    u8 crc = init;
    for (usize i = 0; i < n; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++)
            crc = (u8)((crc & 0x80) ? ((crc << 1) ^ poly) : (crc << 1));
    }
    return crc;
}

static inline u16 dp_crc16_ccitt(const u8 *data, usize n)
{
    u16 crc = 0xFFFF;
    for (usize i = 0; i < n; i++) {
        crc ^= (u16)data[i] << 8;
        for (int b = 0; b < 8; b++)
            crc = (u16)((crc & 0x8000) ? ((crc << 1) ^ 0x1021) : (crc << 1));
    }
    return crc;
}

static inline u32 dp_crc32(const u8 *data, usize n)
{
    u32 crc = 0xFFFFFFFFu;
    for (usize i = 0; i < n; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++)
            crc = (crc >> 1) ^ (0xEDB88320u & (u32)(-(s32)(crc & 1)));
    }
    return ~crc;
}

/* ========================================================================== */
/* 46. DEBUG / TRACE REGISTERS                                                 */
/* ========================================================================== */

#define DP_DEBUG_REG_BASE             0x0F000
#define DP_DEBUG_CTL                  0x0F000
#define DP_DEBUG_STATUS               0x0F004
#define DP_DEBUG_AUX_CTL              0x0F008
#define DP_DEBUG_LT_CTL               0x0F00C
#define DP_DEBUG_VIDEO_CTL            0x0F010
#define DP_DEBUG_FIFO_STATUS          0x0F014
#define DP_DEBUG_ERR_COUNT            0x0F018
#define DP_DEBUG_TRACE_PTR            0x0F01C
#define DP_DEBUG_TRACE_DATA           0x0F020

/* ========================================================================== */
/* 47. MISC CONSTANTS                                                          */
/* ========================================================================== */

#define DP_DEFAULT_AUX_RETRIES         3
#define DP_DEFAULT_LT_RETRIES          3
#define DP_DEFAULT_POWER_ON_DELAY_MS   100
#define DP_DEFAULT_HPD_DELAY_MS        200
#define DP_DEFAULT_LINK_TRAIN_DELAY_US 100
#define DP_AUX_DEFER_BACKOFF_US        400

#define DP_MAX_BPC_SUPPORTED           16
#define DP_MAX_LANES                   4
#define DP_MAX_LINK_RATE_HZ            (8100000000ULL)
#define DP_MAX_PIXEL_CLOCK_KHZ         1200000
#define DP_MAX_H_TOTAL                 8192
#define DP_MAX_V_TOTAL                 8192

#define DP_MAX_DSC_SLICES              32
#define DP_MAX_DSC_BPP                 24
#define DP_MIN_DSC_BPP                 8

#define DP_HDMI_MAX_TMDS_KHZ           340000
#define DP_HDMI_MAX_TMDS_KHZ_HF        600000

/* ========================================================================== */
/* 48. PUBLIC API STUBS (defined in dp_core.c)                                 */
/* ========================================================================== */

/* AUX */
int  dp_aux_transaction(dp_controller_t *c, u8 cmd, u32 addr,
                        u8 *data, u8 len, u8 *reply);
int  dp_aux_read_dpcd(dp_controller_t *c, u32 addr, u8 *buf, u32 len);
int  dp_aux_write_dpcd(dp_controller_t *c, u32 addr, const u8 *buf, u32 len);
int  dp_aux_i2c_read(dp_controller_t *c, u8 i2c_addr, u32 offset,
                     u8 *buf, u32 len);
int  dp_aux_i2c_write(dp_controller_t *c, u8 i2c_addr, u32 offset,
                      const u8 *buf, u32 len);

/* Link */
int  dp_link_read_caps(dp_controller_t *c, dp_link_caps_t *caps);
int  dp_link_set_rate(dp_controller_t *c, dp_link_bandwidth_t bw, dp_lane_count_t lanes);
int  dp_link_train(dp_controller_t *c);
int  dp_link_train_cr(dp_controller_t *c);
int  dp_link_train_eq(dp_controller_t *c);
int  dp_link_verify_status(dp_controller_t *c, dp_link_training_state_t *st);

/* Stream */
int  dp_stream_set_timing(dp_controller_t *c, const dp_timing_spec_t *t);
int  dp_stream_enable(dp_controller_t *c, u8 stream_id);
int  dp_stream_disable(dp_controller_t *c, u8 stream_id);
int  dp_stream_update_msa(dp_controller_t *c, u8 stream_id);

/* EDID */
int  dp_edid_read(dp_controller_t *c, edid_base_block_t *out,
                  cea861_extension_t *cea, u32 cea_len);
int  dp_edid_parse(const edid_base_block_t *b, edid_timing_t *out, u32 max);
int  dp_edid_find_dtd(const edid_base_block_t *b, u32 h, u32 v,
                      edid_detailed_timing_t *out);

/* Audio */
int  dp_audio_configure(dp_controller_t *c, u8 stream_id,
                        const dp_audio_infoframe_params_t *p);
int  dp_audio_enable(dp_controller_t *c, u8 stream_id);
int  dp_audio_disable(dp_controller_t *c, u8 stream_id);

/* HDCP */
int  dp_hdcp_enable(dp_controller_t *c, u32 version);
int  dp_hdcp_disable(dp_controller_t *c);
int  dp_hdcp_authenticate(dp_controller_t *c);
int  dp_hdcp_status(dp_controller_t *c, dp_hdcp_state_t *out);

/* PSR / ALPM */
int  dp_psr_enable(dp_controller_t *c);
int  dp_psr_disable(dp_controller_t *c);
int  dp_alpm_configure(dp_controller_t *c, u32 cfg);

/* MST */
int  dp_mst_enable(dp_controller_t *c);
int  dp_mst_disable(dp_controller_t *c);
int  dp_mst_allocate_vcpi(dp_controller_t *c, u8 vcpi, u32 pbn);
int  dp_mst_clear_vcpi(dp_controller_t *c, u8 vcpi);

/* DSC */
int  dp_dsc_configure(dp_controller_t *c, const dp_dsc_pps_t *pps);
int  dp_dsc_enable(dp_controller_t *c);
int  dp_dsc_disable(dp_controller_t *c);

/* HDMI */
int  hdmi_set_timing(dp_controller_t *c, const hdmi_video_timing_t *t);
int  hdmi_set_color(dp_controller_t *c, u8 format, u8 depth);
int  hdmi_send_avi_infoframe(dp_controller_t *c, const dp_avi_infoframe_t *avi);
int  hdmi_send_audio_infoframe(dp_controller_t *c, const dp_audio_infoframe_t *aif);
int  hdmi_edid_read(dp_controller_t *c, u8 *buf, u32 len);

#ifdef __cplusplus
}
#endif

#endif /* DP_CORE_H */
