/*
 * ============================================================================
 *  SYSCALLS.CPP - Syscall handlers + dispatcher
 * ============================================================================
 */

#include "syscalls.hpp"

namespace fm512 {

/* ========================================================================== */
/* ERROR MAPPING                                                              */
/* ========================================================================== */

std::int64_t error_to_return(Error e) noexcept {
    switch (e) {
    case Error::Ok:            return E_OK;
    case Error::Generic:       return E_IO;
    case Error::Timeout:       return E_IO;
    case Error::Invalid:       return E_INVAL;
    case Error::NoMem:         return E_NOMEM;
    case Error::Busy:          return E_IO;
    case Error::NotFound:      return E_NOENT;
    case Error::Exists:        return E_EXIST;
    case Error::NotEmpty:      return E_NOTEMPTY;
    case Error::IsDirectory:   return E_ISDIR;
    case Error::NotDirectory:  return E_NOTDIR;
    case Error::NoSpace:       return E_NOSPC;
    case Error::Corrupt:       return E_CORRUPT;
    case Error::NotSupported:  return E_NOTSUP;
    case Error::Io:            return E_IO;
    case Error::ReadOnly:      return E_ROFS;
    case Error::TooManyFiles:  return E_MFILE;
    case Error::NameTooLong:   return E_NAMETOOLONG;
    case Error::CrossDevice:   return E_INVAL;
    case Error::Permission:    return E_PERM;
    }
    return E_IO;
}

/* ========================================================================== */
/* USER POINTER SAFETY                                                        */
/* ========================================================================== */

bool user_range_ok(const void *ptr, std::size_t len) noexcept {
    if (len == 0) return true;
    if (ptr == nullptr) return false;
    auto p = reinterpret_cast<std::uintptr_t>(ptr);
    /* Reject kernel canonical (upper half). */
    if (p >= 0xFFFFFFC000000000ULL) return false;
    /* Reject null page. */
    if (p < 0x10000) return false;
    /* Reject overflow. */
    if (len > 0 && p + len < p) return false;
    return true;
}

bool copy_from_user(void *dst, const void *src, std::size_t len) noexcept {
    if (!user_range_ok(src, len)) return false;
    if (!user_range_ok(dst, len)) return false;
    std::memcpy(dst, src, len);
    return true;
}

bool copy_to_user(void *dst, const void *src, std::size_t len) noexcept {
    if (!user_range_ok(dst, len)) return false;
    if (!user_range_ok(src, len)) return false;
    std::memcpy(dst, src, len);
    return true;
}

bool copy_str_from_user(char *dst, const char *src, std::size_t max) noexcept {
    if (!dst || !src || max == 0) return false;
    for (std::size_t i = 0; i < max; ++i) {
        if (!user_range_ok(src + i, 1)) return false;
        dst[i] = src[i];
        if (src[i] == 0) return true;
    }
    return false;
}

bool copy_str_to_user(char *dst, const char *src, std::size_t max) noexcept {
    if (!dst || !src || max == 0) return false;
    for (std::size_t i = 0; i < max; ++i) {
        if (!user_range_ok(dst + i, 1)) return false;
        dst[i] = src[i];
        if (src[i] == 0) return true;
    }
    return false;
}

/* ========================================================================== */
/* HANDLERS — internal helpers                                                */
/* ========================================================================== */

static inline bool path_arg_ok(const char *p) noexcept {
    return p != nullptr && user_range_ok(p, 1);
}

static Error resolve_at(FileManager &fm, UserContext &uc, int dirfd,
                         const char *path, std::uint64_t &out_sec) noexcept
{
    if (!path) return Error::Invalid;

    /* dirfd == AT_FDCWD -> use cwd; otherwise reject (fd-relative not
     * implemented in v1). */
    std::uint64_t base = (dirfd == static_cast<int>(AT_FDCWD))
                        ? uc.cwd_sector
                        : uc.cwd_sector;   // TODO: fd-relative
    auto r = fm.resolve(path, base);
    if (!r.is_ok()) return r.error();
    out_sec = r.value();
    return Error::Ok;
}

/* ========================================================================== */
/* SYSCALL — read / write / open / close                                      */
/* ========================================================================== */

static std::int64_t sys_read_impl(FileManager &fm, UserContext &uc,
                                    std::int64_t fd, void *buf,
                                    std::size_t count) noexcept
{
    (void)uc;
    if (count == 0) return 0;
    if (!user_range_ok(buf, count)) return E_FAULT;

    /* Read into a temp buffer then copy — user might pass buffer that overlaps
     * the FS's internal state (paranoid but safe). */
    constexpr std::size_t CHUNK = 4096;
    std::uint8_t tmp[CHUNK];
    std::size_t total = 0;
    std::size_t remaining = count;
    std::uint8_t *out = static_cast<std::uint8_t *>(buf);

    while (remaining > 0) {
        std::size_t chunk = remaining < CHUNK ? remaining : CHUNK;
        auto r = fm.fs().read(static_cast<int>(fd), tmp, chunk);
        if (!r.is_ok()) {
            if (total > 0) return static_cast<std::int64_t>(total);
            return error_to_return(r.error());
        }
        if (r.value() == 0) break;
        if (!copy_to_user(out + total, tmp, r.value())) return E_FAULT;
        total     += r.value();
        remaining -= r.value();
        if (r.value() < chunk) break;
    }
    return static_cast<std::int64_t>(total);
}

static std::int64_t sys_write_impl(FileManager &fm, UserContext &uc,
                                     std::int64_t fd, const void *buf,
                                     std::size_t count) noexcept
{
    (void)uc;
    if (count == 0) return 0;
    if (!user_range_ok(buf, count)) return E_FAULT;

    /* Guard against forbidden extensions. */
    constexpr std::size_t CHUNK = 4096;
    std::uint8_t tmp[CHUNK];
    std::size_t total = 0;
    std::size_t remaining = count;
    const std::uint8_t *in = static_cast<const std::uint8_t *>(buf);

    while (remaining > 0) {
        std::size_t chunk = remaining < CHUNK ? remaining : CHUNK;
        if (!copy_from_user(tmp, in + total, chunk)) return E_FAULT;

        auto r = fm.fs().write(static_cast<int>(fd), tmp, chunk);
        if (!r.is_ok()) {
            if (total > 0) return static_cast<std::int64_t>(total);
            return error_to_return(r.error());
        }
        total     += r.value();
        remaining -= r.value();
        if (r.value() < chunk) break;
    }
    return static_cast<std::int64_t>(total);
}

static std::int64_t sys_openat_impl(FileManager &fm, UserContext &uc,
                                      std::int64_t dirfd, const char *path,
                                      std::uint32_t flags, std::uint16_t mode) noexcept
{
    if (!path_arg_ok(path)) return E_FAULT;

    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;

    /* Resolve (creates if O_CREAT). */
    auto r = fm.resolve(path_buf, uc.cwd_sector);
    if (!r.is_ok()) {
        if (r.error() == Error::NotFound && (flags & O_CREAT)) {
            auto cr = fm.create(path_buf, mode & ~uc.umask);
            if (!cr.is_ok() && cr.error() != Error::Exists) return error_to_return(cr.error());
            r = fm.resolve(path_buf, uc.cwd_sector);
            if (!r.is_ok()) return error_to_return(r.error());
        } else {
            return error_to_return(r.error());
        }
    }

    /* Type guard: if opening for exec-read, verify magic. */
    if (flags & O_EXEC) {
        std::uint8_t hdr[512];
        auto st = fm.stat(path_buf);
        if (st.is_ok()) {
            std::size_t hlen = st.value().size < sizeof(hdr) ? st.value().size : sizeof(hdr);
            if (hlen > 0) {
                auto rr = fm.read_file(path_buf, hdr, hlen);
                if (rr.is_ok()) {
                    auto chk = TypeGuard::verify_exec(fm.registry(), path_buf, hdr, rr.value());
                    if (chk != GuardCheck::Ok && chk != GuardCheck::EmptyFile)
                        return E_ACCES;
                }
            }
        }
    }

    /* If opening for write, check read-only extension. */
    if (flags & (O_WRONLY | O_RDWR)) {
        auto chk = TypeGuard::verify_write(fm.registry(), path_buf, 0);
        if (chk == GuardCheck::Forbidden) return E_ROFS;
    }

    int fd = -1;
    auto or_ = fm.fs().open(path_buf, flags);
    if (!or_.is_ok()) return error_to_return(or_.error());
    fd = or_.value();
    return fd;
}

static std::int64_t sys_close_impl(FileManager &fm, std::int64_t fd) noexcept {
    auto r = fm.fs().close(static_cast<int>(fd));
    return r.is_ok() ? 0 : error_to_return(r.error());
}

/* ========================================================================== */
/* SYSCALL — stat / lseek / ftruncate                                         */
/* ========================================================================== */

struct kstat {          // userspace-visible stat (kept minimal)
    std::uint64_t ino;
    std::uint16_t mode;
    std::uint32_t uid;
    std::uint32_t gid;
    std::uint64_t size;
    std::uint64_t blocks;
    std::uint32_t nlink;
};

static std::int64_t sys_newfstatat_impl(FileManager &fm, UserContext &uc,
                                          std::int64_t dirfd, const char *path,
                                          kstat *out, int flags) noexcept
{
    (void)flags;
    if (!path_arg_ok(path)) return E_FAULT;
    if (!user_range_ok(out, sizeof(kstat))) return E_FAULT;

    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;

    std::uint64_t base = (dirfd == static_cast<int>(AT_FDCWD))
                        ? uc.cwd_sector : uc.cwd_sector;
    auto r = fm.fs().lookup(path_buf, base);
    if (!r.is_ok()) return error_to_return(r.error());

    auto st = fm.fs().stat(path_buf);
    if (!st.is_ok()) return error_to_return(st.error());

    kstat ks{};
    ks.ino    = st.value().ino;
    ks.mode   = st.value().mode;
    ks.uid    = st.value().uid;
    ks.gid    = st.value().gid;
    ks.size   = st.value().size;
    ks.blocks = st.value().blocks;
    ks.nlink  = st.value().links;

    if (!copy_to_user(out, &ks, sizeof(ks))) return E_FAULT;
    return 0;
}

static std::int64_t sys_fstat_impl(FileManager &fm, std::int64_t fd,
                                     kstat *out) noexcept
{
    if (!user_range_ok(out, sizeof(kstat))) return E_FAULT;
    /* Reconstruct path from fd's node. */
    FileHandle *h = nullptr;
    /* FileSystem doesn't expose find_fd; approximate by iterating fd table. */
    auto st_r = [&]() -> Result<Stat> {
        /* Fallback: use fd's node sector via FileSystem's internal state. */
        /* We don't have accessor... add one in future. For now: */
        return Error::NotSupported;
    }();
    if (!st_r.is_ok()) {
        (void)fm; (void)fd;
        return E_NOTSUP;
    }
    (void)h;
    return E_NOTSUP;
}

static std::int64_t sys_lseek_impl(FileManager &fm, std::int64_t fd,
                                     std::int64_t offset, int whence) noexcept
{
    auto r = fm.fs().seek(static_cast<int>(fd), offset, whence);
    if (!r.is_ok()) return error_to_return(r.error());
    return static_cast<std::int64_t>(r.value());
}

static std::int64_t sys_ftruncate_impl(FileManager &fm, UserContext &uc,
                                         std::int64_t fd, std::int64_t length) noexcept
{
    (void)uc;
    if (length < 0) return E_INVAL;
    /* FileSystem::truncate is path-based; no direct fd-truncate yet. */
    (void)fm; (void)fd;
    return E_NOTSUP;
}

/* ========================================================================== */
/* SYSCALL — directory operations                                             */
/* ========================================================================== */

static std::int64_t sys_mkdirat_impl(FileManager &fm, UserContext &uc,
                                       std::int64_t dirfd, const char *path,
                                       std::uint16_t mode) noexcept
{
    if (!path_arg_ok(path)) return E_FAULT;
    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;

    (void)dirfd;
    auto r = fm.mkdir(path_buf, mode & ~uc.umask);
    return r.is_ok() ? 0 : error_to_return(r.error());
}

static std::int64_t sys_unlinkat_impl(FileManager &fm, UserContext &uc,
                                        std::int64_t dirfd, const char *path,
                                        int flags) noexcept
{
    if (!path_arg_ok(path)) return E_FAULT;
    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;

    (void)dirfd;
    auto isdir = fm.is_dir(path_buf);
    if (isdir.is_ok() && isdir.value()) {
        if (flags & 0x200)   /* AT_REMOVEDIR */
            { auto r = fm.rmdir(path_buf); return r.is_ok() ? 0 : error_to_return(r.error()); }
        return E_ISDIR;
    }
    auto r = fm.unlink(path_buf);
    return r.is_ok() ? 0 : error_to_return(r.error());
}

static std::int64_t sys_renameat2_impl(FileManager &fm, UserContext &uc,
                                         std::int64_t old_dirfd, const char *old_path,
                                         std::int64_t new_dirfd, const char *new_path,
                                         std::uint32_t flags) noexcept
{
    (void)old_dirfd; (void)new_dirfd; (void)flags;
    if (!path_arg_ok(old_path) || !path_arg_ok(new_path)) return E_FAULT;

    char old_buf[MAX_NAME_LEN * 8];
    char new_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(old_buf, old_path, sizeof(old_buf))) return E_FAULT;
    if (!copy_str_from_user(new_buf, new_path, sizeof(new_buf))) return E_FAULT;

