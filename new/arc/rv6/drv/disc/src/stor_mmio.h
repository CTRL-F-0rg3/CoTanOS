/*
 * ============================================================================
 *  STOR_MMIO.H - Low-level MMIO / CSR / barrier primitives for CoTanOS
 *  Architecture: RISC-V (RV32 / RV64)
 *  Purpose: Shared foundation for all CoTanOS drivers (no libc, no DP deps).
 *
 *  This header is standalone and intended to be reused by other driver
 *  suites. Everything is `stor_`-prefixed to avoid link/symbol collisions
 *  when a translation unit includes multiple driver headers.
 * ============================================================================
 */

#ifndef STOR_MMIO_H
#define STOR_MMIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ========================================================================== */
/* 1. COMPILER / PORTABILITY MODIFIERS                                         */
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
#ifndef __weak
#define __weak __attribute__((weak))
#endif
#ifndef __noreturn
#define __noreturn __attribute__((noreturn))
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
/* 2. BASIC TYPES                                                              */
/* ========================================================================== */

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef uintptr_t vaddr_t;
typedef uintptr_t paddr_t;
typedef uint64_t  dma_addr_t;
typedef size_t    usize;
typedef ptrdiff_t ssize;

#ifndef NULL
#define NULL ((void *)0)
#endif

/* ========================================================================== */
/* 3. BIT MANIPULATION PRIMITIVES                                              */
/* ========================================================================== */

#define STOR_BIT(nr)              (1UL << (nr))
#define STOR_BIT_ULL(nr)          (1ULL << (nr))
#define STOR_BITS_PER_LONG        (sizeof(unsigned long) * 8)
#define STOR_BITS_PER_LONG_LONG   (sizeof(unsigned long long) * 8)

#define STOR_GENMASK(h, l) \
    (((~0UL) << (l)) & (~0UL >> (STOR_BITS_PER_LONG - 1 - (h))))
#define STOR_GENMASK_ULL(h, l) \
    (((~0ULL) << (l)) & (~0ULL >> (STOR_BITS_PER_LONG_LONG - 1 - (h))))

#define STOR_FIELD_GET(mask, reg)  (((reg) & (mask)) / ((mask) & ~((mask) << 1)))
#define STOR_FIELD_PREP(mask, val) (((val) * ((mask) & ~((mask) << 1))) & (mask))

static __always_inline u32 stor_bf_shift(u32 mask)
{
    u32 s = 0;
    while (mask && !(mask & 1u)) { mask >>= 1; s++; }
    return s;
}

static __always_inline u32 stor_bf_get(u32 val, u32 mask)
{ return (val & mask) >> stor_bf_shift(mask); }

static __always_inline u32 stor_bf_prep(u32 val, u32 mask)
{ return (val << stor_bf_shift(mask)) & mask; }

#define stor_bf_set(val, mask, field) \
    (((val) & ~(mask)) | stor_bf_prep((field), (mask)))

static __always_inline u32 stor_popcount32(u32 x)
{
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0F0F0F0Fu;
    return (x * 0x01010101u) >> 24;
}

static __always_inline u32 stor_popcount64(u64 x)
{
    return stor_popcount32((u32)(x & 0xFFFFFFFFu)) +
           stor_popcount32((u32)(x >> 32));
}

static __always_inline u32 stor_clz32(u32 x)
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

static __always_inline u32 stor_ctz32(u32 x)
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

static __always_inline u32 stor_ilog2(u32 x) { return 31u - stor_clz32(x); }

static __always_inline u32 stor_align_up(u32 v, u32 a) { return (v + a - 1u) & ~(a - 1u); }
static __always_inline u32 stor_align_down(u32 v, u32 a) { return v & ~(a - 1u); }
static __always_inline bool stor_align_check(u32 v, u32 a) { return (v & (a - 1u)) == 0; }

static __always_inline bool stor_is_pow2(u32 x) { return x && !(x & (x - 1u)); }

static __always_inline u32 stor_roundup_pow2(u32 x)
{
    if (x <= 1u) return 1u;
    x--;
    x |= x >> 1; x |= x >> 2; x |= x >> 4; x |= x >> 8; x |= x >> 16;
    return x + 1u;
}

static __always_inline u32 stor_min_u32(u32 a, u32 b) { return a < b ? a : b; }
static __always_inline u32 stor_max_u32(u32 a, u32 b) { return a > b ? a : b; }
static __always_inline u64 stor_min_u64(u64 a, u64 b) { return a < b ? a : b; }
static __always_inline u64 stor_max_u64(u64 a, u64 b) { return a > b ? a : b; }

