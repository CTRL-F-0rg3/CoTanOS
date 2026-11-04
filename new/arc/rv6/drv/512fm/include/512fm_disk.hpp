/*
 * ============================================================================
 *  512FM_DISK.HPP - Sector-level disk abstraction
 *
 *  Mirrors Redox's `Disk` trait: read_at(block, buffer), write_at(block, buffer),
 *  size().  The adapter below bridges CoTanOS's stor_blk_dev_t into this
 *  interface, translating sector numbers to LBA.
 * ============================================================================
 */

#ifndef FM512_DISK_HPP
#define FM512_DISK_HPP

#include "512fm_types.hpp"

namespace fm512 {

class Disk {
public:
    virtual ~Disk() noexcept = default;

    /* block = sector number; buffer must be `bytes` long (multiple of 512). */
    virtual Result<std::size_t> read_at(std::uint64_t block,
                                         void *buffer,
                                         std::size_t bytes) noexcept = 0;
    virtual Result<std::size_t> write_at(std::uint64_t block,
                                          const void *buffer,
                                          std::size_t bytes) noexcept = 0;
    virtual Result<std::uint64_t> size() noexcept = 0;   // total sectors
};

/* --------------------------------------------------------------------------
 * BlockDeviceDisk — adapter over stor_blk_dev_t
 * ------------------------------------------------------------------------ */

struct stor_blk_dev;   // forward-declared, from storage_core.h

class BlockDeviceDisk final : public Disk {
    stor_blk_dev *dev_;
    std::uint32_t sector_size_;

public:
    explicit BlockDeviceDisk(stor_blk_dev *dev) noexcept;

    Result<std::size_t> read_at(std::uint64_t block,
                                 void *buffer,
                                 std::size_t bytes) noexcept override;
    Result<std::size_t> write_at(std::uint64_t block,
                                  const void *buffer,
                                  std::size_t bytes) noexcept override;
    Result<std::uint64_t> size() noexcept override;
};

} // namespace fm512

#endif // FM512_DISK_HPP
