# SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
# SPDX-License-Identifier: GPL-3.0-only

# ini/ — CoTanOS Hardware Bootloader (RISC-V)

CoTanOS's own RISC-V boot chain (no OpenSBI). The x86_64 analogy:
what the BIOS did there is done here by **ini/** (S-mode); the M-mode
firmware is our own **CoTanSBI** (`ini/sbi`).

## Boot stages

```
QEMU reset vector (0x1000)  -- a0 = hartid, a1 = FDT address -->
  [1] CoTanSBI  (M-mode)  ini/sbi   -bios          @ 0x8000_0000
        own SBI: ECALL (console, timer, reset), trap delegation,
        PMP, parking of secondary harts, mret into the bootloader
      [2] ini/      (S-mode)  ini/    -device loader @ 0x8020_0000
        BIOS role: FDT parsing (RAM), physical 1:1 mapping
        (Sv39: RAM + MMIO), welcome screen (ramfb), jump to the kernel
        [3] 256xtr kernel (S-mode)  0x8040_0000  (QEMU -kernel, arc-rv6)
              a0 = hartid, a1 = FDT, a2 = struct bootinfo (memory, FB...)
```

## Memory map (QEMU `virt`, 128 MiB RAM)

| Range                        | Purpose                          |
|------------------------------|----------------------------------|
| 0x0000_1000                  | QEMU reset vector (ROM)          |
| 0x0001_0000                  | test device (poweroff)           |
| 0x0200_0000                  | CLINT (timer)                    |
| 0x0C00_0000                  | PLIC (external interrupts)       |
| 0x1000_0000                  | UART NS16550A                    |
| 0x1010_0000                  | fw_cfg (ramfb!)                  |
| 0x3000_0000                  | PCIe ECAM                        |
| 0x8000_0000                  | **CoTanSBI** (ini/sbi/fw.bin)    |
| 0x8020_0000                  | **ini/ bootloader** (boot.bin)   |
| 0x8040_0000                  | **256xtr kernel** (arc-rv6)      |
| 0x8100_0000                  | font atlas (ini/font_atlas.bin)  |
| 0x8500_0000                  | ramfb framebuffer (640x480x32)   |
| 0x8000_0000 + RAM            | RAM (default up to 0x8800_0000)  |
| RAM end                      | FDT (Flattened Device Tree)      |

## Build and run

```sh
./run-test-qemu.sh                # QEMU GRAPHICAL window (GTK) + console
./run-test-qemu.sh --nographic    # serial console only (CI/SSH)
```

* Builds: on the host (`riscv64-elf-gcc` + `cargo`) or automatically in a
  container (podman/docker, image `cotanos-boot` from
  `tls/container/Dockerfile.boot`); a stale image without the tools is
  detected and rebuilt.
* QEMU: host preferred (the window works natively, gtk/sdl backend
  picked automatically); without host QEMU everything runs in a
  container with the display forwarded (X11/Wayland).
* Quit QEMU: `Ctrl+A`, then `X`.
* The window is **ramfb** (`-device ramfb`) — ini submits a framebuffer
  in guest RAM (640x480x32, XRGB8888) through fw_cfg. The boot screen is
  in the **macOS style**, clean and minimal, with the **DejaVu Sans font
  atlas** from TrangorgeOS (`lsd/uspace/src/gfx/font_atlas.bin` +
  metrics, 4 bpp alpha anti-aliasing):
  1. **ini/**: black background + white `COTANOS` centered (19 px) —
     no window, no on-screen log (the log lives on the serial console),
  2. **kernel (256xtr)**: keeps that screen and draws a **spinning star
     outline** at the bottom — the newest spoke is white and the older
     ones fade out, so the outline draws itself and disappears while it
     rotates. It spins for the whole kernel startup and ends when
     userspace takes over the framebuffer. The animation is driven by
     S-mode timer ticks (26 ms).
  The framebuffer address reaches the kernel in `bootinfo` (version 2).
  (PCI VGA does not work on riscv 'virt': QEMU assigns no BARs there.)

Host requirements: `make` and `qemu-system-riscv64` (for the window)
and/or docker/podman. The container builds itself on first run.

Manual build of the individual stages:

```sh
make -C ini/sbi                # -> fw.bin   (CoTanSBI, M-mode)
make -C ini                    # -> boot.bin (bootloader, S-mode)
cd new && RUSTFLAGS="-C link-arg=-T$PWD/arc/rv6/link.ld" \
    cargo +nightly build -p arc-rv6 --target riscv64gc-unknown-none-elf
```

## Contract between the stages

* **CoTanSBI -> ini** (`mret` into S-mode): `a0 = hartid`, `a1 = FDT`.
* **ini -> kernel**: `a0 = hartid`, `a1 = FDT`, `a2 = &struct bootinfo`
  (see `bootinfo.h`), active Sv39 1:1 map (RAM + MMIO), SBI works
  (ECALL), S-mode timer via CLINT/SBI.
* The SBI implements v0.2+/v1.0: `DBCN` (putchar), `TIME` (set_timer),
  `SRST` (reset), `Base` — plus legacy v0.1 (putchar/set_timer/shutdown).
* FDT: QEMU >= 6.0 passes the address in `a1`; the bootloader also has
  a RAM-scan fallback (older QEMU).

## Next steps (roadmap)

* Kernel switch to its own page tables (e.g. higher-half addresses).
* PLIC + external interrupts (virtio disks, keyboard).
* SMP: waking secondary harts through SBI (fence/HART).

## Notes

* `ini/` and `ini/sbi/` are C + assembly (`riscv64-elf-*` tools) — on
  purpose: firmware must be minimal and predictable. The kernel (256xtr)
  stays in Rust (`new/arc/rv6`, target `riscv64gc-unknown-none-elf`).
* Concept documentation: `ini/doc/` (HTML), decisions: `new/doc/arc/`.

