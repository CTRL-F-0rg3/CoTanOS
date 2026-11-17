/*
 * ============================================================================
 *  512FM_DISK.CPP - Disk abstraction implementation
 * ============================================================================
 */

#include "512fm_disk.hpp"

/* Bring in the CoTanOS storage driver. */
extern "C" {
#include "storage_core.h"
}

namespace fm512 {

/* ========================================================================== */
/* BlockDeviceDisk                                                            */
/* ========================================================================== */

BlockDeviceDisk::BlockDeviceDisk(stor_blk_dev *dev) noexcept
    : dev_(dev)
    , sector_size_(dev ? dev->sector_size : SECTOR_SIZE) {}

Result<std::size_t> BlockDeviceDisk::read_at(std::uint64_t block,
                                              void *buffer,
                                              std::size_t bytes) noexcept
{
    if (!dev_ || !buffer) return Error::Invalid;
    if (bytes % SECTOR_SIZE != 0) return Error::Invalid;

    std::uint64_t lba = block * (SECTOR_SIZE / sector_size_);
    std::uint32_t nsect = static_cast<std::uint32_t>(
        bytes / sector_size_);

    int rc = stor_blk_read_sync(dev_, lba, nsect, buffer);
    if (rc != STOR_OK) return Error::Io;
    return bytes;
}

Result<std::size_t> BlockDeviceDisk::write_at(std::uint64_t block,
                                               const void *buffer,
                                               std::size_t bytes) noexcept
{
    if (!dev_ || !buffer) return Error::Invalid;
    if (bytes % SECTOR_SIZE != 0) return Error::Invalid;

    std::uint64_t lba = block * (SECTOR_SIZE / sector_size_);
    std::uint32_t nsect = static_cast<std::uint32_t>(
        bytes / sector_size_);

    int rc = stor_blk_write_sync(dev_, lba, nsect, buffer);
    if (rc != STOR_OK) return Error::Io;
    return bytes;
}

Result<std::uint64_t> BlockDeviceDisk::size() noexcept
{
    if (!dev_) return Error::Invalid;
    std::uint64_t bytes = dev_->capacity_bytes;
    return bytes / SECTOR_SIZE;
}

} // namespace fm512
