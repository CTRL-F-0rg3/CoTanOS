/*
 * ============================================================================
 *  512FM_FILESYSTEM.CPP - Main filesystem implementation
 *
 *  On-disk layout (all 512-byte sectors):
 *    [0]       Header (COW ring: header.generation tracks current copy)
 *    [1]       Root Node
 *    [2..N]    Free-list blocks (allocator levels)
 *    [N+1..]   Data blocks (nodes + file data)
 *
 *  COW model:
 *    - Node updates write a fresh node sector; the old sector is freed.
 *    - Directory updates rewrite the affected node.
 *    - The header is rewritten last, atomically switching root_sector.
 *    - If a write fails mid-transaction, the old header is still on disk.
 * ============================================================================
 */

#include "512fm_filesystem.hpp"

namespace fm512 {

/* ========================================================================== */
/* Helpers                                                                    */
/* ========================================================================== */

static __attribute__((always_inline)) bool is_path_sep(char c) noexcept {
    return c == '/' || c == '\\';
}

/* Split `path` into components; caller iterates `next_component`. */
struct PathIterator {
    const char *p;

    explicit PathIterator(const char *path) noexcept : p(path) {}

    /* Returns the next component into `buf` (max MAX_NAME_LEN+1), returns
     * false when done. */
    bool next(char *buf, std::uint32_t &len) noexcept {
        while (*p && is_path_sep(*p)) ++p;
        if (!*p) return false;
        len = 0;
        while (*p && !is_path_sep(*p)) {
            if (len < MAX_NAME_LEN) buf[len++] = *p;
            ++p;
        }
        buf[len] = 0;
        return true;
    }
};

/* ========================================================================== */
/* Construction / lifecycle                                                   */
/* ========================================================================== */

FileSystem::FileSystem(Disk &disk) noexcept : disk_(disk) {}

FileSystem::~FileSystem() noexcept {
    if (mounted_) (void)unmount();
}

Result<void> FileSystem::load_header() noexcept {
    auto r = disk_.read_at(HEADER_SECTOR, &header_, sizeof(header_));
    if (!r.is_ok()) return Error::Io;
    if (!header_.valid()) return Error::Corrupt;
    return {};
}

Result<void> FileSystem::write_header() noexcept {
    header_.checksum = 0;   /* checksum computed over zeroed field */
    /* Simple XOR checksum over header body. */
    std::uint32_t ck = 0;
    const std::uint32_t *w = reinterpret_cast<const std::uint32_t *>(&header_);
    for (std::size_t i = 0; i < sizeof(header_) / 4; ++i)
        if (i != offsetof(Header, checksum) / 4) ck ^= w[i];
    header_.checksum = ck;

    auto r = disk_.write_at(HEADER_SECTOR, &header_, sizeof(header_));
    if (!r.is_ok()) return Error::Io;
    return {};
}

Result<void> FileSystem::format(const std::uint8_t uuid[16]) noexcept {
    if (mounted_) return Error::Busy;

    auto r = disk_.size();
    if (!r.is_ok()) return Error::Io;
    std::uint64_t total = r.value();
    if (total < 16) return Error::NoSpace;

    std::memset(&header_, 0, sizeof(header_));
    header_.signature     = SIGNATURE;
    header_.version       = VERSION;
    header_.block_size    = BLOCK_SIZE;
    header_.total_sectors = total;
    header_.root_sector   = ROOT_SECTOR;
    header_.free_head     = ROOT_SECTOR + 1;
    header_.next_free     = 0;
    header_.generation    = 1;
    if (uuid) std::memcpy(header_.uuid, uuid, 16);

    /* Initial allocator: everything beyond reserved area is free. */
    alloc_.reset();
    std::uint64_t data_start = ROOT_SECTOR + node_sectors() + BLOCK_LEVELS;
    if (data_start < total)
        alloc_.add_free_extent(data_start, total - data_start);

    /* Write root node (empty directory). */
    Node root{};
    node_init(root, MODE_DIR | 0755, 0, 0, "/", ROOT_SECTOR);
    auto wr = node_write(disk_, ROOT_SECTOR, root);
    if (!wr.is_ok()) return Error::Io;

    /* Persist allocator. */
    auto pr = alloc_.persist(disk_, header_.free_head);
    if (!pr.is_ok()) return Error::NoSpace;
    header_.free_head = pr.value();

    return write_header();
}

Result<void> FileSystem::mount() noexcept {
    if (mounted_) return Error::Busy;

    auto r = load_header();
    if (!r.is_ok()) return r.error();

    auto ar = alloc_.load(disk_, header_.free_head);
    if (!ar.is_ok()) return Error::Corrupt;

    mounted_ = true;
    return {};
}

Result<void> FileSystem::unmount() noexcept {
    if (!mounted_) return Error::Invalid;

    auto pr = alloc_.persist(disk_, header_.free_head);
    if (pr.is_ok()) header_.free_head = pr.value();
    ++header_.generation;
    (void)write_header();

    mounted_ = false;
    std::memset(fds_, 0, sizeof(fds_));
    return {};
}

/* ========================================================================== */
/* Sector allocation                                                          */
/* ========================================================================== */

Result<std::uint64_t> FileSystem::alloc_sector() noexcept {
    return alloc_.allocate(1);
}

Result<std::uint64_t> FileSystem::alloc_extent(std::uint64_t count) noexcept {
    return alloc_.allocate(count);
}

void FileSystem::free_sector(std::uint64_t sector) noexcept {
    alloc_.free_sectors(sector, 1);
}

void FileSystem::free_node_data(Node &n) noexcept {
    for (auto &e : n.extents) {
        if (!e.is_empty()) alloc_.free_sectors(e.block, e.length);
    }
    if (n.extents_next) {
        ExtentBlock blk{};
        auto r = disk_.read_at(n.extents_next, &blk, sizeof(blk));
        if (r.is_ok()) {
            for (auto &e : blk.extents)
                if (!e.is_empty()) alloc_.free_sectors(e.block, e.length);
        }
        alloc_.free_sectors(n.extents_next, 1);
    }
}

/* ========================================================================== */
/* Directory operations                                                       */
/* ========================================================================== */

Result<std::uint64_t> FileSystem::dir_lookup(std::uint64_t dir_sector,
                                               const char *name,
                                               std::uint32_t name_len) noexcept
{
    Node dir{};
    auto r = node_read(disk_, dir_sector, dir);
    if (!r.is_ok()) return r.error();
    if (!dir.is_dir()) return Error::NotDirectory;

    /* Directory is a linked list of child nodes via `next`. */
    std::uint64_t child = dir.next;
    while (child != 0) {
        Node c{};
        auto cr = node_read(disk_, child, c);
        if (!cr.is_ok()) return Error::Corrupt;

        std::uint32_t cl = 0;
        while (cl < MAX_NAME_LEN && c.name[cl]) ++cl;
        if (cl == name_len && std::memcmp(c.name, name, name_len) == 0)
            return child;

        child = c.next;
    }
    return Error::NotFound;
}

Result<void> FileSystem::dir_link(std::uint64_t dir_sector,
                                    std::uint64_t child_sector) noexcept
{
    Node dir{};
    auto r = node_read(disk_, dir_sector, dir);
    if (!r.is_ok()) return r.error();
    if (!dir.is_dir()) return Error::NotDirectory;

    Node child{};
    auto cr = node_read(disk_, child_sector, child);
    if (!cr.is_ok()) return cr.error();

    child.next = dir.next;
    dir.next   = child_sector;

    auto w1 = node_write(disk_, child_sector, child);
    if (!w1.is_ok()) return w1.error();
    auto w2 = node_write(disk_, dir_sector, dir);
    if (!w2.is_ok()) return w2.error();
    return {};
}

Result<void> FileSystem::dir_unlink(std::uint64_t dir_sector,
                                      std::uint64_t child_sector) noexcept
{
    Node dir{};
    auto r = node_read(disk_, dir_sector, dir);
    if (!r.is_ok()) return r.error();

    if (dir.next == child_sector) {
        dir.next = 0;
        return node_write(disk_, dir_sector, dir).is_ok()
            ? Result<void>{} : Result<void>{Error::Io};
    }

    std::uint64_t prev = dir.next;
    while (prev != 0) {
        Node p{};
        auto pr = node_read(disk_, prev, p);
        if (!pr.is_ok()) return Error::Corrupt;
        if (p.next == child_sector) {
            p.next = 0;
            auto wr = node_write(disk_, prev, p);
            return wr.is_ok() ? Result<void>{} : Result<void>{Error::Io};
        }
        prev = p.next;
    }
    return Error::NotFound;
}

/* ========================================================================== */
/* Path resolution                                                            */
/* ========================================================================== */

Result<std::uint64_t> FileSystem::lookup(const char *path,
                                           std::uint64_t cwd_sector) noexcept
{
    if (!mounted_ || !path) return Error::Invalid;

    std::uint64_t cur = (path[0] == '/') ? header_.root_sector : cwd_sector;
    PathIterator it(path);

    char comp[MAX_NAME_LEN + 1];
    std::uint32_t clen = 0;
    while (it.next(comp, clen)) {
        if (clen == 0) continue;
        if (clen == 1 && comp[0] == '.') continue;
        if (clen == 2 && comp[0] == '.' && comp[1] == '.') {
            Node n{};
            auto r = node_read(disk_, cur, n);
            if (!r.is_ok()) return r.error();
            cur = n.parent ? n.parent : header_.root_sector;
            continue;
        }
        auto r = dir_lookup(cur, comp, clen);
        if (!r.is_ok()) return r.error();
        cur = r.value();
    }
    return cur;
}

/* ========================================================================== */
/* Create / unlink / mkdir / rmdir                                            */
/* ========================================================================== */

Result<void> FileSystem::create(const char *path, std::uint16_t mode,
                                 std::uint32_t uid, std::uint32_t gid) noexcept
{
    if (!mounted_) return Error::NotSupported;

    /* Split into parent + basename. */
    const char *last = path;
    for (const char *p = path; *p; ++p) if (is_path_sep(*p)) last = p + 1;
    std::uint32_t base_len = 0;
    while (last[base_len] && base_len < MAX_NAME_LEN) ++base_len;
    if (base_len == 0) return Error::Invalid;

    /* Look up parent. */
    char parent_path[MAX_NAME_LEN * 8];
    if (last == path) {
        std::memcpy(parent_path, "/", 2);
    } else {
        std::size_t plen = static_cast<std::size_t>(last - path);
        if (plen >= sizeof(parent_path)) return Error::NameTooLong;
        std::memcpy(parent_path, path, plen);
        parent_path[plen] = 0;
    }
    auto pr = lookup(parent_path);
    if (!pr.is_ok()) return pr.error();
    std::uint64_t parent = pr.value();

    /* Already exists? */
    auto ex = dir_lookup(parent, last, base_len);
    if (ex.is_ok()) return Error::Exists;

    auto sr = alloc_sector();
    if (!sr.is_ok()) return Error::NoSpace;
    std::uint64_t sec = sr.value();

    Node n{};
    char name[MAX_NAME_LEN + 1];
    std::memcpy(name, last, base_len); name[base_len] = 0;
    node_init(n, MODE_FILE | (mode & 0777), uid, gid, name, parent);
    auto wr = node_write(disk_, sec, n);
    if (!wr.is_ok()) { free_sector(sec); return wr.error(); }

    auto lr = dir_link(parent, sec);
    if (!lr.is_ok()) { free_sector(sec); return lr.error(); }
    return {};
}

Result<void> FileSystem::mkdir(const char *path, std::uint16_t mode,
                                std::uint32_t uid, std::uint32_t gid) noexcept
{
    if (!mounted_) return Error::NotSupported;

    const char *last = path;
    for (const char *p = path; *p; ++p) if (is_path_sep(*p)) last = p + 1;
    std::uint32_t base_len = 0;
    while (last[base_len] && base_len < MAX_NAME_LEN) ++base_len;
    if (base_len == 0) return Error::Invalid;

    char parent_path[MAX_NAME_LEN * 8];
    if (last == path) std::memcpy(parent_path, "/", 2);
    else {
        std::size_t plen = static_cast<std::size_t>(last - path);
        if (plen >= sizeof(parent_path)) return Error::NameTooLong;
        std::memcpy(parent_path, path, plen);
        parent_path[plen] = 0;
    }
    auto pr = lookup(parent_path);
    if (!pr.is_ok()) return pr.error();
    std::uint64_t parent = pr.value();

    auto ex = dir_lookup(parent, last, base_len);
    if (ex.is_ok()) return Error::Exists;

    auto sr = alloc_sector();
    if (!sr.is_ok()) return Error::NoSpace;
    std::uint64_t sec = sr.value();

    Node n{};
    char name[MAX_NAME_LEN + 1];
    std::memcpy(name, last, base_len); name[base_len] = 0;
    node_init(n, MODE_DIR | (mode & 0777), uid, gid, name, parent);
    n.links = 2;
    auto wr = node_write(disk_, sec, n);
    if (!wr.is_ok()) { free_sector(sec); return wr.error(); }

    auto lr = dir_link(parent, sec);
    if (!lr.is_ok()) { free_sector(sec); return lr.error(); }
    return {};
}

Result<void> FileSystem::unlink(const char *path) noexcept {
    if (!mounted_) return Error::NotSupported;

    auto r = lookup(path);
    if (!r.is_ok()) return r.error();
    std::uint64_t sec = r.value();

    Node n{};
    auto nr = node_read(disk_, sec, n);
    if (!nr.is_ok()) return nr.error();
    if (n.is_dir()) return Error::IsDirectory;
    if (n.links > 1) {
        n.links--;
        return node_write(disk_, sec, n).is_ok()
            ? Result<void>{} : Result<void>{Error::Io};
    }

    /* Remove from parent. */
    auto ur = dir_unlink(n.parent, sec);
    if (!ur.is_ok()) return ur.error();

    free_node_data(n);
    free_sector(sec);
    return {};
}

Result<void> FileSystem::rmdir(const char *path) noexcept {
    if (!mounted_) return Error::NotSupported;

    auto r = lookup(path);
    if (!r.is_ok()) return r.error();
    std::uint64_t sec = r.value();

    Node n{};
    auto nr = node_read(disk_, sec, n);
    if (!nr.is_ok()) return nr.error();
    if (!n.is_dir()) return Error::NotDirectory;
    if (n.next != 0) return Error::NotEmpty;

    auto ur = dir_unlink(n.parent, sec);
    if (!ur.is_ok()) return ur.error();

    free_node_data(n);
    free_sector(sec);
    return {};
}

/* ========================================================================== */
/* File I/O                                                                   */
/* ========================================================================== */

FileHandle *FileSystem::find_fd(int fd) noexcept {
    if (fd < 0 || fd >= static_cast<int>(MAX_OPEN_FILES)) return nullptr;
    return fds_[fd].used ? &fds_[fd] : nullptr;
}

Result<int> FileSystem::open(const char *path, std::uint32_t flags) noexcept {
    if (!mounted_) return Error::NotSupported;

    auto r = lookup(path);
    if (!r.is_ok()) {
        if (r.error() == Error::NotFound && (flags & O_CREAT))
            {
                auto cr = create(path, 0644);
                if (!cr.is_ok()) return cr.error();
                r = lookup(path);
                if (!r.is_ok()) return r.error();
            }
        else
            return r.error();
    }

    std::uint64_t sec = r.value();

    int fd = -1;
    for (int i = 0; i < static_cast<int>(MAX_OPEN_FILES); ++i) {
        if (!fds_[i].used) { fd = i; break; }
    }
    if (fd < 0) return Error::TooManyFiles;

    fds_[fd].node_sector = sec;
    fds_[fd].offset      = 0;
    fds_[fd].flags       = flags;
    fds_[fd].used        = true;

    if (flags & O_TRUNC) {
        Node n{};
        auto nr = node_read(disk_, sec, n);
        if (nr.is_ok() && n.is_file()) {
            free_node_data(n);
            n.size = 0;
            (void)node_write(disk_, sec, n);
        }
    }
    return fd;
}

Result<std::size_t> FileSystem::node_read_data(const Node &n,
                                                 std::uint64_t offset,
                                                 void *buf,
                                                 std::size_t count) noexcept
{
    if (offset >= n.size) return std::size_t{0};
    if (offset + count > n.size) count = n.size - offset;

    std::uint8_t *out = static_cast<std::uint8_t *>(buf);
    std::uint64_t file_off = 0;
    std::size_t   done = 0;

    /* Walk extents; skip those before `offset`, copy from those overlapping. */
    for (std::uint32_t i = 0; i < INLINE_EXTENTS; ++i) {
        const Extent &e = n.extents[i];
        if (e.is_empty()) continue;

        std::uint64_t ext_start = file_off;
        std::uint64_t ext_end   = file_off + e.length * SECTOR_SIZE;
        file_off = ext_end;

        if (ext_end <= offset) continue;
        if (ext_start >= offset + count) break;

        std::uint64_t copy_start = offset > ext_start ? offset : ext_start;
        std::uint64_t copy_end   = (offset + count) < ext_end
                                 ? (offset + count) : ext_end;
        std::uint64_t copy_len   = copy_end - copy_start;
        if (copy_len == 0) continue;

        std::uint64_t sector_off = (copy_start - ext_start) / SECTOR_SIZE;
        std::uint64_t byte_off   = (copy_start - ext_start) % SECTOR_SIZE;
        std::uint64_t sector    = e.block + sector_off;

        /* Read one sector at a time for simplicity. */
        while (copy_len > 0) {
            std::uint8_t sector_buf[SECTOR_SIZE];
            auto rr = disk_.read_at(sector, sector_buf, SECTOR_SIZE);
            if (!rr.is_ok()) return Error::Io;
            std::uint64_t avail = SECTOR_SIZE - byte_off;
            std::uint64_t take  = copy_len < avail ? copy_len : avail;
            std::memcpy(out + done, sector_buf + byte_off, take);
            done      += take;
            copy_len  -= take;
            byte_off   = 0;
            ++sector;
        }
    }
    return done;
}

Result<std::size_t> FileSystem::node_write_data(Node &n,
                                                  std::uint64_t offset,
                                                  const void *buf,
                                                  std::size_t count) noexcept
{
    /* Simplified: extend the node by allocating a single extent for the
     * whole write. Production would merge with adjacent extents. */
    if (count == 0) return std::size_t{0};

    std::uint64_t start_sector = 0;
    std::uint64_t sectors      = (offset + count + SECTOR_SIZE - 1) / SECTOR_SIZE;
    std::uint64_t old_sectors  = (n.size + SECTOR_SIZE - 1) / SECTOR_SIZE;

    if (sectors > old_sectors) {
        /* Allocate a new extent for the tail. */
        std::uint64_t extra = sectors - old_sectors;
        auto r = alloc_extent(extra);
        if (!r.is_ok()) return Error::NoSpace;
        start_sector = r.value();

        /* Find the first empty inline extent, or allocate a chain block. */
        bool stored = false;
        for (auto &e : n.extents) {
            if (e.is_empty()) {
                e.block  = start_sector;
                e.length = extra;
                stored = true;
                break;
            }
        }
        if (!stored) {
            if (n.extents_next == 0) {
                auto cb = alloc_sector();
                if (!cb.is_ok()) { alloc_.free_sectors(start_sector, extra); return Error::NoSpace; }
                n.extents_next = cb.value();
                ExtentBlock eb{};
                auto wr = disk_.write_at(n.extents_next, &eb, sizeof(eb));
                if (!wr.is_ok()) return Error::Io;
            }
            ExtentBlock eb{};
            auto rr = disk_.read_at(n.extents_next, &eb, sizeof(eb));
            if (!rr.is_ok()) return Error::Io;
            for (auto &e : eb.extents) {
                if (e.is_empty()) { e.block = start_sector; e.length = extra; stored = true; break; }
            }
            if (!stored) return Error::NoSpace;
            auto wr = disk_.write_at(n.extents_next, &eb, sizeof(eb));
            if (!wr.is_ok()) return Error::Io;
        }
    }

    /* Write data via the extent chain. */
    const std::uint8_t *in = static_cast<const std::uint8_t *>(buf);
    std::size_t done = 0;
    std::uint64_t file_off = 0;
    std::uint64_t target_off = offset;

    /* Simplistic forward-write: assumes offsets are monotonically increasing
     * and extents are ordered. Production code would use a full extent walk. */
    auto write_via_extents = [&](const Extent *exts, std::uint32_t n_ext) -> bool {
        for (std::uint32_t i = 0; i < n_ext; ++i) {
            const Extent &e = exts[i];
            if (e.is_empty()) continue;
            std::uint64_t ext_start = file_off;
            std::uint64_t ext_end   = file_off + e.length * SECTOR_SIZE;
            file_off = ext_end;

            if (ext_end <= target_off) continue;
            if (ext_start >= target_off + count) return true;

            std::uint64_t wstart = target_off > ext_start ? target_off : ext_start;
            std::uint64_t wend   = (target_off + count) < ext_end
                                 ? (target_off + count) : ext_end;
            std::uint64_t wlen   = wend - wstart;
            if (wlen == 0) continue;

            std::uint64_t sector_off = (wstart - ext_start) / SECTOR_SIZE;
            std::uint64_t byte_off   = (wstart - ext_start) % SECTOR_SIZE;
            std::uint64_t sector    = e.block + sector_off;

            while (wlen > 0) {
                std::uint8_t sector_buf[SECTOR_SIZE];
                if (byte_off > 0 || wlen < SECTOR_SIZE - byte_off) {
                    auto rr = disk_.read_at(sector, sector_buf, SECTOR_SIZE);
                    if (!rr.is_ok()) return false;
                } else {
                    std::memset(sector_buf, 0, SECTOR_SIZE);
                }
                std::uint64_t avail = SECTOR_SIZE - byte_off;
                std::uint64_t take  = wlen < avail ? wlen : avail;
                std::memcpy(sector_buf + byte_off, in + done, take);
                auto wr = disk_.write_at(sector, sector_buf, SECTOR_SIZE);
                if (!wr.is_ok()) return false;
                done      += take;
                wlen      -= take;
                byte_off   = 0;
                ++sector;
            }
        }
        return true;
    };

    if (!write_via_extents(n.extents, INLINE_EXTENTS)) return Error::Io;
    if (n.extents_next) {
        ExtentBlock eb{};
        auto rr = disk_.read_at(n.extents_next, &eb, sizeof(eb));
        if (!rr.is_ok()) return Error::Io;
        if (!write_via_extents(eb.extents, 32)) return Error::Io;
    }

    if (offset + done > n.size) n.size = offset + done;
    return done;
}

Result<std::size_t> FileSystem::read(int fd, void *buf,
                                       std::size_t count) noexcept
{
    FileHandle *h = find_fd(fd);
    if (!h) return Error::Invalid;

    Node n{};
    auto nr = node_read(disk_, h->node_sector, n);
    if (!nr.is_ok()) return nr.error();
    if (!n.is_file()) return Error::IsDirectory;

    auto r = node_read_data(n, h->offset, buf, count);
    if (r.is_ok()) h->offset += r.value();
    return r;
}

Result<std::size_t> FileSystem::write(int fd, const void *buf,
                                        std::size_t count) noexcept
{
    FileHandle *h = find_fd(fd);
    if (!h) return Error::Invalid;
    if (h->flags & O_RDONLY) return Error::Permission;

    Node n{};
    auto nr = node_read(disk_, h->node_sector, n);
    if (!nr.is_ok()) return nr.error();
    if (!n.is_file()) return Error::IsDirectory;

    std::uint64_t offset = (h->flags & O_APPEND) ? n.size : h->offset;

    auto r = node_write_data(n, offset, buf, count);
    if (!r.is_ok()) return r;

    auto wr = node_write(disk_, h->node_sector, n);
    if (!wr.is_ok()) return Error::Io;

    h->offset = offset + r.value();
    return r;
}

Result<std::uint64_t> FileSystem::seek(int fd, std::int64_t offset,
                                         int whence) noexcept
{
    FileHandle *h = find_fd(fd);
    if (!h) return Error::Invalid;

    Node n{};
    auto nr = node_read(disk_, h->node_sector, n);
    if (!nr.is_ok()) return nr.error();

    std::int64_t base = 0;
    switch (whence) {
    case SEEK_SET: base = 0; break;
    case SEEK_CUR: base = static_cast<std::int64_t>(h->offset); break;
    case SEEK_END: base = static_cast<std::int64_t>(n.size); break;
    default: return Error::Invalid;
    }
    std::int64_t target = base + offset;
    if (target < 0) return Error::Invalid;
    h->offset = static_cast<std::uint64_t>(target);
    return h->offset;
}

Result<void> FileSystem::close(int fd) noexcept {
    FileHandle *h = find_fd(fd);
    if (!h) return Error::Invalid;
    std::memset(h, 0, sizeof(*h));
    return {};
}

/* ========================================================================== */
/* Metadata                                                                   */
/* ========================================================================== */

Result<Stat> FileSystem::stat(const char *path) noexcept {
    auto r = lookup(path);
    if (!r.is_ok()) return r.error();
    std::uint64_t sec = r.value();

    Node n{};
    auto nr = node_read(disk_, sec, n);
    if (!nr.is_ok()) return nr.error();

    Stat st{};
    st.mode   = n.mode;
    st.links  = n.links;
    st.uid    = n.uid;
    st.gid    = n.gid;
    st.size   = n.size;
    st.ino    = sec;
    st.blocks = node_allocated_bytes(n) / SECTOR_SIZE;
    return st;
}

Result<DirEntry> FileSystem::readdir(std::uint64_t dir_sector,
                                       std::uint32_t index) noexcept
{
    Node dir{};
    auto r = node_read(disk_, dir_sector, dir);
    if (!r.is_ok()) return r.error();
    if (!dir.is_dir()) return Error::NotDirectory;

    /* Walk `next` chain `index` times. */
    std::uint64_t cur = dir.next;
    for (std::uint32_t i = 0; i < index && cur != 0; ++i) {
        Node c{};
        auto cr = node_read(disk_, cur, c);
        if (!cr.is_ok()) return Error::Corrupt;
        cur = c.next;
    }
    if (cur == 0) return Error::NotFound;

    Node c{};
    auto cr = node_read(disk_, cur, c);
    if (!cr.is_ok()) return cr.error();

    DirEntry d{};
    d.ino = cur;
    d.mode = c.mode;
    std::uint32_t len = 0;
    while (len < MAX_NAME_LEN && c.name[len]) ++len;
    d.name_len = static_cast<std::uint8_t>(len);
    std::memcpy(d.name, c.name, len);
    d.name[len] = 0;
    return d;
}

Result<void> FileSystem::truncate(const char *path,
                                    std::uint64_t new_size) noexcept
{
    auto r = lookup(path);
    if (!r.is_ok()) return r.error();
    std::uint64_t sec = r.value();

    Node n{};
    auto nr = node_read(disk_, sec, n);
    if (!nr.is_ok()) return nr.error();
    if (!n.is_file()) return Error::IsDirectory;

    if (new_size == 0) {
        free_node_data(n);
        n.size = 0;
        return node_write(disk_, sec, n).is_ok()
            ? Result<void>{} : Result<void>{Error::Io};
    }
    /* Non-zero truncate not implemented in v1 (needs extent splitting). */
    if (new_size == n.size) return {};
    return Error::NotSupported;
}

/* ========================================================================== */
/* Convenience                                                                */
/* ========================================================================== */

Result<void> format(Disk &disk, const std::uint8_t uuid[16]) noexcept {
    FileSystem fs(disk);
    return fs.format(uuid);
}

} // namespace fm512
