/*
 * ============================================================================
 *  FILES.CPP - Kernel-side file manager implementation
 * ============================================================================
 */

#include "files.hpp"

namespace fm512 {

/* ========================================================================== */
/* HELPERS                                                                    */
/* ========================================================================== */

static inline char lower(char c) noexcept {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c;
}

const char *extension_of(const char *path) noexcept {
    if (!path) return "";
    const char *dot = nullptr;
    const char *slash = nullptr;
    for (const char *p = path; *p; ++p) {
        if (*p == '/')  slash = p;
        if (*p == '.')  dot   = p;
    }
    if (!dot) return "";
    if (slash && dot < slash) return "";
    if (dot[1] == 0) return "";
    return dot;
}

bool ext_equals(const char *a, const char *b) noexcept {
    if (!a || !b) return false;
    while (*a && *b) {
        if (lower(*a) != lower(*b)) return false;
        ++a; ++b;
    }
    return *a == 0 && *b == 0;
}

bool has_extension(const char *path, const char *ext) noexcept {
    return ext_equals(extension_of(path), ext);
}

const char *category_name(FileCategory c) noexcept {
    switch (c) {
    case FileCategory::Text:       return "text";
    case FileCategory::Config:     return "config";
    case FileCategory::Log:        return "log";
    case FileCategory::Script:     return "script";
    case FileCategory::Binary:     return "binary";
    case FileCategory::Executable: return "executable";
    case FileCategory::Library:    return "library";
    case FileCategory::Data:       return "data";
    case FileCategory::Archive:    return "archive";
    case FileCategory::Image:      return "image";
    case FileCategory::Source:     return "source";
    case FileCategory::Table:      return "table";
    case FileCategory::System:     return "system";
    case FileCategory::CoTanOS:    return "cotanos";
    default:                        return "unknown";
    }
}

/* ========================================================================== */
/* EXTENSION REGISTRY                                                         */
/* ========================================================================== */

static ExtensionInfo make_ext(const char *ext,
                                const char *name,
                                const char *mime,
                                FileCategory cat,
                                std::uint32_t flags,
                                std::initializer_list<std::uint8_t> magic = {})
{
    ExtensionInfo e{};
    std::size_t el = 0;
    while (ext[el] && el < sizeof(e.ext) - 1) { e.ext[el] = ext[el]; ++el; }
    e.ext[el] = 0;

    std::size_t nl = 0;
    while (name[nl] && nl < sizeof(e.name) - 1) { e.name[nl] = name[nl]; ++nl; }

    std::size_t ml = 0;
    while (mime[ml] && ml < sizeof(e.mime) - 1) { e.mime[ml] = mime[ml]; ++ml; }

    e.category = cat;
    e.flags    = flags;

    std::size_t i = 0;
    for (std::uint8_t b : magic) {
        if (i >= sizeof(e.magic)) break;
        e.magic[i++] = b;
    }
    e.magic_len = static_cast<std::uint8_t>(i);
    if (i > 0) e.flags |= EXT_FLAG_NEEDS_MAGIC;
    return e;
}

Result<void> ExtensionRegistry::init_defaults() noexcept {
    count_ = 0;

    /* --- 20 primary extensions -------------------------------------- */

    /* 1. .txt — plain text */
    if (!register_extension(make_ext(".txt", "Plain Text", "text/plain",
        FileCategory::Text, EXT_FLAG_TEXT)).is_ok()) return Error::NoMem;

    /* 2. .cfg — configuration */
    if (!register_extension(make_ext(".cfg", "Configuration", "text/plain",
        FileCategory::Config, EXT_FLAG_TEXT)).is_ok()) return Error::NoMem;

    /* 3. .log — log file */
    if (!register_extension(make_ext(".log", "Log File", "text/plain",
        FileCategory::Log, EXT_FLAG_TEXT)).is_ok()) return Error::NoMem;

    /* 4. .sh — shell script */
    if (!register_extension(make_ext(".sh", "Shell Script",
        "text/x-shellscript", FileCategory::Script,
        EXT_FLAG_TEXT | EXT_FLAG_EXECUTABLE,
        { '#', '!' })).is_ok()) return Error::NoMem;

    /* 5. .md — markdown */
    if (!register_extension(make_ext(".md", "Markdown", "text/markdown",
        FileCategory::Text, EXT_FLAG_TEXT)).is_ok()) return Error::NoMem;

    /* 6. .json — JSON data */
    if (!register_extension(make_ext(".json", "JSON Data", "application/json",
        FileCategory::Data, EXT_FLAG_TEXT,
        { '{' })).is_ok()) return Error::NoMem;

    /* 7. .ini — INI config */
    if (!register_extension(make_ext(".ini", "INI Config", "text/plain",
        FileCategory::Config, EXT_FLAG_TEXT)).is_ok()) return Error::NoMem;

    /* 8. .csv — CSV data */
    if (!register_extension(make_ext(".csv", "CSV Data", "text/csv",
        FileCategory::Table, EXT_FLAG_TEXT)).is_ok()) return Error::NoMem;

    /* 9. .html — HTML document */
    if (!register_extension(make_ext(".html", "HTML Document", "text/html",
        FileCategory::Text, EXT_FLAG_TEXT,
        { '<', '!' })).is_ok()) return Error::NoMem;

    /* 10. .xml — XML data */
    if (!register_extension(make_ext(".xml", "XML Data", "application/xml",
        FileCategory::Data, EXT_FLAG_TEXT,
        { '<', '?' })).is_ok()) return Error::NoMem;

    /* 11. .bin — raw binary */
    if (!register_extension(make_ext(".bin", "Raw Binary",
        "application/octet-stream", FileCategory::Binary,
        EXT_FLAG_BINARY)).is_ok()) return Error::NoMem;

    /* 12. .dat — generic data */
    if (!register_extension(make_ext(".dat", "Data File",
        "application/octet-stream", FileCategory::Data,
        EXT_FLAG_BINARY)).is_ok()) return Error::NoMem;

    /* 13. .elf — ELF executable */
    if (!register_extension(make_ext(".elf", "ELF Executable",
        "application/x-executable", FileCategory::Executable,
        EXT_FLAG_BINARY | EXT_FLAG_EXECUTABLE,
        { 0x7F, 'E', 'L', 'F' })).is_ok()) return Error::NoMem;

    /* 14. .o — object file */
    if (!register_extension(make_ext(".o", "Object File",
        "application/x-object", FileCategory::Binary,
        EXT_FLAG_BINARY,
        { 0x7F, 'E', 'L', 'F' })).is_ok()) return Error::NoMem;

    /* 15. .so — shared library */
    if (!register_extension(make_ext(".so", "Shared Object",
        "application/x-sharedlib", FileCategory::Library,
        EXT_FLAG_BINARY,
        { 0x7F, 'E', 'L', 'F' })).is_ok()) return Error::NoMem;

    /* 16. .tar — tar archive */
    if (!register_extension(make_ext(".tar", "TAR Archive",
        "application/x-tar", FileCategory::Archive,
        EXT_FLAG_BINARY)).is_ok()) return Error::NoMem;

    /* 17. .img — raw disk image */
    if (!register_extension(make_ext(".img", "Disk Image",
        "application/x-rawdisk", FileCategory::Image,
        EXT_FLAG_BINARY)).is_ok()) return Error::NoMem;

    /* 18. .dsk — CoTanOS disk image */
    if (!register_extension(make_ext(".dsk", "CoTanOS Disk",
        "application/x-cotanos-disk", FileCategory::CoTanOS,
        EXT_FLAG_BINARY | EXT_FLAG_SYSTEM,
        { 'C', 'O', 'T', 'D', 'S', 'K' })).is_ok()) return Error::NoMem;

    /* 19. .cot — CoTanOS executable */
    if (!register_extension(make_ext(".cot", "CoTanOS Executable",
        "application/x-cotanos-exec", FileCategory::Executable,
        EXT_FLAG_BINARY | EXT_FLAG_EXECUTABLE | EXT_FLAG_SYSTEM,
        { 'C', 'O', 'T', 'E', 'X', 'E' })).is_ok()) return Error::NoMem;

    /* 20. .sys — CoTanOS system file (read-only) */
    if (!register_extension(make_ext(".sys", "CoTanOS System File",
        "application/x-cotanos-system", FileCategory::System,
        EXT_FLAG_BINARY | EXT_FLAG_SYSTEM | EXT_FLAG_READONLY,
        { 'C', 'O', 'T', 'S', 'Y', 'S' })).is_ok()) return Error::NoMem;

    /* --- 12 custom CoTanOS extensions ------------------------------- */

    if (!register_extension(make_ext(".cotl", "CoTanOS Library",
        "application/x-cotanos-library", FileCategory::CoTanOS,
        EXT_FLAG_BINARY | EXT_FLAG_SYSTEM,
        { 'C', 'O', 'T', 'L', 'I', 'B' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotk", "CoTanOS Toolkit",
        "application/x-cotanos-toolkit", FileCategory::CoTanOS,
        EXT_FLAG_BINARY | EXT_FLAG_EXECUTABLE,
        { 'C', 'O', 'T', 'T', 'K', 'T' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotd", "CoTanOS Data",
        "application/x-cotanos-data", FileCategory::CoTanOS,
        EXT_FLAG_BINARY,
        { 'C', 'O', 'T', 'D', 'A', 'T' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotp", "CoTanOS Package",
        "application/x-cotanos-pkg", FileCategory::CoTanOS,
        EXT_FLAG_BINARY,
        { 'C', 'O', 'T', 'P', 'K', 'G' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotb", "CoTanOS Binary Blob",
        "application/x-cotanos-blob", FileCategory::Binary,
        EXT_FLAG_BINARY,
        { 'C', 'O', 'T', 'B', 'L', 'B' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotm", "CoTanOS Module",
        "application/x-cotanos-module", FileCategory::CoTanOS,
        EXT_FLAG_BINARY | EXT_FLAG_SYSTEM,
        { 'C', 'O', 'T', 'M', 'O', 'D' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".coth", "CoTanOS Header",
        "text/x-cotanos-header", FileCategory::Source,
        EXT_FLAG_TEXT)).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotr", "CoTanOS Resource",
        "application/x-cotanos-res", FileCategory::CoTanOS,
        EXT_FLAG_BINARY,
        { 'C', 'O', 'T', 'R', 'E', 'S' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotx", "CoTanOS Extended Exec",
        "application/x-cotanos-exec2", FileCategory::Executable,
        EXT_FLAG_BINARY | EXT_FLAG_EXECUTABLE,
        { 'C', 'O', 'T', 'E', 'X', '2' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotc", "CoTanOS Config",
        "text/x-cotanos-cfg", FileCategory::Config,
        EXT_FLAG_TEXT | EXT_FLAG_SYSTEM)).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotf", "CoTanOS Firmware",
        "application/x-cotanos-fw", FileCategory::System,
        EXT_FLAG_BINARY | EXT_FLAG_SYSTEM | EXT_FLAG_READONLY,
        { 'C', 'O', 'T', 'F', 'W', '0' })).is_ok()) return Error::NoMem;

    if (!register_extension(make_ext(".cotz", "CoTanOS Compressed",
        "application/x-cotanos-z", FileCategory::Archive,
        EXT_FLAG_BINARY,
        { 'C', 'O', 'T', 'Z', 'I', 'P' })).is_ok()) return Error::NoMem;

    return {};
}

Result<void> ExtensionRegistry::register_extension(const ExtensionInfo &info) noexcept {
    if (count_ >= MAX_EXTENSIONS) return Error::NoMem;
    /* Reject duplicates. */
    for (std::size_t i = 0; i < count_; ++i)
        if (ext_equals(exts_[i].ext, info.ext)) return Error::Exists;
    exts_[count_++] = info;
    return {};
}

Result<const ExtensionInfo *> ExtensionRegistry::lookup(const char *name) noexcept {
    const char *ext = extension_of(name);
    for (std::size_t i = 0; i < count_; ++i)
        if (ext_equals(exts_[i].ext, ext)) return &exts_[i];
    return Error::NotFound;
}

Result<FileCategory> ExtensionRegistry::categorize(const char *name) noexcept {
    auto r = lookup(name);
    if (!r.is_ok()) return FileCategory::Unknown;
    return r.value()->category;
}

const ExtensionInfo *ExtensionRegistry::detect_by_magic(const std::uint8_t *data,
                                                          std::size_t len) noexcept
{
    if (!data || len == 0) return nullptr;
    for (std::size_t i = 0; i < count_; ++i) {
        const ExtensionInfo &e = exts_[i];
        if (!(e.flags & EXT_FLAG_NEEDS_MAGIC) || e.magic_len == 0) continue;
        if (e.magic_len > len) continue;
        if (std::memcmp(data, e.magic, e.magic_len) == 0) return &e;
    }
    return nullptr;
}

/* ========================================================================== */
/* SECTOR MAP                                                                 */
/* ========================================================================== */

void SectorMap::reset() noexcept {
    for (auto &r : ranges_) r = {};
    count_ = 0;
}

Result<void> SectorMap::claim(std::uint64_t start, std::uint64_t length,
                                std::uint64_t owner_ino) noexcept
{
    if (length == 0) return Error::Invalid;

    /* Try to merge with an adjacent range of the same owner. */
    for (std::size_t i = 0; i < count_; ++i) {
        auto &r = ranges_[i];
        if (r.owner_ino != owner_ino) continue;
        if (r.start + r.length == start) { r.length += length; return {}; }
        if (start + length == r.start)   { r.start = start; r.length += length; return {}; }
    }

    if (count_ >= MAX_SECTOR_RANGES) return Error::NoMem;
    ranges_[count_++] = SectorRange{ start, length, owner_ino };
    return {};
}

Result<void> SectorMap::release(std::uint64_t start, std::uint64_t length) noexcept {
    if (length == 0) return Error::Invalid;

    for (std::size_t i = 0; i < count_; ++i) {
        auto &r = ranges_[i];
        if (r.start == start && r.length == length) {
            for (std::size_t j = i + 1; j < count_; ++j) ranges_[j - 1] = ranges_[j];
            --count_;
            return {};
        }
        if (r.start <= start && start + length <= r.start + r.length) {
            /* Split. */
            if (start == r.start) {
                r.start  += length;
                r.length -= length;
                return {};
            }
            if (start + length == r.start + r.length) {
                r.length -= length;
                return {};
            }
            /* Middle removal -> two ranges. */
            if (count_ >= MAX_SECTOR_RANGES) return Error::NoMem;
            SectorRange tail{ start + length,
                              r.start + r.length - (start + length),
                              r.owner_ino };
            r.length = start - r.start;
            ranges_[count_++] = tail;
            return {};
        }
    }
    return Error::NotFound;
}

Result<std::uint64_t> SectorMap::owner_of(std::uint64_t sector) const noexcept {
    for (std::size_t i = 0; i < count_; ++i) {
        const auto &r = ranges_[i];
        if (sector >= r.start && sector < r.start + r.length) return r.owner_ino;
    }
    return Error::NotFound;
}

Result<Extent> SectorMap::next_extent(std::uint64_t owner_ino,
                                        std::uint64_t after_sector) const noexcept
{
    std::uint64_t best_start = UINT64_MAX;
    std::uint64_t best_len   = 0;
    for (std::size_t i = 0; i < count_; ++i) {
        const auto &r = ranges_[i];
        if (r.owner_ino != owner_ino) continue;
        if (r.start < after_sector) continue;
        if (r.start < best_start) { best_start = r.start; best_len = r.length; }
    }
    if (best_start == UINT64_MAX) return Error::NotFound;
    return Extent{ best_start, best_len };
}

/* ========================================================================== */
/* TYPE GUARD                                                                 */
/* ========================================================================== */

namespace TypeGuard {

GuardCheck verify_open(const ExtensionRegistry &reg, const char *name,
                        const std::uint8_t *data, std::size_t len) noexcept
{
    if (len == 0) return GuardCheck::EmptyFile;
    auto info = reg.lookup(name);
    if (!info.is_ok()) return GuardCheck::UnknownExt;
    if (!(info.value()->flags & EXT_FLAG_NEEDS_MAGIC)) return GuardCheck::Ok;
    if (len < info.value()->magic_len) return GuardCheck::Ok;   // too short to check

    if (std::memcmp(data, info.value()->magic, info.value()->magic_len) == 0)
        return GuardCheck::Ok;
    return GuardCheck::Mismatch;
}

GuardCheck verify_exec(const ExtensionRegistry &reg, const char *name,
                        const std::uint8_t *data, std::size_t len) noexcept
{
    if (len == 0) return GuardCheck::EmptyFile;
    auto info = reg.lookup(name);
    if (!info.is_ok()) return GuardCheck::NotExecutable;
    if (!(info.value()->flags & EXT_FLAG_EXECUTABLE)) return GuardCheck::NotExecutable;
    if ((info.value()->flags & EXT_FLAG_NEEDS_MAGIC) &&
        (len >= info.value()->magic_len) &&
        std::memcmp(data, info.value()->magic, info.value()->magic_len) != 0)
        return GuardCheck::Mismatch;
    return GuardCheck::Ok;
}

GuardCheck verify_write(const ExtensionRegistry &reg, const char *name,
                         std::uint64_t new_size) noexcept
{
    (void)new_size;
    auto info = reg.lookup(name);
    if (!info.is_ok()) return GuardCheck::UnknownExt;
    if (info.value()->flags & EXT_FLAG_READONLY) return GuardCheck::Forbidden;
    return GuardCheck::Ok;
}

const char *check_name(GuardCheck c) noexcept {
    switch (c) {
    case GuardCheck::Ok:            return "ok";
    case GuardCheck::EmptyFile:     return "empty";
    case GuardCheck::Mismatch:      return "mismatch";
    case GuardCheck::UnknownExt:    return "unknown-ext";
    case GuardCheck::Forbidden:     return "forbidden";
    case GuardCheck::NotExecutable: return "not-executable";
    }
    return "?";
}

} // namespace TypeGuard

/* ========================================================================== */
/* FILE MANAGER — construction / init                                         */
/* ========================================================================== */

FileManager::FileManager(FileSystem &fs) noexcept : fs_(fs) {}

Result<void> FileManager::init() noexcept {
    auto r = registry_.init_defaults();
    if (!r.is_ok()) return r;

    map_.reset();
    r = rebuild_sector_map();
    if (!r.is_ok()) return r;

    initialized_ = true;
    return {};
}

Result<void> FileManager::rebuild_sector_map() noexcept {
    map_.reset();
    /* Walk root + all reachable nodes. */
    auto r = walk_node(fs_.header().root_sector);
    if (!r.is_ok()) return r;
    return {};
}

Result<void> FileManager::walk_node(std::uint64_t sec) noexcept {
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), sec, n);
    if (!nr.is_ok()) return nr;

    /* Claim each extent. */
    for (const auto &e : n.extents) {
        if (e.is_empty()) continue;
        auto r = map_.claim(e.block, e.length, sec);
        if (!r.is_ok()) return r;
    }

    /* Walk overflow chain. */
    if (n.extents_next) {
        ExtentBlock blk{};
        auto rr = fs_.disk_for_manager().read_at(n.extents_next, &blk, sizeof(blk));
        if (rr.is_ok()) {
            for (const auto &e : blk.extents) {
                if (e.is_empty()) continue;
                auto r = map_.claim(e.block, e.length, sec);
                if (!r.is_ok()) return r;
            }
        }
        (void)map_.claim(n.extents_next, 1, sec);
    }

    /* Directory: walk children. */
    if (n.is_dir()) {
        std::uint64_t child = n.next;
        while (child != 0) {
            auto cr = walk_node(child);
            if (!cr.is_ok()) return cr;
            Node c{};
            auto rr = node_read(fs_.disk_for_manager(), child, c);
            if (!rr.is_ok()) return rr;
            child = c.next;
        }
    }
    return {};
}

/* ========================================================================== */
/* FILE MANAGER — directory operations                                        */
/* ========================================================================== */

Result<void> FileManager::mkdir(const char *path, std::uint16_t mode) noexcept {
    auto r = fs_.mkdir(path, mode);
    if (!r.is_ok()) return r;
    /* Claim the new node's sector. */
    auto sec = fs_.lookup(path);
    if (sec.is_ok()) (void)map_.claim(sec.value(), 1, sec.value());
    return {};
}

Result<void> FileManager::mkdir_p(const char *path) noexcept {
    if (!path || !*path) return Error::Invalid;
    char buf[MAX_NAME_LEN * 16];
    std::size_t len = 0;
    while (path[len] && len < sizeof(buf) - 1) { buf[len] = path[len]; ++len; }
    buf[len] = 0;

    for (std::size_t i = 0; i <= len; ++i) {
        if (buf[i] == '/' || buf[i] == 0) {
            char saved = buf[i];
            buf[i] = 0;
            if (buf[0] != 0) {
                auto r = fs_.lookup(buf);
                if (!r.is_ok()) {
                    auto cr = fs_.mkdir(buf);
                    if (!cr.is_ok() && cr.error() != Error::Exists) return cr;
                }
            }
            buf[i] = saved;
            if (saved == 0) break;
        }
    }
    return rebuild_sector_map();
}

Result<void> FileManager::rmdir(const char *path) noexcept {
    auto r = fs_.rmdir(path);
    if (!r.is_ok()) return r;
    return rebuild_sector_map();
}

Result<DirEntry> FileManager::readdir(std::uint64_t dir_sector,
                                        std::uint32_t index) noexcept
{
    return fs_.readdir(dir_sector, index);
}

Result<std::uint32_t> FileManager::count_children(std::uint64_t dir_sector) noexcept {
    Node dir{};
    auto r = node_read(fs_.disk_for_manager(), dir_sector, dir);
    if (!r.is_ok()) return r.error();
    if (!dir.is_dir()) return Error::NotDirectory;

    std::uint32_t n = 0;
    std::uint64_t child = dir.next;
    while (child != 0) {
        Node c{};
        auto cr = node_read(fs_.disk_for_manager(), child, c);
        if (!cr.is_ok()) return Error::Corrupt;
        child = c.next;
        ++n;
    }
    return n;
}

/* ========================================================================== */
/* FILE MANAGER — file operations                                             */
/* ========================================================================== */

Result<void> FileManager::create(const char *path, std::uint16_t mode) noexcept {
    auto r = fs_.create(path, mode);
    if (!r.is_ok()) return r;
    auto sec = fs_.lookup(path);
    if (sec.is_ok()) (void)map_.claim(sec.value(), 1, sec.value());
    return {};
}

Result<void> FileManager::unlink(const char *path) noexcept {
    auto r = fs_.unlink(path);
    if (!r.is_ok()) return r;
    return rebuild_sector_map();
}

Result<void> FileManager::rename(const char *old_path, const char *new_path) noexcept {
    /* Simple rename: create new link, unlink old.
     * For real atomic rename we'd need directory-entry update. */
    auto nr = fs_.lookup(new_path);
    if (nr.is_ok()) return Error::Exists;

    auto orr = fs_.lookup(old_path);
    if (!orr.is_ok()) return orr;
    std::uint64_t old_sec = orr.value();

    /* Read old node, adjust name + parent, write to a new node sector. */
    Node old_n{};
    auto nr2 = node_read(fs_.disk_for_manager(), old_sec, old_n);
    if (!nr2.is_ok()) return nr2;

    /* Resolve new parent. */
    const char *last = new_path;
    for (const char *p = new_path; *p; ++p) if (*p == '/') last = p + 1;
    char new_parent_path[MAX_NAME_LEN * 8];
    if (last == new_path) std::memcpy(new_parent_path, "/", 2);
    else {
        std::size_t plen = static_cast<std::size_t>(last - new_path);
        std::memcpy(new_parent_path, new_path, plen);
        new_parent_path[plen] = 0;
    }
    auto pr = fs_.lookup(new_parent_path);
    if (!pr.is_ok()) return pr;

    Node new_n = old_n;
    new_n.parent = pr.value();
    char name_buf[MAX_NAME_LEN + 1];
    std::size_t nlen = 0;
    while (last[nlen] && nlen < MAX_NAME_LEN) { name_buf[nlen] = last[nlen]; ++nlen; }
    name_buf[nlen] = 0;
    node_set_name(new_n, name_buf);

    /* Write as a fresh node, unlink old. */
    auto sr = fs_.alloc_sector();
    if (!sr.is_ok()) return Error::NoSpace;
    std::uint64_t new_sec = sr.value();
    auto wr = node_write(fs_.disk_for_manager(), new_sec, new_n);
    if (!wr.is_ok()) return wr;

    (void)fs_.unlink(old_path);
    return rebuild_sector_map();
}

Result<void> FileManager::copy(const char *src, const char *dst) noexcept {
    auto sr = read_file(src, nullptr, 0);   // just check exists
    if (!sr.is_ok()) return sr;

    auto st = fs_.stat(src);
    if (!st.is_ok()) return st;

    std::uint64_t size = st.value().size;
    if (size == 0) return create(dst);

    /* Stream in 4 KiB chunks. */
    constexpr std::size_t CHUNK = 4096;
    std::uint8_t buf[CHUNK];
    int in_fd = -1, out_fd = -1;

    auto ir = fs_.open(src, O_RDONLY);
    if (!ir.is_ok()) return ir;
    in_fd = ir.value();

    auto cr = fs_.open(dst, O_WRONLY | O_CREAT | O_TRUNC);
    if (!cr.is_ok()) { (void)fs_.close(in_fd); return cr; }
    out_fd = cr.value();

    std::uint64_t remaining = size;
    while (remaining > 0) {
        std::size_t chunk = remaining < CHUNK ? remaining : CHUNK;
        auto rr = fs_.read(in_fd, buf, chunk);
        if (!rr.is_ok() || rr.value() == 0) break;
        auto wr = fs_.write(out_fd, buf, rr.value());
        if (!wr.is_ok()) break;
        remaining -= rr.value();
    }
    (void)fs_.close(in_fd);
    (void)fs_.close(out_fd);
    return rebuild_sector_map();
}

Result<void> FileManager::move(const char *src, const char *dst) noexcept {
    auto r = rename(src, dst);
    if (r.is_ok()) return r;
    /* Fallback: copy + unlink. */
    auto cr = copy(src, dst);
    if (!cr.is_ok()) return cr;
    return unlink(src);
}

Result<std::size_t> FileManager::read_file(const char *path, void *buf,
                                             std::size_t max) noexcept
{
    auto r = fs_.lookup(path);
    if (!r.is_ok()) return r.error();
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), r.value(), n);
    if (!nr.is_ok()) return nr;
    if (!n.is_file()) return Error::IsDirectory;
    return read_all_extents(n, 0, buf, max ? max : n.size);
}

Result<std::size_t> FileManager::write_file(const char *path, const void *buf,
                                              std::size_t len) noexcept
{
    auto r = fs_.lookup(path);
    if (!r.is_ok()) {
        if (r.error() != Error::NotFound) return r;
        auto cr = create(path);
        if (!cr.is_ok()) return cr;
        r = fs_.lookup(path);
        if (!r.is_ok()) return r;
    }
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), r.value(), n);
    if (!nr.is_ok()) return nr;
    if (!n.is_file()) return Error::IsDirectory;

    auto check = TypeGuard::verify_write(registry_, path, len);
    if (check == GuardCheck::Forbidden) return Error::Permission;

    /* Truncate existing data. */
    if (n.size > 0) {
        for (auto &e : n.extents) {
            if (!e.is_empty()) { map_.release(e.block, e.length); fs_.free_extent(e.block, e.length); }
            e = {};
        }
        n.size = 0;
    }

    auto wr = write_all_extents(n, 0, buf, len);
    if (!wr.is_ok()) return wr;
    auto nd = node_write(fs_.disk_for_manager(), r.value(), n);
    if (!nd.is_ok()) return nd;
    return wr;
}

