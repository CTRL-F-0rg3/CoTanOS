/*
 * ============================================================================
 *  512FM_NODE.HPP - Node / Extent serialisation + inline helpers
 * ============================================================================
 */

#ifndef FM512_NODE_HPP
#define FM512_NODE_HPP

#include "512fm_types.hpp"
#include "512fm_disk.hpp"

namespace fm512 {

/* Compute node byte size / sectors */
constexpr std::uint32_t node_sectors() noexcept {
    return (sizeof(Node) + SECTOR_SIZE - 1) / SECTOR_SIZE;
}

/* Extent chain block: 32 extents per sector */
struct __attribute__((packed)) ExtentBlock {
    Extent       extents[32];
    std::uint32_t checksum;
    std::uint32_t reserved;
};
static_assert(sizeof(ExtentBlock) == SECTOR_SIZE, "ExtentBlock must be 512 B");

/* Sum of all extent lengths in a Node (including chained overflow). */
std::uint64_t node_allocated_bytes(const Node &n) noexcept;

/* Look up the Nth extent (0-based), following the chain. Returns
 * `true` if found (filling `out`), `false` if beyond the last extent. */
bool node_get_extent(const Node &n,
                     std::uint32_t index,
                     Disk &disk,
                     Extent &out,
                     std::uint64_t *chain_sector_out = nullptr);

/* Iterate every extent in a node, calling `fn(Extent)`. Stops when
 * `fn` returns false or extents are exhausted. */
template <typename Fn>
void node_foreach_extent(const Node &n, Disk &disk, Fn &&fn) {
    for (std::uint32_t i = 0; i < INLINE_EXTENTS; ++i) {
        if (n.extents[i].is_empty()) return;
        if (!fn(n.extents[i])) return;
    }
    if (n.extents_next == 0) return;

    ExtentBlock blk{};
    auto r = disk.read_at(n.extents_next, &blk, sizeof(blk));
    if (!r.is_ok()) return;
    for (std::uint32_t i = 0; i < 32; ++i) {
        if (blk.extents[i].is_empty()) return;
        if (!fn(blk.extents[i])) return;
    }
}

/* Read a Node from disk; validates checksum. */
Result<void> node_read(Disk &disk, std::uint64_t sector, Node &out);

/* Write a Node (computes checksum). */
Result<void> node_write(Disk &disk, std::uint64_t sector, Node &inout);

/* Zero-initialise a new Node. */
void node_init(Node &n,
               std::uint16_t mode,
               std::uint32_t uid,
               std::uint32_t gid,
               const char *name,
               std::uint64_t parent);

/* Append a filename to the node's name field (truncates to MAX_NAME_LEN). */
void node_set_name(Node &n, const char *name);

} // namespace fm512

#endif // FM512_NODE_HPP