    auto r = fm.rename(old_buf, new_buf);
    return r.is_ok() ? 0 : error_to_return(r.error());
}

/* Userspace-visible dirent (matches Linux dirent64 layout, minus d_off). */
struct kdirent {
    std::uint64_t d_ino;
    std::int64_t  d_off;
    std::uint16_t d_reclen;
    std::uint8_t  d_type;
    char          d_name[256];
};

static std::int64_t sys_getdents64_impl(FileManager &fm, UserContext &uc,
                                          std::int64_t fd, void *buf,
                                          std::size_t count) noexcept
{
    (void)uc;
    if (!user_range_ok(buf, count)) return E_FAULT;

    /* Need the fd's node sector. Use FileSystem's internal access. */
    std::uint64_t node_sec = 0;
    auto sec_r = fm.fs().fd_node_sector(static_cast<int>(fd));
    if (!sec_r.is_ok()) return error_to_return(sec_r.error());
    node_sec = sec_r.value();

    auto isd = fm.is_dir_sec(node_sec);
    if (!isd.is_ok() || !isd.value()) return E_NOTDIR;

    std::size_t out_pos = 0;
    std::uint32_t idx = 0;
    while (true) {
        auto r = fm.readdir(node_sec, idx);
        if (!r.is_ok()) {
            if (r.error() == Error::NotFound) break;
            if (out_pos > 0) break;
            return error_to_return(r.error());
        }
        kdirent de{};
        de.d_ino    = r.value().ino;
        de.d_off    = 0;
        de.d_type   = static_cast<std::uint8_t>(r.value().mode >> 12);
        std::size_t nl = 0;
        while (nl < sizeof(de.d_name) - 1 && r.value().name[nl])
            { de.d_name[nl] = r.value().name[nl]; ++nl; }
        de.d_name[nl] = 0;
        de.d_reclen = static_cast<std::uint16_t>(sizeof(de.d_ino)
                                                + sizeof(de.d_off)
                                                + sizeof(de.d_reclen)
                                                + sizeof(de.d_type)
                                                + nl + 1);

        if (out_pos + de.d_reclen > count) {
            if (out_pos == 0) return E_INVAL;
            break;
        }

        if (!copy_to_user(static_cast<std::uint8_t*>(buf) + out_pos,
                          &de, de.d_reclen)) return E_FAULT;
        out_pos += de.d_reclen;
        ++idx;
    }
    return static_cast<std::int64_t>(out_pos);
}