static __always_inline u32 stor_clamp_u32(u32 v, u32 lo, u32 hi)
{ return v < lo ? lo : (v > hi ? hi : v); }

static __always_inline u32 stor_abs_diff_u32(u32 a, u32 b)
{ return a > b ? a - b : b - a; }

/* ========================================================================== */
/* 4. BYTE ORDER                                                               */
/* ========================================================================== */

static __always_inline u16 stor_bswap16(u16 x) { return (u16)((x >> 8) | (x << 8)); }
static __always_inline u32 stor_bswap32(u32 x)
{
    return ((x & 0xFF000000u) >> 24) | ((x & 0x00FF0000u) >> 8) |
           ((x & 0x0000FF00u) << 8)  | ((x & 0x000000FFu) << 24);
}
static __always_inline u64 stor_bswap64(u64 x)
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

#define stor_cpu_to_le16(x) ((u16)(x))
#define stor_cpu_to_le32(x) ((u32)(x))
#define stor_cpu_to_le64(x) ((u64)(x))
#define stor_le16_to_cpu(x) ((u16)(x))
#define stor_le32_to_cpu(x) ((u32)(x))
#define stor_le64_to_cpu(x) ((u64)(x))

#define stor_cpu_to_be16(x) stor_bswap16((u16)(x))
#define stor_cpu_to_be32(x) stor_bswap32((u32)(x))
#define stor_cpu_to_be64(x) stor_bswap64((u64)(x))
#define stor_be16_to_cpu(x) stor_bswap16((u16)(x))
#define stor_be32_to_cpu(x) stor_bswap32((u32)(x))
#define stor_be64_to_cpu(x) stor_bswap64((u64)(x))

/* ========================================================================== */
/* 5. MEMORY COPY HELPERS (no libc)                                            */
/* ========================================================================== */

static __always_inline void stor_mem_copy(void *dst, const void *src, usize n)
{
    u8 *d = (u8 *)dst; const u8 *s = (const u8 *)src;
    while (n--) *d++ = *s++;
}

static __always_inline void stor_mem_set(void *dst, u8 v, usize n)
{
    u8 *d = (u8 *)dst;
    while (n--) *d++ = v;
}

static __always_inline int stor_mem_cmp(const void *a, const void *b, usize n)
{
    const u8 *x = (const u8 *)a; const u8 *y = (const u8 *)b;
    while (n--) { if (*x != *y) return (int)*x - (int)*y; x++; y++; }
    return 0;
}

static __always_inline bool stor_mem_is_zero(const void *p, usize n)
{
    const u8 *b = (const u8 *)p;
    while (n--) if (*b++) return false;
    return true;
}

/* ========================================================================== */
/* 6. RISC-V BARRIERS                                                          */
/* ========================================================================== */

#define stor_riscv_mb()      __asm__ __volatile__ ("fence iorw, iorw" ::: "memory")
#define stor_riscv_rmb()     __asm__ __volatile__ ("fence ir, ir"     ::: "memory")
#define stor_riscv_wmb()     __asm__ __volatile__ ("fence ow, ow"     ::: "memory")
#define stor_riscv_smp_mb()  __asm__ __volatile__ ("fence rw, rw"     ::: "memory")
#define stor_riscv_smp_rmb() __asm__ __volatile__ ("fence r, r"       ::: "memory")
#define stor_riscv_smp_wmb() __asm__ __volatile__ ("fence w, w"       ::: "memory")
#define stor_riscv_fence_i() __asm__ __volatile__ ("fence.i"          ::: "memory")

#define stor_io_br()  do {} while (0)
#define stor_io_ar()  stor_riscv_mb()
#define stor_io_bw()  stor_riscv_mb()
#define stor_io_aw()  do {} while (0)

#define stor_rmb()     stor_riscv_rmb()
#define stor_wmb()     stor_riscv_wmb()
#define stor_mb()      stor_riscv_mb()
#define stor_smp_mb()  stor_riscv_smp_mb()
#define stor_smp_rmb() stor_riscv_smp_rmb()
#define stor_smp_wmb() stor_riscv_smp_wmb()

#define stor_barrier() __asm__ __volatile__("" ::: "memory")