Result<std::size_t> FileManager::append_file(const char *path, const void *buf,
                                               std::size_t len) noexcept
{
    auto r = fs_.lookup(path);
    if (!r.is_ok()) return r;
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), r.value(), n);
    if (!nr.is_ok()) return nr;
    if (!n.is_file()) return Error::IsDirectory;

    auto check = TypeGuard::verify_write(registry_, path, n.size + len);
    if (check == GuardCheck::Forbidden) return Error::Permission;

    auto wr = write_all_extents(n, n.size, buf, len);
    if (!wr.is_ok()) return wr;
    auto nd = node_write(fs_.disk_for_manager(), r.value(), n);
    if (!nd.is_ok()) return nd;
    return wr;
}

/* ========================================================================== */
/* FILE MANAGER — metadata                                                    */
/* ========================================================================== */

Result<Stat> FileManager::stat(const char *path) noexcept {
    return fs_.stat(path);
}

Result<void> FileManager::chmod(const char *path, std::uint16_t mode) noexcept {
    auto r = fs_.lookup(path);
    if (!r.is_ok()) return r;
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), r.value(), n);
    if (!nr.is_ok()) return nr;
    n.mode = static_cast<std::uint16_t>((n.mode & MODE_TYPE) | (mode & 0777));
    auto wr = node_write(fs_.disk_for_manager(), r.value(), n);
    return wr.is_ok() ? Result<void>{} : wr;
}

