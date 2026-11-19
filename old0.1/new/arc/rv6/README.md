# SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
# SPDX-License-Identifier: GPL-3.0-only

# arc/rv6 — RISC-V architecture (256xtr kernel)

The RISC-V kernel entry of the **256xtr** microkernel: a Rust binary
crate (`#![no_std]`) linked at `0x8040_0000` and started by the `ini/`
bootloader with `a0 = hartid`, `a1 = FDT`, `a2 = &BootInfo`.

It wires the existing memory management from `new/krn/mem`
(`cotanos-mem`): the physical frame allocator is initialised over the
RAM described by BootInfo, the kernel + firmware regions are reserved,
and a frame allocate/release smoke test runs at boot. The kernel then
installs its own trap vector (timer via SBI) and idles — drawing the
boot spinner (rotating star outline) on the framebuffer each timer tick
until userspace takes over the screen.

## Build

```sh
cd new
RUSTFLAGS="-C link-arg=-T$PWD/arc/rv6/link.ld" \
    cargo +nightly build -p arc-rv6 --target riscv64gc-unknown-none-elf
# binary: new/target/riscv64gc-unknown-none-elf/debug/arc-rv6
```

`run-test-qemu.sh` builds the whole chain (SBI + bootloader + kernel)
and boots it under QEMU — see `ini/README.md` for the full picture.