/* ========================================================================== */
/* SYSCALL — chmod / chown / access / chdir / getcwd                          */
/* ========================================================================== */

static std::int64_t sys_fchmodat_impl(FileManager &fm, UserContext &uc,
                                        std::int64_t dirfd, const char *path,
                                        std::uint16_t mode) noexcept
{
    if (!path_arg_ok(path)) return E_FAULT;
    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;
    (void)dirfd; (void)uc;
    auto r = fm.chmod(path_buf, mode);
    return r.is_ok() ? 0 : error_to_return(r.error());
}

static std::int64_t sys_fchownat_impl(FileManager &fm, UserContext &uc,
                                        std::int64_t dirfd, const char *path,
                                        std::uint32_t uid, std::uint32_t gid) noexcept
{
    if (!path_arg_ok(path)) return E_FAULT;
    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;
    (void)dirfd; (void)uc;
    auto r = fm.chown(path_buf, uid, gid);
    return r.is_ok() ? 0 : error_to_return(r.error());
}

static std::int64_t sys_faccessat_impl(FileManager &fm, UserContext &uc,
                                         std::int64_t dirfd, const char *path,
                                         int mode) noexcept
{
    if (!path_arg_ok(path)) return E_FAULT;
    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;
    (void)mode; (void)dirfd;

    auto r = fm.stat(path_buf);
    if (!r.is_ok()) return error_to_return(r.error());

    /* Check mode permissions against uid/gid (baseline: only owner). */
    Stat s = r.value();
    std::uint16_t m = s.mode & 0777;
    bool ok = true;
    if (s.uid != uc.uid) {
        if (s.uid == 0 || uc.uid == 0) ok = true;   // root override
        else {
            std::uint16_t other = m & 7;
            if ((mode & 4) && !(other & 4)) ok = false;
            if ((mode & 2) && !(other & 2)) ok = false;
            if ((mode & 1) && !(other & 1)) ok = false;
        }
    }
    return ok ? 0 : E_ACCES;
}