#define STOR_READ_ONCE(x)  (*(volatile __typeof__(x) *)&(x))
#define STOR_WRITE_ONCE(x, v) (*(volatile __typeof__(x) *)&(x) = (v))

/* ========================================================================== */
/* 7. RAW MMIO                                                                 */
/* ========================================================================== */

static __always_inline void stor_raw_writeb(u8 v, volatile void __iomem *a)
{ __asm__ __volatile__ ("sb %0, 0(%1)" : : "r"(v), "r"(a) : "memory"); }
static __always_inline void stor_raw_writew(u16 v, volatile void __iomem *a)
{ __asm__ __volatile__ ("sh %0, 0(%1)" : : "r"(v), "r"(a) : "memory"); }
static __always_inline void stor_raw_writel(u32 v, volatile void __iomem *a)
{ __asm__ __volatile__ ("sw %0, 0(%1)" : : "r"(v), "r"(a) : "memory"); }
static __always_inline void stor_raw_writeq(u64 v, volatile void __iomem *a)
{ __asm__ __volatile__ ("sd %0, 0(%1)" : : "r"(v), "r"(a) : "memory"); }

static __always_inline u8 stor_raw_readb(const volatile void __iomem *a)
{ u8 v; __asm__ __volatile__ ("lb %0, 0(%1)" : "=r"(v) : "r"(a) : "memory"); return v; }
static __always_inline u16 stor_raw_readw(const volatile void __iomem *a)
{ u16 v; __asm__ __volatile__ ("lh %0, 0(%1)" : "=r"(v) : "r"(a) : "memory"); return v; }
static __always_inline u32 stor_raw_readl(const volatile void __iomem *a)
{ u32 v; __asm__ __volatile__ ("lw %0, 0(%1)" : "=r"(v) : "r"(a) : "memory"); return v; }
static __always_inline u64 stor_raw_readq(const volatile void __iomem *a)
{ u64 v; __asm__ __volatile__ ("ld %0, 0(%1)" : "=r"(v) : "r"(a) : "memory"); return v; }

/* ========================================================================== */
/* 8. ORDERED MMIO ACCESSORS (with fence)                                      */
/* ========================================================================== */

static __always_inline u8  stor_readb(const volatile void __iomem *a)
{ u8 v;  stor_io_br(); v = stor_raw_readb(a); stor_io_ar(); return v; }
static __always_inline u16 stor_readw(const volatile void __iomem *a)
{ u16 v; stor_io_br(); v = stor_raw_readw(a); stor_io_ar(); return v; }
static __always_inline u32 stor_readl(const volatile void __iomem *a)
{ u32 v; stor_io_br(); v = stor_raw_readl(a); stor_io_ar(); return v; }
static __always_inline u64 stor_readq(const volatile void __iomem *a)
{ u64 v; stor_io_br(); v = stor_raw_readq(a); stor_io_ar(); return v; }

static __always_inline void stor_writeb(u8 v,  volatile void __iomem *a)
{ stor_io_bw(); stor_raw_writeb(v, a); stor_io_aw(); }
static __always_inline void stor_writew(u16 v, volatile void __iomem *a)
{ stor_io_bw(); stor_raw_writew(v, a); stor_io_aw(); }
static __always_inline void stor_writel(u32 v, volatile void __iomem *a)
{ stor_io_bw(); stor_raw_writel(v, a); stor_io_aw(); }
static __always_inline void stor_writeq(u64 v, volatile void __iomem *a)
{ stor_io_bw(); stor_raw_writeq(v, a); stor_io_aw(); }

/* ========================================================================== */
/* 9. REGISTER ACCESS HELPERS                                                  */
/* ========================================================================== */

static __always_inline u32 stor_reg_read(vaddr_t base, u32 off)
{ return stor_readl((const volatile void __iomem *)(base + off)); }

static __always_inline void stor_reg_write(vaddr_t base, u32 off, u32 v)
{ stor_writel(v, (volatile void __iomem *)(base + off)); }

static __always_inline void stor_reg_update(vaddr_t base, u32 off, u32 mask, u32 val)
{
    u32 cur = stor_reg_read(base, off);
    cur &= ~mask;
    cur |= (val & mask);
    stor_reg_write(base, off, cur);
}

static __always_inline void stor_reg_set_bits(vaddr_t base, u32 off, u32 bits)
{ stor_reg_write(base, off, stor_reg_read(base, off) | bits); }