Result<void> FileManager::chown(const char *path, std::uint32_t uid,
                                  std::uint32_t gid) noexcept
{
    auto r = fs_.lookup(path);
    if (!r.is_ok()) return r;
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), r.value(), n);
    if (!nr.is_ok()) return nr;
    n.uid = uid;
    n.gid = gid;
    auto wr = node_write(fs_.disk_for_manager(), r.value(), n);
    return wr.is_ok() ? Result<void>{} : wr;
}

Result<void> FileManager::truncate(const char *path, std::uint64_t size) noexcept {
    return fs_.truncate(path, size);
}

/* ========================================================================== */
/* FILE MANAGER — paths                                                       */
/* ========================================================================== */

Result<std::uint64_t> FileManager::resolve(const char *path,
                                             std::uint64_t cwd) noexcept
{
    return fs_.lookup(path, cwd);
}

Result<bool> FileManager::is_dir(const char *path) noexcept {
    auto r = fs_.lookup(path);
    if (!r.is_ok()) return r.error();
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), r.value(), n);
    if (!nr.is_ok()) return nr;
    return n.is_dir();
}

Result<bool> FileManager::is_file(const char *path) noexcept {
    auto r = fs_.lookup(path);
    if (!r.is_ok()) return r.error();
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), r.value(), n);
    if (!nr.is_ok()) return nr;
    return n.is_file();
}