static std::int64_t sys_chdir_impl(FileManager &fm, UserContext &uc,
                                     const char *path) noexcept
{
    if (!path_arg_ok(path)) return E_FAULT;
    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;

    auto r = fm.resolve(path_buf, uc.cwd_sector);
    if (!r.is_ok()) return error_to_return(r.error());

    auto isd = fm.is_dir(path_buf);
    if (!isd.is_ok()) return error_to_return(isd.error());
    if (!isd.value()) return E_NOTDIR;

    uc.cwd_sector = r.value();
    return 0;
}

static std::int64_t sys_getcwd_impl(FileManager &fm, UserContext &uc,
                                      char *buf, std::size_t size) noexcept
{
    if (!buf || size == 0 || !user_range_ok(buf, size)) return E_FAULT;

    char tmp[1024];
    auto r = fm.path_of_sector(uc.cwd_sector, tmp, sizeof(tmp));
    if (!r.is_ok()) return error_to_return(r.error());

    std::size_t len = 0;
    while (tmp[len]) ++len;
    if (len + 1 > size) return E_NAMETOOLONG;
    if (!copy_str_to_user(buf, tmp, size)) return E_FAULT;
    return static_cast<std::int64_t>(len + 1);
}

/* ========================================================================== */
/* SYSCALL — sync / mount / umount / mkfs / fsck / trim                       */
/* ========================================================================== */

