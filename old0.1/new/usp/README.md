# SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
# SPDX-License-Identifier: GPL-3.0-only

# usp/ — CoTanOS userspace (demo programs with full standard libraries)

The 256xtr kernel implements the **Linux syscall ABI on RISC-V**
(`ecall` from U-mode, `a7` = syscall number, errno as negative return),
so this tree contains completely ordinary **static riscv64-linux
binaries**:

| Program         | Language | Standard library                                  |
|-----------------|----------|---------------------------------------------------|
| `/bin/init`     | C        | glibc (full POSIX: uname, sysinfo, clock_gettime…)|
| `/bin/hello-cpp`| C++      | libstdc++ (iostream, vector, string, algorithm)   |
| `/bin/hello-rs` | Rust     | the real, precompiled `std` (target `riscv64gc-unknown-linux-gnu`, `crt-static`) |

`init` chains the programs with `execve` (the kernel has no `fork` yet):

```
/bin/init -> /bin/hello-cpp -> /bin/hello-rs -> /bin/init done -> exit
```

…after which the kernel restarts `/bin/init` (up to 3 times, then halts).

## Toolchain (host)

* `riscv64-linux-gnu-gcc` / `g++` (Debian/Ubuntu package
  `gcc-riscv64-linux-gnu` / `g++-riscv64-linux-gnu`),
* `rustc` + the target `riscv64gc-unknown-linux-gnu`
  (`rustup target add riscv64gc-unknown-linux-gnu`) — the std is
  precompiled by the Rust project, nothing is recompiled here,
* `cpio` for packing the initrd.

## Build

```sh
make                # builds the programs, packs ../arc/rv6/initrd.cpio
```

The kernel embeds the archive (`include_bytes!`) — rebuild the kernel
afterwards (`run-test-qemu.sh` does both).

## What the demos prove

* C: `printf` (write), `uname`, `sysinfo` (allocator statistics),
  `clock_gettime` (CLINT), `getpid`, `execve`, `environ`.
* C++: iostream formatting + containers — all through the same libc.
* Rust: `HashMap` (its hasher seeds via the `getrandom` syscall),
  `std::env::args` (reads `/proc/self/cmdline` through
  openat/read/close), `Instant::now` (clock_gettime), `Vec`, `format!`.