Result<void> FileManager::path_of_sector(std::uint64_t sec, char *out,
                                           std::size_t max) noexcept
{
    if (!out || max < 2) return Error::Invalid;
    if (!fs_.mounted()) return Error::NotSupported;

    constexpr std::size_t MAX_DEPTH = 16;
    const char *names[MAX_DEPTH];
    char name_bufs[MAX_DEPTH][MAX_NAME_LEN + 1];

    std::size_t depth = 0;
    std::uint64_t cur = sec;
    std::uint64_t root = fs_.header().root_sector;

    while (cur != root && cur != 0 && depth < MAX_DEPTH) {
        Node n{};
        auto r = node_read(fs_.disk_for_manager(), cur, n);
        if (!r.is_ok()) return r;
        std::size_t nl = 0;
        while (nl < MAX_NAME_LEN && n.name[nl]) { name_bufs[depth][nl] = n.name[nl]; ++nl; }
        name_bufs[depth][nl] = 0;
        names[depth] = name_bufs[depth];
        ++depth;
        cur = n.parent;
    }

    std::size_t pos = 0;
    out[pos++] = '/';
    for (std::size_t i = depth; i > 0; --i) {
        const char *nm = names[i - 1];
        std::size_t nl = 0;
        while (nm[nl]) { if (pos + 1 >= max) return Error::NameTooLong; out[pos++] = nm[nl++]; }
        if (i > 1) {
            if (pos + 1 >= max) return Error::NameTooLong;
            out[pos++] = '/';
        }
    }
    out[pos] = 0;
    return {};
}

