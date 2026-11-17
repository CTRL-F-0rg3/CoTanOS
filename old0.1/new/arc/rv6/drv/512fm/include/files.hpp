/*
 * ============================================================================
 *  FILES.HPP - Kernel-side file manager for 512fm
 *  CoTanOS · RISC-V · C++17 · freestanding
 *
 *  Wraps FileSystem with:
 *    - Extension registry (20 primary + runtime-registered custom)
 *    - SectorMap (ownership tracking for fsck + defrag)
 *    - TypeGuard (prevents corruption via bad extension decisions)
 *    - Higher-level file/dir operations
 * ============================================================================
 */

#ifndef FM512_FILES_HPP
#define FM512_FILES_HPP

#include "512fm_filesystem.hpp"
#include "512fm_node.hpp"

namespace fm512 {

/* ========================================================================== */
/* EXTENSIONS                                                                 */
/* ========================================================================== */

enum class FileCategory : std::uint8_t {
    Unknown    = 0,
    Text       = 1,
    Config     = 2,
    Log        = 3,
    Script     = 4,
    Binary     = 5,
    Executable = 6,
    Library    = 7,
    Data       = 8,
    Archive    = 9,
    Image      = 10,
    Source     = 11,
    Table      = 12,
    System     = 13,
    CoTanOS    = 14,
};

constexpr std::uint32_t EXT_FLAG_EXECUTABLE = 1u << 0;
constexpr std::uint32_t EXT_FLAG_READONLY   = 1u << 1;
constexpr std::uint32_t EXT_FLAG_SYSTEM     = 1u << 2;
constexpr std::uint32_t EXT_FLAG_BINARY     = 1u << 3;
constexpr std::uint32_t EXT_FLAG_TEXT       = 1u << 4;
constexpr std::uint32_t EXT_FLAG_NEEDS_MAGIC= 1u << 5;

struct ExtensionInfo {
    char          ext[16];       // ".txt"
    char          name[24];      // "Plain Text"
    char          mime[40];      // "text/plain"
    FileCategory  category;
    std::uint32_t flags;
    std::uint8_t  magic_len;
    std::uint8_t  magic[8];      // first bytes to match (if EXT_FLAG_NEEDS_MAGIC)
};

constexpr std::size_t MAX_EXTENSIONS = 128;

class ExtensionRegistry {
public:
    Result<void> init_defaults() noexcept;
    Result<void> register_extension(const ExtensionInfo &info) noexcept;

    Result<const ExtensionInfo *> lookup(const char *name) noexcept;
    Result<FileCategory> categorize(const char *name) noexcept;

    /* Detect extension by scanning magic bytes. Returns nullptr if no match. */
    const ExtensionInfo *detect_by_magic(const std::uint8_t *data,
                                          std::size_t len) noexcept;

    std::size_t count() const noexcept { return count_; }

private:
    ExtensionInfo exts_[MAX_EXTENSIONS]{};
    std::size_t   count_ = 0;
};

/* Extension helpers */
const char    *extension_of(const char *path) noexcept;    // ".txt" or ""
bool           has_extension(const char *path, const char *ext) noexcept;
bool           ext_equals(const char *a, const char *b) noexcept;
const char    *category_name(FileCategory c) noexcept;

/* ========================================================================== */
/* SECTOR MAP — ownership tracking                                           */
/* ========================================================================== */

struct SectorRange {
    std::uint64_t start;
    std::uint64_t length;
    std::uint64_t owner_ino;
};

constexpr std::size_t MAX_SECTOR_RANGES = 2048;

class SectorMap {
public:
    void reset() noexcept;

    Result<void> claim(std::uint64_t start, std::uint64_t length,
                        std::uint64_t owner_ino) noexcept;
    Result<void> release(std::uint64_t start, std::uint64_t length) noexcept;

    Result<std::uint64_t> owner_of(std::uint64_t sector) const noexcept;

    /* Find next extent owned by `owner_ino` starting at or after `after`. */
    Result<Extent> next_extent(std::uint64_t owner_ino,
                                std::uint64_t after_sector = 0) const noexcept;

    /* For defrag / fsck: iterate every range. */
    template <typename Fn>
    void foreach_range(Fn &&fn) const {
        for (std::size_t i = 0; i < count_; ++i) fn(ranges_[i]);
    }

