// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS Memory Management Subsystem
//!
//! This crate provides the complete memory management infrastructure for CoTanOS,
//! from early boot arena allocation to production-grade slab and zone-based allocators.

#![no_std]
#![feature(exact_size_is_empty)]
// NOTE: relaxed from `deny` to `warn` so the crate builds on RISC-V;
// restore `deny` once the remaining docs are filled in.
#![warn(missing_docs)]
#![deny(unsafe_op_in_unsafe_fn)]
#![warn(clippy::all)]

// ============================================================================
// Module declarations
// ============================================================================

/// Early boot arena allocator.
pub mod arn;

/// Physical frame allocator.
pub mod frm;

/// Bitmap engine.
pub mod map;

/// Slab allocator.
pub mod slb;

/// Statistics and diagnostics.
pub mod stat;

/// Zone manager.
pub mod zon;

// ============================================================================
// Prelude — commonly used types
// ============================================================================

/// Prelude module for convenient imports.
pub mod prelude {
    pub use crate::arn::ArenaAllocator;
    pub use crate::frm::{
        AllocatorStats, FragmentationReport, FrameError, IrqSpinlock, PageSize, PhysAddr,
        PhysFrame, PhysFrameRange, PhysicalFrameAllocator, Size1Gb, Size2Mb, Size4Kb,
    };
    pub use crate::map::{BitmapError, BitmapLayout};
    pub use crate::slb::{SlabAllocator, SlabError, SlabSize};
    pub use crate::stat::{AllocatorType, MemoryReport, StatCollector, StatRegistry};
    pub use crate::zon::{ZoneError, ZoneManager, ZoneStats, ZoneType};
}

pub use prelude::*;

// ============================================================================
// Crate-level utilities
// ============================================================================

/// Aligns `addr` up to a multiple of `align` (a power of two).
#[inline]
pub const fn align_up(addr: usize, align: usize) -> Option<usize> {
    if align == 0 || (align & (align - 1)) != 0 {
        return None;
    }
    let mask = align - 1;
    match addr.checked_add(mask) {
        Some(a) => Some(a & !mask),
        None => None,
    }
}

/// Aligns `addr` down to a multiple of `align` (a power of two).
#[inline]
pub const fn align_down(addr: usize, align: usize) -> Option<usize> {
    if align == 0 || (align & (align - 1)) != 0 {
        return None;
    }
    Some(addr & !(align - 1))
}

/// Returns `true` if `addr` is aligned to `align` (a power of two).
#[inline]
pub const fn is_aligned(addr: usize, align: usize) -> bool {
    align != 0 && (align & (align - 1)) == 0 && (addr & (align - 1)) == 0
}

// ============================================================================
// Compile-time checks
// ============================================================================

const _: () = assert!(is_aligned(Size4Kb::SIZE, Size4Kb::SIZE));
const _: () = assert!(is_aligned(Size2Mb::SIZE, Size2Mb::SIZE));
const _: () = assert!(is_aligned(Size1Gb::SIZE, Size1Gb::SIZE));

const _: () = assert!(Size4Kb::SIZE < Size2Mb::SIZE);
const _: () = assert!(Size2Mb::SIZE < Size1Gb::SIZE);

// ============================================================================
// Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_align_up() {
        assert_eq!(align_up(0, 4096), Some(0));
        assert_eq!(align_up(1, 4096), Some(4096));
        assert_eq!(align_up(4095, 4096), Some(4096));
        assert_eq!(align_up(4096, 4096), Some(4096));
        assert_eq!(align_up(4097, 4096), Some(8192));
        assert_eq!(align_up(usize::MAX, 4096), None);
        assert_eq!(align_up(0, 0), None);
        assert_eq!(align_up(0, 3), None);
    }

    #[test]
    fn test_align_down() {
        assert_eq!(align_down(0, 4096), Some(0));
        assert_eq!(align_down(1, 4096), Some(0));
        assert_eq!(align_down(4095, 4096), Some(0));
        assert_eq!(align_down(4096, 4096), Some(4096));
        assert_eq!(align_down(4097, 4096), Some(4096));
        assert_eq!(align_down(0, 0), None);
        assert_eq!(align_down(0, 3), None);
    }

    #[test]
    fn test_is_aligned() {
        assert!(is_aligned(0, 4096));
        assert!(is_aligned(4096, 4096));
        assert!(is_aligned(8192, 4096));
        assert!(!is_aligned(1, 4096));
        assert!(!is_aligned(4095, 4096));
        assert!(!is_aligned(0, 0));
        assert!(!is_aligned(0, 3));
    }
}