/* ========================================================================== */
/* FILE MANAGER — sector mapping / integrity                                  */
/* ========================================================================== */

Result<Extent> FileManager::map_logical(std::uint64_t ino,
                                          std::uint64_t byte_offset) noexcept
{
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), ino, n);
    if (!nr.is_ok()) return nr;

    std::uint64_t file_off = 0;
    for (std::uint32_t i = 0; i < INLINE_EXTENTS; ++i) {
        const Extent &e = n.extents[i];
        if (e.is_empty()) return Error::Range;
        std::uint64_t ext_end = file_off + e.length * SECTOR_SIZE;
        if (byte_offset < ext_end) {
            std::uint64_t delta = byte_offset - file_off;
            return Extent{ e.block + delta / SECTOR_SIZE, 1 };
        }
        file_off = ext_end;
    }
    return Error::Range;
}

Result<void> FileManager::defragment(std::uint64_t ino) noexcept {
    /* Read all extents, allocate one contiguous block, rewrite. */
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), ino, n);
    if (!nr.is_ok()) return nr;
    if (!n.is_file()) return Error::IsDirectory;
    if (n.size == 0) return {};

    std::size_t total_sectors = (n.size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    auto ar = fs_.alloc_extent(total_sectors);
    if (!ar.is_ok()) return Error::NoSpace;
    std::uint64_t start = ar.value();

    /* Copy data. */
    std::uint8_t buf[SECTOR_SIZE];
    std::size_t done = 0;
    for (std::uint32_t i = 0; i < INLINE_EXTENTS; ++i) {
        const Extent &e = n.extents[i];
        if (e.is_empty()) break;
        for (std::uint64_t s = 0; s < e.length; ++s) {
            auto rr = fs_.disk_for_manager().read_at(e.block + s, buf, SECTOR_SIZE);
            if (!rr.is_ok()) return Error::Io;
            auto wr = fs_.disk_for_manager().write_at(start + done, buf, SECTOR_SIZE);
            if (!wr.is_ok()) return Error::Io;
            ++done;
        }
    }

    /* Free old, install new. */
    for (auto &e : n.extents) {
        if (!e.is_empty()) { fs_.free_extent(e.block, e.length); }
        e = {};
    }
    n.extents[0] = Extent{ start, total_sectors };
    auto wr = node_write(fs_.disk_for_manager(), ino, n);
    return wr.is_ok() ? Result<void>{} : wr;
}