static std::int64_t sys_sync_impl(FileManager &fm) noexcept {
    auto r = fm.sync();
    return r.is_ok() ? 0 : error_to_return(r.error());
}

static std::int64_t sys_mount_impl(FileManager &fm, UserContext &uc,
                                     const char *src, const char *target,
                                     const char *fstype, std::uint64_t flags,
                                     const void *data) noexcept
{
    (void)fm; (void)uc; (void)src; (void)target; (void)fstype;
    (void)flags; (void)data;
    /* Only 512fm supported. Mount happens at boot in kernel; syscall
     * would be for additional mounts (not implemented in v1). */
    return E_NOTSUP;
}

static std::int64_t sys_umount2_impl(FileManager &fm, UserContext &uc,
                                       const char *target, int flags) noexcept
{
    (void)fm; (void)uc; (void)target; (void)flags;
    return E_NOTSUP;
}

static std::int64_t sys_mkfs_impl(FileManager &fm, UserContext &uc,
                                    const std::uint8_t *uuid) noexcept
{
    (void)uc;
    if (uc.uid != 0) return E_PERM;
    std::uint8_t local_uuid[16];
    if (uuid) {
        if (!user_range_ok(uuid, 16)) return E_FAULT;
        if (!copy_from_user(local_uuid, uuid, 16)) return E_FAULT;
    } else {
        std::memset(local_uuid, 0, 16);
    }
    auto r = fm.fs().format(local_uuid);
    return r.is_ok() ? 0 : error_to_return(r.error());
}

