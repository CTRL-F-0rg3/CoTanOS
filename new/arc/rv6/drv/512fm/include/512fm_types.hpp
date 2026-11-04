/*
 * ============================================================================
 *  512FM_TYPES.HPP - Core types, error codes, Result<T>
 *  CoTanOS · RISC-V · C++17 · freestanding
 * ============================================================================
 */

#ifndef FM512_TYPES_HPP
#define FM512_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>

namespace fm512 {

/* ========================================================================== */
/* CONSTANTS                                                                  */
/* ========================================================================== */

constexpr std::uint32_t SECTOR_SIZE   = 512;
constexpr std::uint32_t SECTORS_PER_BLOCK = 1;      // 1 sector = 1 block
constexpr std::uint32_t BLOCK_SIZE    = 512;

constexpr std::uint64_t SIGNATURE     = 0x0030363131; // "512\0"
constexpr std::uint32_t VERSION       = 1;

constexpr std::uint32_t MAX_NAME_LEN  = 240;
constexpr std::uint32_t INLINE_EXTENTS = 6;
constexpr std::uint32_t EXTENTS_PER_BLOCK =
    (SECTOR_SIZE / 16);   // 32 extents per chain block

constexpr std::uint64_t ROOT_SECTOR   = 1;   // sector 0 = Header
constexpr std::uint64_t HEADER_SECTOR = 0;

constexpr std::uint64_t BLOCK_LEVELS  = 8;   // allocator hierarchy depth

/* File mode (Unix-style) */
constexpr std::uint16_t MODE_TYPE     = 0xF000;
constexpr std::uint16_t MODE_FILE     = 0x8000;
constexpr std::uint16_t MODE_DIR      = 0x4000;
constexpr std::uint16_t MODE_SYMLINK  = 0xA000;
constexpr std::uint16_t MODE_PERM     = 0x0FFF;
constexpr std::uint16_t PERM_READ     = 0x4;
constexpr std::uint16_t PERM_WRITE    = 0x2;
constexpr std::uint16_t PERM_EXEC     = 0x1;

/* Open flags */
constexpr std::uint32_t O_RDONLY      = 0x0000;
constexpr std::uint32_t O_WRONLY      = 0x0001;
constexpr std::uint32_t O_RDWR        = 0x0002;
constexpr std::uint32_t O_CREAT       = 0x0040;
constexpr std::uint32_t O_EXCL        = 0x0080;
constexpr std::uint32_t O_TRUNC       = 0x0200;
constexpr std::uint32_t O_APPEND      = 0x0400;
constexpr std::uint32_t O_DIRECTORY   = 0x0800;

/* Seek whence */
constexpr int SEEK_SET = 0;
constexpr int SEEK_CUR = 1;
constexpr int SEEK_END = 2;

/* ========================================================================== */
/* ERROR CODES                                                                */
/* ========================================================================== */

enum class Error : int {
    Ok             = 0,
    Generic        = -1,
    Timeout        = -2,
    Invalid        = -3,
    NoMem          = -4,
    Busy           = -5,
    NotFound       = -6,
    Exists         = -7,
    NotEmpty       = -8,
    IsDirectory    = -9,
    NotDirectory   = -10,
    NoSpace        = -11,
    Corrupt        = -12,
    NotSupported   = -13,
    Io             = -14,
    ReadOnly       = -15,
    TooManyFiles   = -16,
    NameTooLong    = -17,
    CrossDevice    = -18,
    Permission     = -19,
};

/* ========================================================================== */
/* Result<T> (same pattern as storage driver)                                 */
/* ========================================================================== */

template <typename T>
class [[nodiscard]] Result {
    union {
        T     value_;
        Error error_;
    };
    bool ok_;

public:
    Result(T v) : value_(std::move(v)), ok_(true) {}
    Result(Error e) : error_(e), ok_(false) {}

    Result(const Result& o) : ok_(o.ok_) {
        if (ok_) new (&value_) T(o.value_);
        else     error_ = o.error_;
    }
    Result(Result&& o) noexcept : ok_(o.ok_) {
        if (ok_) new (&value_) T(std::move(o.value_));
        else     error_ = o.error_;
    }
    Result& operator=(const Result& o) {
        if (this != &o) { this->~Result(); ok_ = o.ok_;
            if (ok_) new (&value_) T(o.value_); else error_ = o.error_; }
        return *this;
    }
    Result& operator=(Result&& o) noexcept {
        if (this != &o) { this->~Result(); ok_ = o.ok_;
            if (ok_) new (&value_) T(std::move(o.value_)); else error_ = o.error_; }
        return *this;
    }
    ~Result() { if (ok_) value_.~T(); }

    bool is_ok()  const noexcept { return ok_; }
    bool is_err() const noexcept { return !ok_; }
    explicit operator bool() const noexcept { return ok_; }

