/*
 * ============================================================================
 *  512FM_FILESYSTEM.HPP - Main filesystem + Scheme interface
 *
 *  FileSystem: mount / format / transaction / file ops.
 *  Scheme:     VFS-facing open / read / write / seek / stat / readdir.
 *
 *  COW model:
 *    - Every mutating operation writes new blocks and defers pointer updates.
 *    - The header (sector 0) is only rewritten on commit, atomically
 *      switching root + free_head to the new generation.
 *    - If the header write fails, the old state is still valid.
 * ============================================================================
 */

#ifndef FM512_FILESYSTEM_HPP
#define FM512_FILESYSTEM_HPP

#include "512fm_types.hpp"
#include "512fm_disk.hpp"
#include "512fm_node.hpp"
#include "512fm_allocator.hpp"

namespace fm512 {

/* Open file descriptor table entry */
struct FileHandle {
    std::uint64_t node_sector;
    std::uint64_t offset;
    std::uint32_t flags;
    bool          used;
};

constexpr std::uint32_t MAX_OPEN_FILES = 64;

class FileSystem {
public:
    /* --- Lifecycle --- */
    explicit FileSystem(Disk &disk) noexcept;
    ~FileSystem() noexcept;

    /* Format a fresh filesystem (destroys all data). */
    Result<void> format(const std::uint8_t uuid[16]) noexcept;

    /* Mount an existing filesystem (scans for Header). */
    Result<void> mount() noexcept;

    /* Unmount, persisting allocator + header. */
    Result<void> unmount() noexcept;

    bool mounted() const noexcept { return mounted_; }
    std::uint64_t total_sectors() const noexcept { return header_.total_sectors; }
    std::uint64_t free_sectors() const noexcept { return alloc_.total_free(); }

    /* --- Scheme interface (path-based) --- */
    Result<std::uint64_t> lookup(const char *path,
                                 std::uint64_t cwd_sector = ROOT_SECTOR) noexcept;

    Result<void> create(const char *path,
                        std::uint16_t mode,
                        std::uint32_t uid = 0,
                        std::uint32_t gid = 0) noexcept;
    Result<void> mkdir(const char *path,
                       std::uint16_t mode = MODE_DIR | 0755,
                       std::uint32_t uid = 0,
                       std::uint32_t gid = 0) noexcept;
    Result<void> unlink(const char *path) noexcept;
    Result<void> rmdir(const char *path) noexcept;
    Result<void> rename(const char *old_path, const char *new_path) noexcept;

    /* --- File I/O --- */
    Result<int>  open(const char *path, std::uint32_t flags) noexcept;
    Result<std::size_t> read(int fd, void *buf, std::size_t count) noexcept;
    Result<std::size_t> write(int fd, const void *buf, std::size_t count) noexcept;
    Result<std::uint64_t> seek(int fd, std::int64_t offset, int whence) noexcept;
    Result<void> close(int fd) noexcept;

    /* --- Metadata --- */
    Result<Stat> stat(const char *path) noexcept;
    Result<DirEntry> readdir(std::uint64_t dir_sector,
                              std::uint32_t index) noexcept;
    Result<void> truncate(const char *path, std::uint64_t new_size) noexcept;

    /* --- Diagnostics --- */
    const Header &header() const noexcept { return header_; }
    const Allocator &allocator() const noexcept { return alloc_; }

private:
    /* --- Helpers --- */
    Result<void> load_header() noexcept;
    Result<void> write_header() noexcept;
    Result<std::uint64_t> alloc_sector() noexcept;
    Result<std::uint64_t> alloc_extent(std::uint64_t count) noexcept;
    void free_sector(std::uint64_t sector) noexcept;
    void free_node_data(Node &n) noexcept;

    Result<std::uint64_t> dir_lookup(std::uint64_t dir_sector,
                                     const char *name,
                                     std::uint32_t name_len) noexcept;
    Result<void> dir_link(std::uint64_t dir_sector,
                           std::uint64_t child_sector) noexcept;
    Result<void> dir_unlink(std::uint64_t dir_sector,
                             std::uint64_t child_sector) noexcept;

    Result<std::size_t> node_read_data(const Node &n,
                                        std::uint64_t offset,
                                        void *buf,
                                        std::size_t count) noexcept;
    Result<std::size_t> node_write_data(Node &n,
                                         std::uint64_t offset,
                                         const void *buf,
                                         std::size_t count) noexcept;

    FileHandle *find_fd(int fd) noexcept;

    /* --- State --- */
    Disk        &disk_;
    Header       header_{};
    Allocator    alloc_{};
    FileHandle   fds_[MAX_OPEN_FILES]{};
    bool         mounted_ = false;
};

/* --------------------------------------------------------------------------
 * Convenience: format a Disk in one call
 * ------------------------------------------------------------------------ */
Result<void> format(Disk &disk, const std::uint8_t uuid[16]) noexcept;

} // namespace fm512

#endif // FM512_FILESYSTEM_HPP
