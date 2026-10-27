# CoTanOS

**Formerly TrangorgeOS.**

CoTanOS is an operating system developed primarily for laptops: a lightweight system, lightweight applications and, in the long run, hardware designed together with the software. It is being rebuilt around a microkernel, with one deliberate exception (the network driver stays in the kernel).

Repository: `CTRL-F-0rg3/TrangorgeOS` (the repository name will follow the rename).

## Vision and goals

- **A complete platform, not just a kernel.** The OS, its applications, the app store and the hardware are designed as one ecosystem, so every layer can be understood and changed by the people who use it.
- **Laptops first.** Power efficiency, a responsive UI and a small footprint come before breadth of hardware support. Supporting every GPU or device ever made is a non-goal; supporting our own hardware and a short list of well-documented reference devices is.
- **Independence and openness.** The long-term target is RISC-V, including our own silicon and our own laptops. x86-64 stays as the proven development and first-product platform, and the system is written so that the architecture is a replaceable layer.
- **Layers under control.** Critical layers (kernel, capabilities, IPC, crypto) are kept small, audited and owned by us. Everything that can run outside the kernel does.

## Architecture

### Previous design: the separable tripartition

Up to the current alpha, CoTanOS (as TrangorgeOS) is a hybrid kernel organised as a **separable tripartition with nested systems**: kernel, driver space, graphics protocol and userspace are separate layers that were mapped onto x86 privilege rings:

| Layer | x86-64 ring |
|---|---|
| Kernel | ring 0 |
| Driver space | ring 1 |
| Graphics protocol | ring 2 |
| Userspace | ring 3 |

This model works, but it depends on features that only x86 provides in this form.

### New design: microkernel

The rebuild moves the system to a microkernel in which privilege is no longer tied to rings:

- **Kernel (`krn/`)**: a workspace of small crates: memory (`mem`, `vmm`), scheduling (`sch` for mechanism, `pol` for replaceable policy), IPC (`ipc`), capabilities (`cap`), interrupts and timers (`int`, `tmr`), syscalls (`sys`) and boot (`ini`).
- **Hardware abstraction (`krn/hal`)**: the only place where the kernel meets the architecture. `arc/x86` and `arc/rv6` implement it, so nothing outside `arc/` knows which CPU it runs on.
- **Drivers, services and applications run as isolated userspace processes.** They receive only the capabilities they need (MMIO ranges, interrupts, memory pages, channels). On RISC-V the privilege domains map to M/S/U modes plus PMP and an IOMMU for DMA isolation.
- **Exception: networking.** The network driver (`krn/net`) stays inside the kernel.

### Languages

| Area | Languages |
|---|---|
| Kernel | Rust, C, assembly |
| Drivers (default) | Zig |
| Drivers (allowed) | Rust, C, C++, D (`-betterC`), Odin |
| Services, libraries, applications | Rust (C and C++ where porting) |

Every driver is a separate process, so its language is an internal detail. All languages talk to the system through one interface description (`tls/gen/idl`) from which bindings are generated. The Zig compiler version is pinned in `.zig-version`.

## Hardware

The repository will also hold hardware work as it appears: silicon prototypes (RISC-V cores, FPGA and tape-out projects), board and laptop designs, and hardware-specific adaptations of the system. Until it exists it is not part of `new/`.

## Repository layout

The repository is being reorganised:

- `lsd/`: all legacy directories, moved without content changes (history is preserved with `git mv`).
- `new/`: the new structure. Legacy code is migrated into it module by module, and nothing in `new/` may depend on `lsd/`.

Directory names in `new/` are three-letter abbreviations. The full list lives in `new/doc/nam/abbreviations.md`.

