//! Ring-switch trampoline: worlds (kernel/driver/user) with their own address
//! spaces, switched on hypercall/syscall.
//!
//! The architecture-specific implementation installs the `int 0x80` gate at
//! DPL 3 and defines [`CpuCtx`], [`add_world`] and [`start`]; this module just
//! re-exports it so `crate::trampoline_rings::...` resolves regardless of arch.

#[cfg(target_arch = "x86_64")]
mod arch;

#[cfg(target_arch = "x86_64")]
pub use arch::x86_64::trampoline_rings::*;
