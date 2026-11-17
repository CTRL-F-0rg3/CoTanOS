// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — Sv39 page tables (per-process address spaces).
//!
//! All page-table frames (root + intermediate tables) come from the
//! `cotanos-mem` physical frame allocator. Physical addresses are identity
//! mapped at all times, so a PTE is reached by dereferencing its physical
//! address directly.

use core::arch::asm;

use cotanos_mem::prelude::{PhysicalFrameAllocator, Size4Kb};

pub const PTE_V: usize = 1 << 0;
pub const PTE_R: usize = 1 << 1;
pub const PTE_W: usize = 1 << 2;
pub const PTE_X: usize = 1 << 3;
pub const PTE_U: usize = 1 << 4;
pub const PTE_G: usize = 1 << 5;
pub const PTE_A: usize = 1 << 6;
pub const PTE_D: usize = 1 << 7;

pub struct PageTable {
    pub root: usize, // physical address of the root table (identity = VA)
}

impl PageTable {
    /// Creates an empty page table (one zeroed root frame).
    pub fn new(alloc: &'static PhysicalFrameAllocator<Size4Kb>) -> Option<Self> {
        let f = alloc.allocate_frame()?;
        let root = f.start_address().as_usize();
        unsafe { core::ptr::write_bytes(root as *mut u8, 0, 4096) };
        Some(Self { root })
    }

    fn table_at(pa: usize) -> &'static mut [usize; 512] {
        // SAFETY: identity mapping — physical == virtual, table is a frame.
        unsafe { &mut *(pa as *mut [usize; 512]) }
    }

    /// Allocates a zeroed page-table frame.
    fn new_table(alloc: &'static PhysicalFrameAllocator<Size4Kb>) -> Option<usize> {
        let f = alloc.allocate_frame()?;
        let pa = f.start_address().as_usize();
        unsafe { core::ptr::write_bytes(pa as *mut u8, 0, 4096) };
        Some(pa)
    }

    /// Maps a 4 KiB leaf. Creates intermediate tables as needed.
    pub fn map(
        &self,
        va: usize,
        pa: usize,
        flags: usize,
        alloc: &'static PhysicalFrameAllocator<Size4Kb>,
    ) -> Result<(), ()> {
        let mut table = self.root;
        for level in (1..=2).rev() {
            let idx = (va >> (12 + 9 * level)) & 0x1FF;
            let entry = Self::table_at(table)[idx];
            if entry & PTE_V == 0 {
                let nt = Self::new_table(alloc).ok_or(())?;
                Self::table_at(table)[idx] = (nt >> 12 << 10) | PTE_V;
                table = nt;
            } else if entry & (PTE_R | PTE_W | PTE_X) != 0 {
                // A higher-level leaf already occupies this range.
                return Err(());
            } else {
                table = (entry >> 10) << 12;
            }
        }
        let idx = (va >> 12) & 0x1FF;
        Self::table_at(table)[idx] = ((pa >> 12) << 10) | flags;
        Self::flush_tlb();
        Ok(())
    }

    /// Maps a 2 MiB leaf directly (kernel/MMIO identity regions only).
    pub fn map_2m(
        &self,
        va: usize,
        pa: usize,
        flags: usize,
        alloc: &'static PhysicalFrameAllocator<Size4Kb>,
    ) -> Result<(), ()> {
        debug_assert!(va % 0x200000 == 0 && pa % 0x200000 == 0);
        let mut table = self.root;
        let idx2 = (va >> 30) & 0x1FF;
        let entry2 = Self::table_at(table)[idx2];
        if entry2 & PTE_V == 0 {
            let nt = Self::new_table(alloc).ok_or(())?;
            Self::table_at(table)[idx2] = (nt >> 12 << 10) | PTE_V;
            table = nt;
        } else {
            table = (entry2 >> 10) << 12;
        }
        let idx1 = (va >> 21) & 0x1FF;
        Self::table_at(table)[idx1] = ((pa >> 12) << 10) | flags;
        Self::flush_tlb();
        Ok(())
    }

    /// Flushes the local TLB after page-table modifications. The RISC-V
    /// spec does not require TLB shootdown for our single hart, but the
    /// stale translations MUST be dropped (QEMU caches them).
    fn flush_tlb() {
        unsafe {
            asm!("sfence.vma zero, zero");
        }
    }

    /// Walks to the leaf for `va`; returns (leaf PTE value, leaf PTE ptr,
    /// level: 0 = 4 KiB, 1 = 2 MiB, 2 = 1 GiB) when present, else `None`.
    fn leaf(&self, va: usize) -> Option<(usize, *mut usize, usize)> {
        let mut table = self.root;
        for level in (1..=2).rev() {
            let idx = (va >> (12 + 9 * level)) & 0x1FF;
            let entry = Self::table_at(table)[idx];
            if entry & PTE_V == 0 {
                return None;
            }
            if entry & (PTE_R | PTE_W | PTE_X) != 0 {
                // SAFETY: `table` points at an allocated page-table frame.
                let p = unsafe { (table as *mut usize).add(idx) };
                return Some((entry, p, level));
            }
            table = (entry >> 10) << 12;
        }
        let idx = (va >> 12) & 0x1FF;
        let entry = Self::table_at(table)[idx];
        if entry & PTE_V == 0 {
            return None;
        }
        // SAFETY: `table` points at an allocated page-table frame.
        let p = unsafe { (table as *mut usize).add(idx) };
        Some((entry, p, 0))
    }

    /// Returns the physical address for `va`, or 0.
    pub fn pa_of(&self, va: usize) -> usize {
        match self.leaf(va) {
            Some((entry, _, _)) => (entry >> 10) << 12,
            None => 0,
        }
    }

    /// Returns the leaf flags for `va`, or 0.
    pub fn flags_of(&self, va: usize) -> usize {
        match self.leaf(va) {
            Some((entry, _, _)) => entry & 0x3FF,
            None => 0,
        }
    }

    /// Updates the leaf flags for `va` (caller provides the full flag word).
    pub fn set_flags(&self, va: usize, flags: usize) {
        if let Some((_, p, _)) = self.leaf(va) {
            // SAFETY: the pointer comes from our own table walk.
            unsafe { *p = (*p & !0x3FF) | (flags & 0x3FF) };
            Self::flush_tlb();
        }
    }

    /// Unmaps the leaf at `va`; returns its physical address (4 KiB leaves
    /// only — 2 MiB identity leaves return `None` and are never freed).
    pub fn unmap(
        &self,
        va: usize,
        _alloc: &'static PhysicalFrameAllocator<Size4Kb>,
    ) -> Option<usize> {
        let (entry, p, level) = self.leaf(va)?;
        // SAFETY: the pointer comes from our own table walk.
        unsafe { *p = 0 };
        Self::flush_tlb();
        if level != 0 {
            // Identity 2 MiB leaf — the caller must not free this frame.
            return None;
        }
        // Intermediate tables are intentionally kept allocated: they are
        // few, and the next mapping reuses them (no churn for 256xtr v1).
        Some((entry >> 10) << 12)
    }

    /// Switches the CPU to this page table (Sv39).
    pub fn activate(&self) {
        let satp = (8usize << 60) | (self.root >> 12);
        unsafe {
            asm!("csrw satp, {0}", in(reg) satp);
            asm!("sfence.vma zero, zero");
        }
    }
}
