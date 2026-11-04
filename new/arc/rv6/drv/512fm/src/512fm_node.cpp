/*
 * ============================================================================
 *  512FM_NODE.CPP - Node serialisation helpers
 * ============================================================================
 */

#include "512fm_node.hpp"

namespace fm512 {

/* ========================================================================== */
/* Checksum                                                                   */
/* ========================================================================== */

static std::uint32_t node_checksum(const Node &n) noexcept {
    /* CRC32-lite over the first 384 bytes (excluding checksum + padding). */
    const std::uint8_t *p = reinterpret_cast<const std::uint8_t *>(&n);
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < offsetof(Node, checksum); ++i) {
        crc ^= p[i];
        for (int b = 0; b < 8; ++b)
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}

/* ========================================================================== */
/* Public API                                                                 */
/* ========================================================================== */

void node_init(Node &n, std::uint16_t mode, std::uint32_t uid,
                std::uint32_t gid, const char *name, std::uint64_t parent) {
    std::memset(&n, 0, sizeof(n));
    n.mode   = mode;
    n.links  = (mode & MODE_TYPE) == MODE_DIR ? 2 : 1;
    n.uid    = uid;
    n.gid    = gid;
    n.parent = parent;
    node_set_name(n, name);
}

void node_set_name(Node &n, const char *name) {
    std::memset(n.name, 0, sizeof(n.name));
    if (!name) return;
    std::size_t len = 0;
    while (name[len] && len < MAX_NAME_LEN) ++len;
    std::memcpy(n.name, name, len);
}

Result<void> node_read(Disk &disk, std::uint64_t sector, Node &out) {
    auto r = disk.read_at(sector, &out, sizeof(out));
    if (!r.is_ok()) return Error::Io;
    if (node_checksum(out) != out.checksum) return Error::Corrupt;
    return {};
}

Result<void> node_write(Disk &disk, std::uint64_t sector, Node &inout) {
    inout.checksum = node_checksum(inout);
    auto r = disk.write_at(sector, &inout, sizeof(inout));
    if (!r.is_ok()) return Error::Io;
    return {};
}

std::uint64_t node_allocated_bytes(const Node &n) noexcept {
    /* For direct extents only (chain not walked here). */
    std::uint64_t total = 0;
    for (auto &e : n.extents) total += e.length;
    return total * SECTOR_SIZE;
}

bool node_get_extent(const Node &n, std::uint32_t index,
                      Disk &disk, Extent &out,
                      std::uint64_t *chain_sector_out) {
    if (index < INLINE_EXTENTS) {
        out = n.extents[index];
        return !out.is_empty();
    }

    if (n.extents_next == 0) return false;
    ExtentBlock blk{};
    auto r = disk.read_at(n.extents_next, &blk, sizeof(blk));
    if (!r.is_ok()) return false;
    if (chain_sector_out) *chain_sector_out = n.extents_next;

    std::uint32_t idx = index - INLINE_EXTENTS;
    if (idx >= 32) return false;
    out = blk.extents[idx];
    return !out.is_empty();
}

} // namespace fm512
