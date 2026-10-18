//! The ring-3 userspace: process loading, syscalls and the world trampoline.
//!
//! This is the *real* userspace (separate from the kernel-linked `tgs-userspace`
//! shell): an ELF image is loaded into a fresh address space, registered as a
//! world in [`crate::trampoline_rings`], and entered in ring 3. Syscalls arrive
//! through `int 0x80` and are dispatched by [`process::syscall::handle`].

pub mod process;

