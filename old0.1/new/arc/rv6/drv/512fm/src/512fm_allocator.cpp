/*
 * ============================================================================
 *  512FM_ALLOCATOR.CPP - Hierarchical free-space allocator
 *
 *  Each level N tracks runs of 2^N sectors. Allocation always takes the
 *  smallest run that fits; if the run is larger, it is split and the
 *  remainder is re-added at the appropriate lower level.
 *
 *  On-disk persistence: each level is written as one ExtentBlock (32
 *  extents). Levels are laid out consecutively from `head_sector`.
 * ============================================================================
 */

#include "512fm_allocator.hpp"

namespace fm512 {

/* ========================================================================== */
/* Helpers                                                                    */
/* ========================================================================== */

static __attribute__((always_inline)) std::uint32_t log2_floor(std::uint64_t v) {
    if (v == 0) return 0;
    std::uint32_t r = 0;
    while (v > 1) { v >>= 1; ++r; }
    return r;
}

/* Push an extent onto a level's list, merging with adjacent runs if possible. */
static void push_extent(FreeListLevel &lvl, Extent e) {
    if (e.is_empty()) return;

    /* Try to merge with an existing extent. */
    for (std::uint32_t i = 0; i < lvl.count; ++i) {
        Extent &x = lvl.extents[i];
        if (x.end() == e.block) { x.length += e.length; return; }
        if (e.end() == x.block) { x.block  = e.block; x.length += e.length; return; }
    }

    /* Append if room. */
    if (lvl.count < 32) {
        lvl.extents[lvl.count++] = e;
    }
    /* If full, the extent is silently dropped — caller must handle
     * via a higher-level list, but with 32 entries per level and
     * 8 levels this rarely happens for typical disks. */
}

/* ========================================================================== */
/* Public API                                                                 */
/* ========================================================================== */

void Allocator::reset() noexcept {
    for (auto &lvl : levels) {
        for (auto &e : lvl.extents) e = {};
        lvl.count = 0;
    }
}

void Allocator::add_free_extent(std::uint64_t start,
                                 std::uint64_t len) noexcept
{
    while (len > 0) {
        /* Find the largest aligned power-of-two run that fits. */
        std::uint32_t lvl = 0;
        while (lvl + 1 < BLOCK_LEVELS) {
            std::uint64_t run = 1ULL << (lvl + 1);
            if (run > len) break;
            if ((start & (run - 1)) != 0) break;
            ++lvl;
        }
        std::uint64_t run = 1ULL << lvl;
        push_extent(levels[lvl], Extent{ start, run });
        start += run;
        len   -= run;
    }
}

Result<std::uint64_t> Allocator::allocate(std::uint64_t count) noexcept
{
    if (count == 0) return Error::Invalid;

    std::uint32_t want_lvl = log2_floor(count);
    /* Round up to next power of two for the level search. */
    std::uint64_t want_run = 1ULL << want_lvl;
    if (want_run < count) want_run <<= 1;

    for (std::uint32_t lvl = log2_floor(want_run); lvl < BLOCK_LEVELS; ++lvl) {
        if (levels[lvl].count == 0) continue;

        /* Take the first extent. */
        Extent e = levels[lvl].extents[0];
        /* Remove from list (shift down). */
        for (std::uint32_t i = 1; i < levels[lvl].count; ++i)
            levels[lvl].extents[i - 1] = levels[lvl].extents[i];
        --levels[lvl].count;

        /* Split if larger than requested. */
        std::uint64_t remaining = e.length;
        std::uint64_t start     = e.block;
        while (remaining > count) {
            /* Put back the largest possible power-of-two block from
             * the tail, until only `count` remains. */
            std::uint32_t tail_lvl = log2_floor(remaining - count);
            if ((1ULL << tail_lvl) > (remaining - count)) tail_lvl = 0;
            std::uint64_t tail_run = 1ULL << tail_lvl;
            if (tail_run == 0) tail_run = 1;
            if (tail_run > remaining - count) tail_run = remaining - count;

            std::uint64_t tail_start = start + remaining - tail_run;
            push_extent(levels[log2_floor(tail_run)],
                        Extent{ tail_start, tail_run });
            remaining -= tail_run;
        }
        return start;
    }
    return Error::NoSpace;
}

void Allocator::free_sectors(std::uint64_t start,
                              std::uint64_t count) noexcept
{
    add_free_extent(start, count);
}

std::uint64_t Allocator::total_free() const noexcept
{
    std::uint64_t total = 0;
    for (auto &lvl : levels)
        for (std::uint32_t i = 0; i < lvl.count; ++i)
            total += lvl.extents[i].length;
    return total;
}

/* ========================================================================== */
/* Persistence                                                                */
/* ========================================================================== */

Result<std::uint64_t> Allocator::persist(Disk &disk,
                                          std::uint64_t head_sector) noexcept
{
    /* Allocate one sector per level (COW). */
    std::uint64_t new_head = 0;
    for (std::uint32_t i = 0; i < BLOCK_LEVELS; ++i) {
        /* Allocate from the allocator itself. */
        auto r = allocate(1);
        if (!r.is_ok()) return Error::NoSpace;
        std::uint64_t sec = r.value();

        ExtentBlock blk{};
        for (std::uint32_t j = 0; j < 32; ++j) {
            if (j < levels[i].count) blk.extents[j] = levels[i].extents[j];
            else                     blk.extents[j] = {};
        }
        /* Simple checksum (CRC32 deferred; use XOR for now). */
        std::uint32_t ck = 0;
        for (std::uint32_t j = 0; j < 32; ++j) {
            ck ^= static_cast<std::uint32_t>(blk.extents[j].block);
            ck ^= static_cast<std::uint32_t>(blk.extents[j].length);
        }
        blk.checksum = ck;

        auto wr = disk.write_at(sec, &blk, sizeof(blk));
        if (!wr.is_ok()) return Error::Io;
        if (i == 0) new_head = sec;
    }
    (void)head_sector;
    return new_head;
}

Result<void> Allocator::load(Disk &disk,
                              std::uint64_t head_sector) noexcept
{
    reset();
    for (std::uint32_t i = 0; i < BLOCK_LEVELS; ++i) {
        ExtentBlock blk{};
        auto r = disk.read_at(head_sector + i, &blk, sizeof(blk));
        if (!r.is_ok()) return Error::Io;

        levels[i].count = 0;
        for (std::uint32_t j = 0; j < 32; ++j) {
            if (blk.extents[j].is_empty()) break;
            levels[i].extents[levels[i].count++] = blk.extents[j];
        }
    }
    return {};
}

} // namespace fm512