    T&       value()       &  noexcept { return value_; }
    const T& value() const &  noexcept { return value_; }
    T&&      value()       && noexcept { return std::move(value_); }
    Error    error() const noexcept { return ok_ ? Error::Ok : error_; }
};

template <>
class [[nodiscard]] Result<void> {
    Error error_;
public:
    Result()        noexcept : error_(Error::Ok) {}
    Result(Error e) noexcept : error_(e) {}
    bool is_ok()  const noexcept { return error_ == Error::Ok; }
    bool is_err() const noexcept { return error_ != Error::Ok; }
    explicit operator bool() const noexcept { return is_ok(); }
    Error error() const noexcept { return error_; }
};

/* ========================================================================== */
/* EXTENT — contiguous sector range                                           */
/* ========================================================================== */

struct __attribute__((packed)) Extent {
    std::uint64_t block;    // starting sector
    std::uint64_t length;   // number of sectors

    constexpr bool is_empty() const noexcept { return length == 0; }
    constexpr std::uint64_t end() const noexcept { return block + length; }
};

/* ========================================================================== */
/* ON-DISK STRUCTURES                                                         */
/* ========================================================================== */

/*
 * Header — sector 0
 * Scanned for SIGNATURE within first 1 MiB (sector-aligned).
 */
struct __attribute__((packed)) Header {
    std::uint64_t signature;    // 0x0030363131 ("512\0")
    std::uint32_t version;
    std::uint32_t block_size;
    std::uint8_t  uuid[16];
    std::uint64_t total_sectors;
    std::uint64_t root_sector;
    std::uint64_t free_head;    // first free-list block
    std::uint64_t next_free;    // next header (COW ring)
    std::uint64_t generation;   // monotonically increasing
    std::uint32_t checksum;     // CRC32 of header (zeroed field)
    std::uint32_t reserved0;
    std::uint8_t  padding[440];

    bool valid() const noexcept { return signature == SIGNATURE && version == VERSION; }
};
static_assert(sizeof(Header) == SECTOR_SIZE, "Header must be exactly 512 bytes");

/*
 * Node — one per file/directory, stored in its own sector(s).
 * Inline extents cover the first INLINE_EXTENTS ranges; overflow via
 * `extents_next` chain block (32 Extents per sector).
 */
struct __attribute__((packed)) Node {
    std::uint16_t mode;
    std::uint16_t links;
    std::uint32_t uid;
    std::uint32_t gid;
    std::uint64_t size;         // bytes
    std::uint64_t parent;       // parent node sector
    std::uint64_t next;         // sibling in same directory
    std::uint64_t extents_next; // overflow extent chain (0 = none)
    Extent        extents[INLINE_EXTENTS];   // 96 bytes
    std::uint8_t  name[MAX_NAME_LEN];
    std::uint32_t checksum;
    std::uint32_t flags;
    std::uint8_t  padding[116];

    bool is_dir()   const noexcept { return (mode & MODE_TYPE) == MODE_DIR; }
    bool is_file()  const noexcept { return (mode & MODE_TYPE) == MODE_FILE; }
    bool is_symlink() const noexcept { return (mode & MODE_TYPE) == MODE_SYMLINK; }
};
static_assert(sizeof(Node) <= SECTOR_SIZE, "Node must fit in one sector");

/*
 * TreePtr — 32-bit reference used by the directory tree.
 * Encodes level (upper 8 bits) + sector index (lower 24 bits).
 * Level 0 = direct node sector; higher levels = tree block index.
 */
struct __attribute__((packed)) TreePtr {
    std::uint32_t raw;

    constexpr std::uint32_t id()    const noexcept { return raw & 0x00FFFFFF; }
    constexpr std::uint8_t  level() const noexcept { return (raw >> 24) & 0xFF; }
    constexpr bool is_null()        const noexcept { return raw == 0; }

    static constexpr TreePtr make(std::uint8_t lvl, std::uint32_t id) noexcept {
        return TreePtr{ (static_cast<std::uint32_t>(lvl) << 24) | (id & 0x00FFFFFF) };
    }
};

/* ========================================================================== */
/* STAT / DIRECTORY ENTRY (POSIX-like)                                        */
/* ========================================================================== */

struct Stat {
    std::uint16_t mode;
    std::uint16_t links;
    std::uint32_t uid;
    std::uint32_t gid;
    std::uint64_t size;
    std::uint64_t ino;       // sector number
    std::uint64_t blocks;    // sectors allocated
};

struct DirEntry {
    std::uint64_t ino;       // node sector
    std::uint16_t mode;
    std::uint8_t  name_len;
    char          name[MAX_NAME_LEN + 1];
};

} // namespace fm512

#endif // FM512_TYPES_HPP