static __always_inline void stor_reg_clear_bits(vaddr_t base, u32 off, u32 bits)
{ stor_reg_write(base, off, stor_reg_read(base, off) & ~bits); }

static __always_inline void stor_reg_set_field(vaddr_t base, u32 off, u32 mask, u32 field)
{ stor_reg_update(base, off, mask, stor_bf_prep(field, mask)); }

static __always_inline u32 stor_reg_get_field(vaddr_t base, u32 off, u32 mask)
{ return stor_bf_get(stor_reg_read(base, off), mask); }

/* ========================================================================== */
/* 10. RISC-V CSR / CPU CONTROL                                                */
/* ========================================================================== */

static __always_inline void stor_wfi(void)       { __asm__ __volatile__ ("wfi"); }
static __always_inline void stor_nop(void)       { __asm__ __volatile__ ("nop"); }
static __always_inline void stor_pause(void)     { __asm__ __volatile__ ("pause"); }
static __always_inline void stor_fence_i(void)   { stor_riscv_fence_i(); }
static __always_inline void stor_ebreak(void)    { __asm__ __volatile__ ("ebreak"); }

static __always_inline u64 stor_read_time(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, time" : "=r"(v)); return v; }

static __always_inline u64 stor_read_cycle(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, cycle" : "=r"(v)); return v; }

static __always_inline u64 stor_read_instret(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, instret" : "=r"(v)); return v; }

static __always_inline u64 stor_read_mhartid(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, mhartid" : "=r"(v)); return v; }

static __always_inline u64 stor_read_mstatus(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, mstatus" : "=r"(v)); return v; }

static __always_inline void stor_write_mstatus(u64 v)
{ __asm__ __volatile__ ("csrw mstatus, %0" : : "r"(v)); }

static __always_inline u64 stor_read_mie(void)
{ u64 v; __asm__ __volatile__ ("csrr %0, mie" : "=r"(v)); return v; }

static __always_inline void stor_write_mie(u64 v)
{ __asm__ __volatile__ ("csrw mie, %0" : : "r"(v)); }

static __always_inline void stor_mie_enable(u32 bit)
{ stor_write_mie(stor_read_mie() | (1ULL << bit)); }

static __always_inline void stor_mie_disable(u32 bit)
{ stor_write_mie(stor_read_mie() & ~(1ULL << bit)); }

#define STOR_IRQ_M_SOFT   3
#define STOR_IRQ_M_TIMER  7
#define STOR_IRQ_M_EXT    11

/* ========================================================================== */
/* 11. TIME DELAY                                                              */
/* ========================================================================== */

/* Platform timebase (Hz). Override with -DSTOR_TIMEBASE_HZ=... at build time. */
#ifndef STOR_TIMEBASE_HZ
#define STOR_TIMEBASE_HZ 1000000ULL
#endif

static __always_inline void stor_udelay(u32 usec)
{
    u64 ticks = ((u64)usec * STOR_TIMEBASE_HZ) / 1000000ULL;
    if (!ticks) ticks = 1;
    u64 start  = stor_read_time();
    u64 target = start + ticks;
    while (stor_read_time() < target) stor_nop();
}

static __always_inline void stor_mdelay(u32 msec)
{
    u64 ticks = ((u64)msec * STOR_TIMEBASE_HZ) / 1000ULL;
    u64 start  = stor_read_time();
    u64 target = start + ticks;
    while (stor_read_time() < target) stor_pause();
}

static __always_inline bool stor_poll_reg_bits_eq(vaddr_t base, u32 off,
                                                   u32 mask, u32 expected,
                                                   u32 timeout_us)
{
    u64 start = stor_read_time();
    u64 dt = ((u64)timeout_us * STOR_TIMEBASE_HZ) / 1000000ULL;
    for (;;) {
        if ((stor_reg_read(base, off) & mask) == expected) return true;
        if (stor_read_time() - start > dt) return false;
        stor_nop();
    }
}

static __always_inline bool stor_poll_reg_bits_ne(vaddr_t base, u32 off,
                                                   u32 mask, u32 expected,
                                                   u32 timeout_us)
{
    u64 start = stor_read_time();
    u64 dt = ((u64)timeout_us * STOR_TIMEBASE_HZ) / 1000000ULL;
    for (;;) {
        if ((stor_reg_read(base, off) & mask) != expected) return true;
        if (stor_read_time() - start > dt) return false;
        stor_nop();
    }
}

/* ========================================================================== */
/* 12. ATOMIC OPERATIONS (RISC-V A extension)                                  */
/* ========================================================================== */

static __always_inline u32 stor_atomic_read_32(volatile u32 *a)
{
    u32 v;
    __asm__ __volatile__ ("amoswap.w.aqrl %0, zero, (%1)"
        : "=r"(v) : "r"(a) : "memory");
    return v;
}

static __always_inline void stor_atomic_write_32(volatile u32 *a, u32 v)
{
    __asm__ __volatile__ ("amoswap.w.aqrl zero, %0, (%1)"
        : : "r"(v), "r"(a) : "memory");
}

static __always_inline u32 stor_atomic_fetch_add_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoadd.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static __always_inline u32 stor_atomic_fetch_sub_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoadd.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"((u32)(-(s32)v)) : "memory");
    return old;
}

static __always_inline u32 stor_atomic_fetch_and_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoand.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static __always_inline u32 stor_atomic_fetch_or_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoor.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static __always_inline u32 stor_atomic_fetch_xor_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoxor.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static __always_inline u32 stor_atomic_xchg_32(volatile u32 *a, u32 v)
{
    u32 old;
    __asm__ __volatile__ ("amoswap.w.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static __always_inline bool stor_atomic_cmpxchg_32(volatile u32 *a, u32 old_v, u32 new_v)
{
    u32 prev;
    __asm__ __volatile__ (
        "1: lr.w.aqrl %0, (%1)\n"
        "   bne     %0, %2, 2f\n"
        "   sc.w.aqrl %0, %3, (%1)\n"
        "   bnez    %0, 1b\n"
        "2:\n"
        : "=&r"(prev) : "r"(a), "r"(old_v), "r"(new_v) : "memory");
    return prev == old_v;
}

static __always_inline u32 stor_atomic_add_return_32(volatile u32 *a, u32 v)
{ return stor_atomic_fetch_add_32(a, v) + v; }

static __always_inline u32 stor_atomic_sub_return_32(volatile u32 *a, u32 v)
{ return stor_atomic_fetch_sub_32(a, v) - v; }

static __always_inline bool stor_atomic_test_and_set_32(volatile u32 *a, u32 bit)
{ return (stor_atomic_fetch_or_32(a, 1u << bit) & (1u << bit)) != 0; }

static __always_inline void stor_atomic_clear_bit_32(volatile u32 *a, u32 bit)
{ (void)stor_atomic_fetch_and_32(a, ~(1u << bit)); }

static __always_inline bool stor_atomic_test_bit_32(volatile u32 *a, u32 bit)
{ return (stor_atomic_read_32(a) & (1u << bit)) != 0; }

static __always_inline u64 stor_atomic_read_64(volatile u64 *a)
{
    u64 v;
    __asm__ __volatile__ ("amoswap.d.aqrl %0, zero, (%1)"
        : "=r"(v) : "r"(a) : "memory");
    return v;
}

static __always_inline void stor_atomic_write_64(volatile u64 *a, u64 v)
{
    __asm__ __volatile__ ("amoswap.d.aqrl zero, %0, (%1)"
        : : "r"(v), "r"(a) : "memory");
}

static __always_inline u64 stor_atomic_fetch_add_64(volatile u64 *a, u64 v)
{
    u64 old;
    __asm__ __volatile__ ("amoadd.d.aqrl %0, %2, (%1)"
        : "=r"(old) : "r"(a), "r"(v) : "memory");
    return old;
}

static __always_inline bool stor_atomic_cmpxchg_64(volatile u64 *a, u64 old_v, u64 new_v)
{
    u64 prev;
    __asm__ __volatile__ (
        "1: lr.d.aqrl %0, (%1)\n"
        "   bne     %0, %2, 2f\n"
        "   sc.d.aqrl %0, %3, (%1)\n"
        "   bnez    %0, 1b\n"
        "2:\n"
        : "=&r"(prev) : "r"(a), "r"(old_v), "r"(new_v) : "memory");
    return prev == old_v;
}

/* ========================================================================== */
/* 13. SPINLOCK                                                                */
/* ========================================================================== */

typedef struct { volatile u32 lock; } stor_spinlock_t;

#define STOR_SPINLOCK_INIT { 0 }

static __always_inline void stor_spinlock_init(stor_spinlock_t *l) { l->lock = 0; }

static __always_inline void stor_spin_lock(stor_spinlock_t *l)
{
    while (stor_atomic_test_and_set_32(&l->lock, 0) == 0)
        while (STOR_READ_ONCE(l->lock)) stor_pause();
    stor_mb();
}

static __always_inline bool stor_spin_trylock(stor_spinlock_t *l)
{
    if (stor_atomic_test_and_set_32(&l->lock, 0) == 0) { stor_mb(); return true; }
    return false;
}

static __always_inline void stor_spin_unlock(stor_spinlock_t *l)
{
    stor_mb();
    STOR_WRITE_ONCE(l->lock, 0);
    stor_mb();
}

/* ========================================================================== */
/* 14. MUTEX                                                                   */
/* ========================================================================== */

typedef struct { volatile u32 count; stor_spinlock_t lock; } stor_mutex_t;

#define STOR_MUTEX_INIT { 0, STOR_SPINLOCK_INIT }

static __always_inline void stor_mutex_init(stor_mutex_t *m)
{ m->count = 1; stor_spinlock_init(&m->lock); }

static __always_inline void stor_mutex_lock(stor_mutex_t *m)
{
    for (;;) {
        stor_spin_lock(&m->lock);
        if (m->count) { m->count = 0; stor_spin_unlock(&m->lock); break; }
        stor_spin_unlock(&m->lock);
        stor_pause();
    }
    stor_mb();
}

static __always_inline bool stor_mutex_trylock(stor_mutex_t *m)
{
    bool ok = false;
    stor_spin_lock(&m->lock);
    if (m->count) { m->count = 0; ok = true; }
    stor_spin_unlock(&m->lock);
    if (ok) stor_mb();
    return ok;
}

static __always_inline void stor_mutex_unlock(stor_mutex_t *m)
{
    stor_mb();
    stor_spin_lock(&m->lock);
    m->count = 1;
    stor_spin_unlock(&m->lock);
}

/* ========================================================================== */
/* 15. SEMAPHORE                                                               */
/* ========================================================================== */

typedef struct {
    volatile u32 count;
    volatile u32 waiters;
    stor_spinlock_t lock;
} stor_semaphore_t;

#define STOR_SEMAPHORE_INIT(n) { (n), 0, STOR_SPINLOCK_INIT }

static __always_inline void stor_sem_init(stor_semaphore_t *s, u32 n)
{ s->count = n; s->waiters = 0; stor_spinlock_init(&s->lock); }

static __always_inline void stor_sem_down(stor_semaphore_t *s)
{
    for (;;) {
        stor_spin_lock(&s->lock);
        if (s->count) { s->count--; stor_spin_unlock(&s->lock); break; }
        s->waiters++;
        stor_spin_unlock(&s->lock);
        stor_pause();
    }
    stor_mb();
}

static __always_inline bool stor_sem_trydown(stor_semaphore_t *s)
{
    bool ok = false;
    stor_spin_lock(&s->lock);
    if (s->count) { s->count--; ok = true; }
    stor_spin_unlock(&s->lock);
    if (ok) stor_mb();
    return ok;
}

static __always_inline void stor_sem_up(stor_semaphore_t *s)
{
    stor_mb();
    stor_spin_lock(&s->lock);
    s->count++;
    stor_spin_unlock(&s->lock);
}

/* ========================================================================== */
/* 16. CRC / CHECKSUM (used by NVMe/AHCI/AAC)                                  */
/* ========================================================================== */

static __always_inline u8 stor_crc8(const u8 *data, usize n, u8 poly, u8 init)
{
    u8 crc = init;
    for (usize i = 0; i < n; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++)
            crc = (u8)((crc & 0x80) ? ((crc << 1) ^ poly) : (crc << 1));
    }
    return crc;
}

static __always_inline u16 stor_crc16_ccitt(const u8 *data, usize n)
{
    u16 crc = 0xFFFF;
    for (usize i = 0; i < n; i++) {
        crc ^= (u16)data[i] << 8;
        for (int b = 0; b < 8; b++)
            crc = (u16)((crc & 0x8000) ? ((crc << 1) ^ 0x1021) : (crc << 1));
    }
    return crc;
}

static __always_inline u32 stor_crc32(const u8 *data, usize n)
{
    u32 crc = 0xFFFFFFFFu;
    for (usize i = 0; i < n; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++)
            crc = (crc >> 1) ^ (0xEDB88320u & (u32)(-(s32)(crc & 1u)));
    }
    return ~crc;
}

#endif /* STOR_MMIO_H */
