<!-- SPDX-FileCopyrightText: 2026 CTRL-F-0rg3 -->
<!-- SPDX-License-Identifier: GPL-3.0-only -->

# Architecture: the boot chain (RISC-V)

## Overview

CoTanOS on RISC-V does not use OpenSBI. The whole boot chain is our own:

1. **CoTanSBI** (`ini/sbi`) — M-mode firmware loaded by QEMU
   as `-bios` at `0x8000_0000`. It acts as the SBI runtime: it serves
   ECALL from S-mode (console, timer, reset), delegates traps to S-mode,
   configures PMP and hands off to the bootloader via `mret`.
2. **ini/** — S-mode bootloader at `0x8020_0000`.
   The x86_64 BIOS equivalent: parses the FDT (RAM), builds Sv39 page
   tables with a physical 1:1 mapping (RAM + MMIO), configures
   the S-mode environment (stvec, timer) and jumps to the kernel.
3. **256xtr kernel** (`new/arc/rv6`) — starts in S-mode at `0x8040_0000`
   with `a0 = hartid`, `a1 = FDT`, `a2 = struct bootinfo`, and wires the
   `new/krn/mem` frame allocator over the RAM from BootInfo.

## ABI contract

See `ini/README.md` (details, memory map, SBI contract).

## Decisions

- **Own SBI instead of OpenSBI** — full control over the transition
  M-mode -> S-mode, minimal trusted-code surface, no
  dependency on external firmware.
- **Bootloader = BIOS** — hardware discovery (FDT) and paging setup
  happen outside the kernel; the kernel receives a ready-made,
  described environment (bootinfo).
- **1:1 map at start** — the kernel switches to its own page tables
  (e.g. higher-half addresses) in its first stage.
- **Firmware in C + assembly** (riscv64-elf-*), kernel in Rust —
  firmware is deliberately minimal and dependency-free.


