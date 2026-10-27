//! rt.zig — TrangorgeOS ring-3 runtime for freestanding Zig apps.
//!
//! Every syscall enters the kernel through `int 0x80`, the gate the kernel
//! installs at DPL 3 in `kernel/src/trampoline_rings/arch/x86_64/trampoline_rings.rs`.
//! Register mapping (same as `ustd/hal/src/gate.rs`):
//!   rax = syscall number in / result out,
//!   rdi, rsi, rdx, r10, r8, r9 = arguments 0..5.

pub const YIELD: u64 = 1;       // give up the time slice
pub const LOG: u64 = 2;         // kernel printf, rdi = NUL-terminated string
pub const TICK: u64 = 3;        // monotonic world-switch counter

pub const TGS_EXIT: u64 = 0x1000;    // exit, rdi = status
pub const TGS_SPAWN: u64 = 0x1001;   // load an ELF and spawn, rdi = path
pub const TGS_GETPID: u64 = 0x1002;  // pid of the calling world
pub const TGS_KEY: u64 = 0x1040;     // poll one HID key (0 = none)

// The posix block (0x2000..) is declared but the kernel does not dispatch it
// yet — see ustd/SYSCALLS.md. Numbers follow ustd/hal/src/sysnum.rs.
pub const MMAP: u64 = 0x2000;
pub const MUNMAP: u64 = 0x2001;
pub const READ: u64 = 0x2005;
pub const WRITE: u64 = 0x2006;

pub inline fn syscall0(num: u64) u64 {
    return asm volatile ("int $0x80"
        : [ret] "={rax}" (-> u64),
        : [num] "{rax}" (num),
    );
}

pub inline fn syscall1(num: u64, a0: u64) u64 {
    return asm volatile ("int $0x80"
        : [ret] "={rax}" (-> u64),
        : [num] "{rax}" (num),
          [a0] "{rdi}" (a0),
    );
}

/// Print a NUL-terminated string on the kernel console.
pub fn log(msg: [*:0]const u8) void {
    _ = syscall1(LOG, @intFromPtr(msg));
}

/// Terminate the calling world with `status`.
pub fn exit(status: u32) noreturn {
    _ = syscall1(TGS_EXIT, status);
    while (true) {}
}

pub fn getpid() u64 {
    return syscall0(TGS_GETPID);
}

pub fn yield_now() void {
    _ = syscall0(YIELD);
}
