/*
 * ============================================================================
 *  SYSCALLS.HPP - Kernel-side syscall layer for 512fm
 *  CoTanOS · RISC-V · C++17
 *
 *  FS runs in kernel; userspace reaches it via these syscalls. All handlers
 *  return int64_t: >=0 on success (byte counts, fds, inodes), <0 errno-style
 *  on failure. User pointers are validated minimally — see comment on
 *  user_range_ok.
 * ============================================================================
 */

#ifndef FM512_SYSCALLS_HPP
#define FM512_SYSCALLS_HPP

#include "files.hpp"

namespace fm512 {

/* ========================================================================== */
/* SYSCALL NUMBERS (compatible with RV Linux subset, custom above 1000)       */
/* ========================================================================== */

constexpr std::int64_t SYS_READ          = 63;
constexpr std::int64_t SYS_WRITE         = 64;
constexpr std::int64_t SYS_OPENAT        = 56;
constexpr std::int64_t SYS_CLOSE         = 57;
constexpr std::int64_t SYS_FSTAT         = 80;
constexpr std::int64_t SYS_NEWFSTATAT    = 79;
constexpr std::int64_t SYS_LSEEK         = 62;
constexpr std::int64_t SYS_UNLINKAT      = 35;
constexpr std::int64_t SYS_MKDIRAT       = 34;
constexpr std::int64_t SYS_RENAMEAT2     = 276;
constexpr std::int64_t SYS_FCHMODAT      = 53;
constexpr std::int64_t SYS_FCHOWNAT      = 54;
constexpr std::int64_t SYS_GETDENTS64    = 61;
constexpr std::int64_t SYS_FTRUNCATE     = 46;
constexpr std::int64_t SYS_SYNC          = 81;
constexpr std::int64_t SYS_DUP           = 23;
constexpr std::int64_t SYS_DUP3          = 24;
constexpr std::int64_t SYS_FACCESSAT     = 48;
constexpr std::int64_t SYS_CHDIR         = 49;
constexpr std::int64_t SYS_GETCWD        = 17;
constexpr std::int64_t SYS_UMOUNT2       = 39;
constexpr std::int64_t SYS_MOUNT         = 40;

/* Custom CoTanOS syscalls (>= 1000) */
constexpr std::int64_t SYS_COT_IDENTIFY  = 1000;
constexpr std::int64_t SYS_COT_REGEXT    = 1001;
constexpr std::int64_t SYS_COT_MKFS      = 1002;
constexpr std::int64_t SYS_COT_FSCK      = 1003;
constexpr std::int64_t SYS_COT_TRIM      = 1004;
constexpr std::int64_t SYS_COT_SECTORMAP = 1005;

/* AT_* flags used by *at syscalls */
constexpr std::int64_t AT_FDCWD          = -100;
constexpr std::int64_t AT_SYMLINK_NOFOLLOW = 0x100;

/* ========================================================================== */
/* USER CONTEXT                                                               */
/* ========================================================================== */

struct UserContext {
    std::uint32_t uid     = 0;
    std::uint32_t gid     = 0;
    std::uint32_t umask   = 022;
    std::uint64_t cwd_sector = ROOT_SECTOR;
    std::uint32_t pid     = 0;
};

/* ========================================================================== */
/* ERROR RETURN MAPPING                                                       */
/* ========================================================================== */

/* errno values (subset of Linux, negative for direct return) */
constexpr std::int64_t E_OK     = 0;
constexpr std::int64_t E_PERM   = -1;
constexpr std::int64_t E_NOENT  = -2;
constexpr std::int64_t E_IO     = -5;
constexpr std::int64_t E_BADF   = -9;
constexpr std::int64_t E_NOMEM  = -12;
constexpr std::int64_t E_ACCES  = -13;
constexpr std::int64_t E_FAULT  = -14;
constexpr std::int64_t E_EXIST  = -17;
constexpr std::int64_t E_NOTDIR = -20;
constexpr std::int64_t E_ISDIR  = -21;
constexpr std::int64_t E_INVAL  = -22;
constexpr std::int64_t E_MFILE  = -24;
constexpr std::int64_t E_NOSPC  = -28;
constexpr std::int64_t E_ROFS   = -30;
constexpr std::int64_t E_NAMETOOLONG = -36;
constexpr std::int64_t E_NOSYS  = -38;
constexpr std::int64_t E_NOTEMPTY = -39;
constexpr std::int64_t E_OVERFLOW = -75;
constexpr std::int64_t E_NOTSUP = -95;
constexpr std::int64_t E_CORRUPT = -117;   // custom

std::int64_t error_to_return(Error e) noexcept;

/* ========================================================================== */
/* USER POINTER SAFETY                                                        */
/* ========================================================================== */

/*
 * On bare-metal RISC-V without MMU there is no real user/kernel isolation.
 * These helpers reject NULL, kernel-space canonical addresses, and obvious
 * overflow cases. Once MMU lands, replace with copy_from_user under page
 * faults.
 */
bool user_range_ok(const void *ptr, std::size_t len) noexcept;
bool copy_from_user(void *dst, const void *src, std::size_t len) noexcept;
bool copy_to_user(void *dst, const void *src, std::size_t len) noexcept;
bool copy_str_from_user(char *dst, const char *src, std::size_t max) noexcept;
bool copy_str_to_user(char *dst, const char *src, std::size_t max) noexcept;

/* ========================================================================== */
/* SYSCALL DISPATCHER                                                         */
/* ========================================================================== */

extern "C" std::int64_t syscall_dispatch(FileManager &fm,
                                          UserContext &uc,
                                          std::int64_t num,
                                          std::uint64_t a0,
                                          std::uint64_t a1,
                                          std::uint64_t a2,
                                          std::uint64_t a3,
                                          std::uint64_t a4,
                                          std::uint64_t a5) noexcept;

} // namespace fm512

#endif // FM512_SYSCALLS_HPP