    std::size_t count() const noexcept { return count_; }

private:
    SectorRange ranges_[MAX_SECTOR_RANGES]{};
    std::size_t count_ = 0;
};

/* ========================================================================== */
/* TYPE GUARD                                                                 */
/* ========================================================================== */

enum class GuardCheck : std::uint8_t {
    Ok = 0,
    EmptyFile,
    Mismatch,        // extension says X, magic says Y
    UnknownExt,      // extension not registered (write allowed, exec refused)
    Forbidden,       // read-only extension
    NotExecutable,
};

namespace TypeGuard {
    GuardCheck verify_open(const ExtensionRegistry &reg, const char *name,
                            const std::uint8_t *data, std::size_t len) noexcept;
    GuardCheck verify_exec(const ExtensionRegistry &reg, const char *name,
                            const std::uint8_t *data, std::size_t len) noexcept;
    GuardCheck verify_write(const ExtensionRegistry &reg, const char *name,
                             std::uint64_t new_size) noexcept;
    const char *check_name(GuardCheck c) noexcept;
}

/* ========================================================================== */
/* FILE MANAGER                                                               */
/* ========================================================================== */

class FileManager {
public:
    explicit FileManager(FileSystem &fs) noexcept;

    Result<void> init() noexcept;

    /* --- Directory operations --- */
    Result<void> mkdir(const char *path, std::uint16_t mode = 0755) noexcept;
    Result<void> mkdir_p(const char *path) noexcept;
    Result<void> rmdir(const char *path) noexcept;

    Result<DirEntry> readdir(std::uint64_t dir_sector,
                              std::uint32_t index) noexcept;
    Result<std::uint32_t> count_children(std::uint64_t dir_sector) noexcept;

    /* --- File operations --- */
    Result<void> create(const char *path, std::uint16_t mode = 0644) noexcept;
    Result<void> unlink(const char *path) noexcept;
    Result<void> rename(const char *old_path, const char *new_path) noexcept;
    Result<void> copy(const char *src, const char *dst) noexcept;
    Result<void> move(const char *src, const char *dst) noexcept;

    Result<std::size_t> read_file(const char *path, void *buf,
                                    std::size_t max) noexcept;
    Result<std::size_t> write_file(const char *path, const void *buf,
                                     std::size_t len) noexcept;
    Result<std::size_t> append_file(const char *path, const void *buf,
                                      std::size_t len) noexcept;

    /* --- Metadata --- */
    Result<Stat> stat(const char *path) noexcept;
    Result<void> chmod(const char *path, std::uint16_t mode) noexcept;
    Result<void> chown(const char *path, std::uint32_t uid,
                        std::uint32_t gid) noexcept;
    Result<void> truncate(const char *path, std::uint64_t size) noexcept;

    /* --- Path / sector helpers --- */
    Result<std::uint64_t> resolve(const char *path,
                                    std::uint64_t cwd = ROOT_SECTOR) noexcept;
    Result<void> path_of_sector(std::uint64_t sec, char *out,
                                 std::size_t max) noexcept;
    Result<bool> is_dir(const char *path) noexcept;
    Result<bool> is_file(const char *path) noexcept;

    /* --- Sector mapping / integrity --- */
    Result<Extent> map_logical(std::uint64_t ino,
                                std::uint64_t byte_offset) noexcept;
    Result<void> defragment(std::uint64_t ino) noexcept;
    Result<void> verify() noexcept;    // fsck
    Result<void> trim() noexcept;      // discard unused extents

    /* --- Extension handling --- */
    Result<const ExtensionInfo *> identify(const char *path) noexcept;
    const ExtensionInfo *detect_by_magic(const std::uint8_t *data,
                                          std::size_t len) noexcept;
    Result<void> register_extension(const ExtensionInfo &info) noexcept;

    /* --- Sync --- */
    Result<void> sync() noexcept;

    /* --- Accessors --- */
    FileSystem          &fs()       noexcept { return fs_; }
    ExtensionRegistry   &registry() noexcept { return registry_; }
    SectorMap           &map()      noexcept { return map_; }

private:
    Result<void> rebuild_sector_map() noexcept;
    Result<void> walk_node(std::uint64_t sec) noexcept;
    Result<std::size_t> read_all_extents(
        const Node &n,
        std::uint64_t offset,
        void *buf,
        std::size_t count) noexcept;
    Result<std::size_t> write_all_extents(
        Node &n,
        std::uint64_t offset,
        const void *buf,
        std::size_t count) noexcept;

    FileSystem         &fs_;
    ExtensionRegistry   registry_;
    SectorMap           map_;
    bool                initialized_ = false;
};

} // namespace fm512

#endif // FM512_FILES_HPP
