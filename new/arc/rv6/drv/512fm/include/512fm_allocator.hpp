/*
 * ============================================================================
 *  512FM_ALLOCATOR.HPP - Hierarchical free-space allocator
 *
 *  Inspired by Redox's `Allocator`: maintains levels[0..7] of free extents.
 *  Level N tracks runs of 2^N consecutive sectors. Allocate splits a
 *  higher-level run when a lower level is exhausted.
 *
 *  Each level's free list is stored on-disk in 512-byte ExtentBlocks
 *  (32 extents per sector). The in-memory state is a mirror loaded on
 *  mount and written back on unmount / commit.
 * ============================================================================
 */

#ifndef FM512_ALLOCATOR_HPP
#define FM512_ALLOCATOR_HPP

#include "512fm_types.hpp"
#include "512fm_disk.hpp"

namespace fm512 {

struct FreeListLevel {
    Extent extents[32];   // 32 free runs at this level
    std::uint32_t count;  // number of valid entries
};

struct Allocator {
    FreeListLevel levels[BLOCK_LEVELS];

    /* Initialise empty. */
    void reset() noexcept;

    /* Add a free extent [start, start+len) to the pool, splitting into
     * power-of-two runs. */
    void add_free_extent(std::uint64_t start, std::uint64_t len) noexcept;

    /* Allocate `count` contiguous sectors. Returns the starting sector
     * or Error::NoSpace. */
    Result<std::uint64_t> allocate(std::uint64_t count) noexcept;

    /* Free `count` sectors starting at `start`. */
    void free_sectors(std::uint64_t start, std::uint64_t count) noexcept;

    /* Total free sectors across all levels. */
    std::uint64_t total_free() const noexcept;

    /* --- Persistence --- */
    /* Write the allocator state to disk starting at `head_sector`.
     * Returns the new head sector (COW: a new copy is written). */
    Result<std::uint64_t> persist(Disk &disk, std::uint64_t head_sector) noexcept;

    /* Load allocator state from disk. */
    Result<void> load(Disk &disk, std::uint64_t head_sector) noexcept;
};

} // namespace fm512

#endif // FM512_ALLOCATOR_HPP