Result<void> FileManager::verify() noexcept {
    /* Rebuild sector map from nodes. */
    auto r = rebuild_sector_map();
    if (!r.is_ok()) return r;

    /* Every claimed extent should also be marked used by the allocator,
     * i.e. NOT be in the free pool. We can only verify this indirectly
     * by attempting to allocate exactly those sectors — but that would
     * mutate state. For v1 we just report basic stats. */

    /* Report any extents whose owners are invalid. */
    bool ok = true;
    map_.foreach_range([&](const SectorRange &r2) {
        Node owner{};
        auto rr = node_read(fs_.disk_for_manager(), r2.owner_ino, owner);
        if (!rr.is_ok()) ok = false;
    });
    return ok ? Result<void>{} : Error::Corrupt;
}

Result<void> FileManager::trim() noexcept {
    /* Free all extents not owned by any node (only makes sense if
     * allocator got out of sync). For now: rebuild map + verify. */
    return verify();
}

/* ========================================================================== */
/* FILE MANAGER — extension handling                                          */
/* ========================================================================== */

Result<const ExtensionInfo *> FileManager::identify(const char *path) noexcept {
    auto r = fs_.lookup(path);
    if (!r.is_ok()) return r;
    Node n{};
    auto nr = node_read(fs_.disk_for_manager(), r.value(), n);
    if (!nr.is_ok()) return nr;

    /* Try extension first. */
    auto info = registry_.lookup(path);
    if (info.is_ok()) return info;

    /* Fallback to magic detection (read first 512 bytes). */
    std::uint8_t hdr[512];
    std::size_t hlen = n.size < sizeof(hdr) ? n.size : sizeof(hdr);
    if (hlen > 0) {
        auto rr = read_all_extents(n, 0, hdr, hlen);
        if (rr.is_ok()) {
            const ExtensionInfo *magic_ext = registry_.detect_by_magic(hdr, hlen);
            if (magic_ext) return magic_ext;
        }
    }
    return Error::NotFound;
}

const ExtensionInfo *FileManager::detect_by_magic(const std::uint8_t *data,
                                                    std::size_t len) noexcept
{
    return registry_.detect_by_magic(data, len);
}

Result<void> FileManager::register_extension(const ExtensionInfo &info) noexcept {
    return registry_.register_extension(info);
}

/* ========================================================================== */
/* FILE MANAGER — sync                                                        */
/* ========================================================================== */

Result<void> FileManager::sync() noexcept {
    return fs_.unmount_and_remount_internal();
}

/* ========================================================================== */
/* FILE MANAGER — internal extent I/O                                         */
/* ========================================================================== */

Result<std::size_t> FileManager::read_all_extents(const Node &n,
                                                    std::uint64_t offset,
                                                    void *buf,
                                                    std::size_t count) noexcept
{
    return node_read_data_public(fs_, n, offset, buf, count);
}

Result<std::size_t> FileManager::write_all_extents(Node &n,
                                                     std::uint64_t offset,
                                                     const void *buf,
                                                     std::size_t count) noexcept
{
    return node_write_data_public(fs_, n, offset, buf, count);
}

} // namespace fm512