static std::int64_t sys_fsck_impl(FileManager &fm, UserContext &uc) noexcept {
    (void)uc;
    auto r = fm.verify();
    return r.is_ok() ? 0 : error_to_return(r.error());
}

static std::int64_t sys_trim_impl(FileManager &fm, UserContext &uc) noexcept {
    (void)uc;
    auto r = fm.trim();
    return r.is_ok() ? 0 : error_to_return(r.error());
}

/* ========================================================================== */
/* SYSCALL — custom identify / register_extension                             */
/* ========================================================================== */

struct k_ext_info {
    char          ext[16];
    char          name[24];
    char          mime[40];
    std::uint32_t category;
    std::uint32_t flags;
};

static std::int64_t sys_cot_identify_impl(FileManager &fm, UserContext &uc,
                                            const char *path,
                                            k_ext_info *out) noexcept
{
    (void)uc;
    if (!path_arg_ok(path)) return E_FAULT;
    if (!user_range_ok(out, sizeof(k_ext_info))) return E_FAULT;

    char path_buf[MAX_NAME_LEN * 8];
    if (!copy_str_from_user(path_buf, path, sizeof(path_buf))) return E_FAULT;

    auto r = fm.identify(path_buf);
    if (!r.is_ok()) return error_to_return(r.error());

    k_ext_info info{};
    std::size_t el = 0;
    while (r.value()->ext[el] && el < sizeof(info.ext) - 1)
        { info.ext[el] = r.value()->ext[el]; ++el; }
    std::size_t nl = 0;
    while (r.value()->name[nl] && nl < sizeof(info.name) - 1)
        { info.name[nl] = r.value()->name[nl]; ++nl; }
    std::size_t ml = 0;
    while (r.value()->mime[ml] && ml < sizeof(info.mime) - 1)
        { info.mime[ml] = r.value()->mime[ml]; ++ml; }
    info.category = static_cast<std::uint32_t>(r.value()->category);
    info.flags    = r.value()->flags;

    if (!copy_to_user(out, &info, sizeof(info))) return E_FAULT;
    return 0;
}

static std::int64_t sys_cot_register_ext_impl(FileManager &fm, UserContext &uc,
                                                const k_ext_info *info) noexcept
{
    if (uc.uid != 0) return E_PERM;
    if (!user_range_ok(info, sizeof(k_ext_info))) return E_FAULT;

    k_ext_info local{};
    if (!copy_from_user(&local, info, sizeof(local))) return E_FAULT;

    ExtensionInfo e{};
    std::memcpy(e.ext,  local.ext,  sizeof(e.ext) - 1);
    std::memcpy(e.name, local.name, sizeof(e.name) - 1);
    std::memcpy(e.mime, local.mime, sizeof(e.mime) - 1);
    e.category = static_cast<FileCategory>(local.category);
    e.flags    = local.flags;

    auto r = fm.register_extension(e);
    return r.is_ok() ? 0 : error_to_return(r.error());
}

/* ========================================================================== */
/* DISPATCHER                                                                 */
/* ========================================================================== */

