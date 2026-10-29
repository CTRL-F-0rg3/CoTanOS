# CoTanOS

CoTanOS is a lightweight operating system designed primarily for laptops, focusing on system responsiveness, resource efficiency, and software-hardware co-design. 

The core of the system is a custom microkernel named **256xtr**, built around an isolated userspace model with one intentional performance exception: the network driver remains inside kernel space.

---

## Vision and Core Goals

* **A Unified Platform, Not Just a Kernel**
  The OS, its application ecosystem, app repository, and custom hardware are designed as one cohesive platform. Every layer is structured to be accessible, understandable, and modifiable by the user.

* **Laptop First & Developer Ergonomics**
  Power efficiency, UI responsiveness, low resource footprint, and developer ergonomics take priority over universal hardware support. The target is deep, optimized support for our reference hardware and custom designs rather than legacy devices.

* **Pure RISC-V Focus**
  CoTanOS is built exclusively for the **RISC-V** architecture. Our long-term roadmap focuses entirely on RISC-V hardware, ranging from silicon prototypes (FPGA/tape-out) to custom open-hardware laptops.

* **Pragmatic Usability over Formal Verification**
  We have transitioned away from mathematical formal proofs in favor of real-world usability, performance, and reliability on our own bare-metal silicon and reference hardware.

* **Dedicated Systems Language**
  A custom, domain-specific programming language is planned to streamline systems development, driver creation, and core component safety.

* **By Users, For Users**
  CoTanOS is an open environment built by the community for the community. Every contribution is welcome—join us in creating a truly user-owned operating system.

---

## Architecture & Kernel (256xtr)

CoTanOS operates on the **256xtr** microkernel architecture. Minimal responsibilities reside in the core kernel, while drivers, system services, and applications execute in isolated userspace processes using capability-based access control and IPC.

### System Hierarchy & Privilege Levels

* **Userspace:**
  * **User Applications:** Fully isolated, running with minimal privileges.
  * **System Services:** (`gfx`, `vfs`, `init`, `drm`) Isolated services managing core system responsibilities.
  * **Isolated Drivers:** (Zig, Rust, C, etc.) Run as independent processes with access restricted only to assigned hardware resources (MMIO ranges, interrupts).

* **Kernel Space (256xtr):**
  * **Core Microkernel:** Handles essential primitives (`mem`, `sch`, `ipc`, `cap`). A minimal, audited codebase.
  * **Network Stack Exception (`krn/net`):** Embedded directly inside kernel space (Ring 0 / Machine mode) purely for performance reasons.

---

## Project Governance & Licensing

* **Default License:** The project is licensed under **GPL-3.0-only** unless explicitly stated otherwise in a file header.
* **License Changes:** Amendments to the core project license require consensus from the primary maintainer and major contributors. Minor code contributions (e.g., minor userspace tweaks) cannot block licensing decisions.
* **GPL Family Constraint:** Any future license modifications must strictly remain within the **GPL family of licenses** to preserve the open-source nature of the project.
* **Author Rights:** Authors retain copyright over their original code contributions as specified in their SPDX headers.

---

## Repository Structure (`new/`)

* **`krn/`** — **256xtr** microkernel crates (`cor`, `hal`, `mem`, `vmm`, `sch`, `pol`, `ipc`, `cap`, `int`, `tmr`, `sys`, `ini`, `net`)
* **`arc/`** — RISC-V (`rv6/`) architecture implementation, boot code, and platform drivers
* **`drv/`** — Isolated userspace drivers (`sto`, `gpu`, `inp`, `aud`, `fsy`, `bus`, `pwr`)
* **`srv/`** — Userspace system services (`gfx`, `vfs`, `net`, `ini`, `drm`)
* **`lib/`** — Core libraries (`std`, `clb`, generated `ipc` bindings, `gui`)
* **`app/`** — Userspace applications
* **`tls/`** — Build toolchain, IDL generators, and repository linters
* **`doc/`** — Specifications, architectural decisions, and naming glossary

---

## Contributing

We welcome contributions from everyone! To get started:

1. Review the architecture guidelines and specs in `new/doc/`.
2. Ensure new drivers include a valid `drv.toml` and pass conformance checks in `tst/cnf/`.
3. Include the appropriate SPDX header in all new files:
   ```text
   SPDX-FileCopyrightText: <year> <author>
   SPDX-License-Identifier: GPL-3.0-only
   ```
   Let’s build an open platform from users, for users.