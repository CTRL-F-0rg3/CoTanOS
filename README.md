
# CoTanOS

CoTanOS is a high-performance, lightweight operating system designed primarily for laptops. It focuses on extreme system responsiveness, power efficiency, and seamless software-hardware co-design. 

At its heart lies the ZFC kernel, a hybrid microkernel and exokernel architecture built to maximize security without sacrificing the raw performance required for modern mobile computing.

## Vision and Core Goals

- Laptop-First Philosophy: 
  Power efficiency, thermal management, fast sleep/wake states, and UI responsiveness take absolute priority. CoTanOS is optimized for the mobile form factor rather than legacy desktop or server hardware.

- Hybrid Microkernel and Exokernel Design: 
  The ZFC kernel provides the security and isolation of a microkernel (capability-based access, strict IPC) while exporting hardware resources exokernel-style. This allows specialized, high-performance userspace drivers to manage hardware directly with kernel-enforced boundaries.

- Dual Architecture Support: 
  Built natively for both x86_64 and RISC-V (rv64gc). The architecture abstraction layer ensures deep, optimized support for both ecosystems, from custom silicon to mainstream laptop platforms.

- Pragmatic Performance: 
  We favor real-world usability and bare-metal performance over academic formal verification. If a component requires kernel-space integration to meet strict laptop performance targets, we make pragmatic, well-documented architectural decisions.

- Multi-Language Systems Stack: 
  We leverage the right tool for the job: Rust for memory-safe core components, Zig and C for low-level hardware interactions, and x86_64/RISC-V Assembly for bootloaders and critical context switching.

- By Users, For Users: 
  A fully open environment. The OS, its toolchain, and the base userland are designed to be accessible, understandable, and modifiable by the community.

## Architecture and the ZFC Kernel

ZFC operates on a hybrid model. Minimal, critical responsibilities reside in the core kernel, while the majority of the OS (drivers, file systems, network stack, UI) runs in isolated userspace.

- Kernel Space (ZFC):
  Handles core primitives such as physical and virtual memory management, CPU scheduling, Inter-Process Communication (IPC), and capability routing. It provides secure hardware resource assignment (MMIO, IRQs, DMA) to userspace protectors via exokernel interfaces, alongside arch-specific traps and context switching.

- Userspace (Isolated and Protected):
  - System Servers: Core services managing virtual file systems, networking, display composition, and power management.
  - Device Drivers: Isolated processes communicating via zero-copy IPC, preventing a single driver failure from crashing the entire system.
  - User Applications: Fully sandboxed applications running with minimal, capability-granted privileges.

## Project Governance and Licensing

- Default License: The entire project is licensed under the GNU General Public License v3.0 only (GPL-3.0-only), unless explicitly stated otherwise in a specific file header.
- Open Source Guarantee: Any future license modifications must strictly remain within the GPL family of licenses to permanently preserve the open-source and copyleft nature of the project.
- Author Rights: Contributors retain copyright over their original code contributions as specified in their SPDX headers.

## Contributing

We welcome contributions from developers of all backgrounds. Whether you specialize in assembly, Rust, Zig, or C, there is a place for you in CoTanOS.

To get started:
1. Review the architectural specifications and design documents in the project documentation.
2. Ensure your code compiles cleanly for the target architecture (x86_64 or riscv64).
3. Include the appropriate SPDX header in all new files:

SPDX-FileCopyrightText: <year> <author>
SPDX-License-Identifier: GPL-3.0-only

Let us build the ultimate laptop operating system, from the bare metal up.