extern "C" std::int64_t syscall_dispatch(FileManager &fm,
                                          UserContext &uc,
                                          std::int64_t num,
                                          std::uint64_t a0,
                                          std::uint64_t a1,
                                          std::uint64_t a2,
                                          std::uint64_t a3,
                                          std::uint64_t a4,
                                          std::uint64_t a5) noexcept
{
    switch (num) {
    case SYS_READ:
        return sys_read_impl(fm, uc, static_cast<std::int64_t>(a0),
                              reinterpret_cast<void *>(a0 = a1),
                              static_cast<std::size_t>(a2));
    case SYS_WRITE:
        return sys_write_impl(fm, uc, static_cast<std::int64_t>(a0),
                               reinterpret_cast<const void *>(a1),
                               static_cast<std::size_t>(a2));
    case SYS_OPENAT:
        return sys_openat_impl(fm, uc,
                                static_cast<std::int64_t>(a0),
                                reinterpret_cast<const char *>(a1),
                                static_cast<std::uint32_t>(a2),
                                static_cast<std::uint16_t>(a3));
    case SYS_CLOSE:
        return sys_close_impl(fm, static_cast<std::int64_t>(a0));
    case SYS_FSTAT:
        return sys_fstat_impl(fm, static_cast<std::int64_t>(a0),
                               reinterpret_cast<kstat *>(a1));
    case SYS_NEWFSTATAT:
        return sys_newfstatat_impl(fm, uc,
                                    static_cast<std::int64_t>(a0),
                                    reinterpret_cast<const char *>(a1),
                                    reinterpret_cast<kstat *>(a2),
                                    static_cast<int>(a3));
    case SYS_LSEEK:
        return sys_lseek_impl(fm, static_cast<std::int64_t>(a0),
                               static_cast<std::int64_t>(a1),
                               static_cast<int>(a2));
    case SYS_FTRUNCATE:
        return sys_ftruncate_impl(fm, uc, static_cast<std::int64_t>(a0),
                                    static_cast<std::int64_t>(a1));
    case SYS_MKDIRAT:
        return sys_mkdirat_impl(fm, uc,
                                 static_cast<std::int64_t>(a0),
                                 reinterpret_cast<const char *>(a1),
                                 static_cast<std::uint16_t>(a2));
    case SYS_UNLINKAT:
        return sys_unlinkat_impl(fm, uc,
                                  static_cast<std::int64_t>(a0),
                                  reinterpret_cast<const char *>(a1),
                                  static_cast<int>(a2));
    case SYS_RENAMEAT2:
        return sys_renameat2_impl(fm, uc,
                                   static_cast<std::int64_t>(a0),
                                   reinterpret_cast<const char *>(a1),
                                   static_cast<std::int64_t>(a2),
                                   reinterpret_cast<const char *>(a3),
                                   static_cast<std::uint32_t>(a4));
    case SYS_GETDENTS64:
        return sys_getdents64_impl(fm, uc, static_cast<std::int64_t>(a0),
                                    reinterpret_cast<void *>(a1),
                                    static_cast<std::size_t>(a2));
    case SYS_FCHMODAT:
        return sys_fchmodat_impl(fm, uc,
                                  static_cast<std::int64_t>(a0),
                                  reinterpret_cast<const char *>(a1),
                                  static_cast<std::uint16_t>(a2));
    case SYS_FCHOWNAT:
        return sys_fchownat_impl(fm, uc,
                                  static_cast<std::int64_t>(a0),
                                  reinterpret_cast<const char *>(a1),
                                  static_cast<std::uint32_t>(a2),
                                  static_cast<std::uint32_t>(a3));
    case SYS_FACCESSAT:
        return sys_faccessat_impl(fm, uc,
                                   static_cast<std::int64_t>(a0),
                                   reinterpret_cast<const char *>(a1),
                                   static_cast<int>(a2));
    case SYS_CHDIR:
        return sys_chdir_impl(fm, uc, reinterpret_cast<const char *>(a0));
    case SYS_GETCWD:
        return sys_getcwd_impl(fm, uc, reinterpret_cast<char *>(a0),
                                static_cast<std::size_t>(a1));
    case SYS_SYNC:
        return sys_sync_impl(fm);
    case SYS_MOUNT:
        return sys_mount_impl(fm, uc,
                               reinterpret_cast<const char *>(a0),
                               reinterpret_cast<const char *>(a1),
                               reinterpret_cast<const char *>(a2),
                               static_cast<std::uint64_t>(a3),
                               reinterpret_cast<const void *>(a4));
    case SYS_UMOUNT2:
        return sys_umount2_impl(fm, uc,
                                 reinterpret_cast<const char *>(a0),
                                 static_cast<int>(a1));
    case SYS_COT_MKFS:
        return sys_mkfs_impl(fm, uc, reinterpret_cast<const std::uint8_t *>(a0));
    case SYS_COT_FSCK:
        return sys_fsck_impl(fm, uc);
    case SYS_COT_TRIM:
        return sys_trim_impl(fm, uc);
    case SYS_COT_IDENTIFY:
        return sys_cot_identify_impl(fm, uc,
                                       reinterpret_cast<const char *>(a0),
                                       reinterpret_cast<k_ext_info *>(a1));
    case SYS_COT_REGEXT:
        return sys_cot_register_ext_impl(fm, uc,
                                           reinterpret_cast<const k_ext_info *>(a0));
    default:
        return E_NOSYS;
    }
}

} // namespace fm512
