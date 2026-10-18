//! x86_64 trampoline: the Rust half of the ring switch and the matching
//! assembly (`trampoline_rings.s`, compiled by the kernel `build.rs`).

pub mod trampoline_rings;
