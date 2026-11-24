const std = @import("std");

const SYS_WRITE = 64;
const SYS_EXIT = 93;

export fn _start() callconv(.C) noreturn {
    const msg = "syslog daemon started\n";
    _ = syscall3(SYS_WRITE, 1, @ptrToInt(msg), msg.len);
    while (true) {
        _ = syscall3(SYS_EXIT, 0, 0, 0);
    }
}

fn syscall3(n: usize, a: usize, b: usize, c: usize) usize {
    return asm volatile ("ecall"
        : [ret] "={x10}" (-> usize)
        : [n] "{x17}" (n),
          [a] "{x10}" (a),
          [b] "{x11}" (b),
          [c] "{x12}" (c)
        : "memory"
    );
}