| Directory | Contents |
|---|---|
| `krn/` | Microkernel, one crate per subsystem (`cor`, `hal`, `mem`, `vmm`, `sch`, `pol`, `ipc`, `cap`, `int`, `tmr`, `sys`, `ini`, `net`, `cc`) |
| `arc/` | Architecture-specific code: `x86/` and `rv6/` (RISC-V 64), each with boot code, assembly and platform drivers |
| `drv/` | Drivers, organised as class / family / driver (`sto`, `net`, `gpu`, `usb`, `inp`, `aud`, `fsy`, `bus`, `pwr`, ...) |
| `srv/` | System services in userspace (`gfx`, `vfs`, `net`, `ini`, `drm`) |
| `lib/` | Libraries: `std`, `clb` (libc), `ipc` (generated bindings), `gui` |
| `app/` | User applications |
| `doc/` | Specifications, design decisions, naming glossary |
| `tls/` | Build tools, generators (IDL, bindings, driver index) and repository linters |
| `tst/` | QEMU runs, benchmarks, unit tests, driver conformance tests, hardware tests |
| `dst/` | Distribution: ISO images, packages, installer |
| `dem/`, `web/`, `not/` | Demos, website, notes |

Each driver has the same layout: `drv.toml` (full name, class, language, license, status, architectures, capabilities), `README.md` and its sources. The driver index and the per-architecture workspaces are generated from the `drv.toml` files.

## Distribution

Official images are published from `dst/`. Builds for x86-64 and RISC-V are produced from the same sources through `Cargo.x86.toml` and `Cargo.rv6.toml`.

## Versioning

| Stage | Meaning |
|---|---|
| **Alpha** | The current stage. |
| **Beta** | The next stage of development. It begins with a working graphical interface. |
| **Gamma** | The stage that completes the ecosystem around the system. |
| **1.0 and later** | Standard numbering. From 1.0 the exact version is stated at the bottom of this file. |

## Roadmap

A one-year plan, from the rebuild to running **Vintage Story** on CoTanOS. Quarters are approximate.

1. **Q1, rebuild.** Repository restructure (`lsd/` and `new/`), tag and branch for the migration, the microkernel core on x86-64 (`cor`, `hal`, `mem`, `vmm`, `int`, `tmr`), tests and benchmarks of the current scheduler and allocator.
2. **Q2, isolation.** `sch` and `pol` rebuilt, `ipc` and `cap`, syscalls, the userspace driver model with `drv.toml`, and the first Zig drivers (storage, input, framebuffer).
3. **Q3, interface.** Graphics protocol and `gfx` service, `vfs`, `net`, the GUI library and the first applications. A working graphical interface starts the beta stage.
4. **Q4, compatibility.** Enough of the libc/`std` layer, threading, dynamic linking, audio, hardware-accelerated or fast software rendering and networking to run Vintage Story. RISC-V port brought up in parallel on FPGA and development boards.

## Licensing

- **Default license: GPL-3.0-only.** It applies to every file that does not state another license in its header.
- **Network driver:** `krn/net` is licensed under BSD-2-Clause (see `new/krn/net/LICENSE`).
- **Drivers are a licensing exception.** Because a driver is an isolated process that talks to the system only over IPC, its author may choose its license, including a closed-source one, where that is technically and legally possible. The license is declared in the driver's `drv.toml` and in its file headers.
- **Contributors keep control of what they write.** A contributor decides the license of the files they created. Every file should carry an SPDX header:

  ```
  SPDX-FileCopyrightText: <year> <author>
  SPDX-License-Identifier: <license>
  ```

- **Missing header = GPL-3.0-only.** A file without a license header is covered by the project-wide GPL-3.0-only license.
- **The project license can be changed without the consent of every contributor.** What cannot be changed without the author is the license of files that author created and licensed differently.

## Contributing

1. Read `new/doc/nam/abbreviations.md` and the specification for the area you touch (`new/doc/spc/`).
2. New code goes into `new/`. Do not add dependencies on `lsd/`.
3. New drivers start from a template in `new/drv/_tpl/` and need a complete `drv.toml`.
4. Drivers must pass the conformance tests in `tst/cnf/` regardless of language.
5. Put the SPDX header in every file you create.

---

*Version: alpha. The exact version will be listed here starting with 1.0.*
