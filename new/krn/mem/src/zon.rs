// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS Memory Zone Manager (`zon.rs`)
//!
//! Manages physical memory divided into zones based on hardware constraints
//! and access limitations. This is critical for DMA-capable devices, 32-bit
//! peripherals, and NUMA-aware allocation strategies.
//!
//! # Design Principles
//!
//! * **Hardware-aware partitioning**: Memory is divided into zones based on
//!   physical address ranges that satisfy specific hardware constraints:
//!   - `ZoneDma`: Below 16 MiB (legacy ISA DMA)
//!   - `ZoneDma32`: Below 4 GiB (32-bit DMA, PCI devices)
//!   - `ZoneNormal`: All remaining memory (general-purpose)
//!   - `ZoneHighMem`: Above direct-mapping window (if applicable)
//!
//! * **Fallback allocation**: When a zone is exhausted, the manager can
//!   allocate from other zones (with appropriate warnings), preventing
//!   unnecessary OOM conditions.
//!
//! * **Per-zone bitmaps**: Each zone maintains its own bitmap, allowing
//!   independent management and statistics without global locks.
//!
//! * **SMP-safe**: All zone operations use [`IrqSpinlock`] from `frm.rs`.
//!
//! * **Integration**: Works seamlessly with [`PhysicalFrameAllocator`] and
//!   registers with [`StatRegistry`] for unified diagnostics.
//!
//! # File Layout
//!
//! 1. [`ZoneError`] — error types.
//! 2. [`ZoneType`] — zone classification enum.
//! 3. [`Zone`] — single zone with bitmap and statistics.
//! 4. [`ZoneManager`] — manages all zones, handles fallback.
//! 5. Zone allocation strategies (preferred, fallback, contiguous).
//! 6. Statistics and diagnostics.
//! 7. Tests.

use core::fmt;
use core::marker::PhantomData;
use core::sync::atomic::{AtomicUsize, Ordering};

use crate::frm::{FrameError, IrqSpinlock, PageSize, PhysAddr, PhysFrame};
use crate::map::{BitmapError, BitmapLayout};

// ============================================================================
// 1. ZoneError
// ============================================================================

/// Errors that can occur during zone operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ZoneError {
    /// The requested zone type is not available or not initialized.
    ZoneNotAvailable {
        zone_type: ZoneType,
    },
    /// No memory available in the requested zone (and fallback disabled).
    OutOfMemory {
        zone_type: ZoneType,
        requested_frames: usize,
    },
    /// The physical address does not belong to any known zone.
    AddressNotInZone {
        addr: PhysAddr,
    },
    /// The frame was not allocated from this zone.
    InvalidFrame {
        frame: PhysAddr,
        expected_zone: ZoneType,
    },
    /// The zone manager is not initialized.
    NotInitialized,
    /// Bitmap operation failed.
    BitmapError(BitmapError),
    /// Frame allocator operation failed.
    FrameError(FrameError),
    /// Contiguous allocation failed (fragmentation).
    ContiguousAllocationFailed {
        requested: usize,
        largest_available: usize,
    },
}

impl fmt::Display for ZoneError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::ZoneNotAvailable { zone_type } => {
                write!(f, "zone: {} not available", zone_type.name())
            }
            Self::OutOfMemory {
                zone_type,
                requested_frames,
            } => {
                write!(
                    f,
                    "zone: {} out of memory (requested {} frames)",
                    zone_type.name(),
                    requested_frames
                )
            }
            Self::AddressNotInZone { addr } => {
                write!(f, "zone: address {:#x} not in any zone", addr)
            }
            Self::InvalidFrame {
                frame,
                expected_zone,
            } => {
                write!(
                    f,
                    "zone: frame {:#x} not from {} zone",
                    frame,
                    expected_zone.name()
                )
            }
            Self::NotInitialized => write!(f, "zone: manager not initialized"),
            Self::BitmapError(e) => write!(f, "zone: bitmap error: {}", e),
            Self::FrameError(e) => write!(f, "zone: frame error: {}", e),
            Self::ContiguousAllocationFailed {
                requested,
                largest_available,
            } => {
                write!(
                    f,
                    "zone: contiguous allocation failed (requested {}, largest {})",
                    requested, largest_available
                )
            }
        }
    }
}

impl From<BitmapError> for ZoneError {
    fn from(e: BitmapError) -> Self {
        Self::BitmapError(e)
    }
}

impl From<FrameError> for ZoneError {
    fn from(e: FrameError) -> Self {
        Self::FrameError(e)
    }
}

// ============================================================================
// 2. ZoneType — zone classification
// ============================================================================

/// Classification of memory zones based on hardware constraints.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub enum ZoneType {
    /// Legacy ISA DMA zone: physical addresses below 16 MiB.
    /// Required for old ISA devices that can only address 24 bits.
    Dma,
    /// 32-bit DMA zone: physical addresses below 4 GiB.
    /// Required for PCI devices and other 32-bit DMA masters.
    Dma32,
    /// Normal zone: all remaining memory above 4 GiB (or all memory on
    /// systems with less than 4 GiB).
    Normal,
    /// High memory zone: memory above the kernel's direct-mapping window.
    /// Only relevant on 32-bit systems or special configurations.
    HighMem,
}

impl ZoneType {
    /// Returns the human-readable name.
    #[inline]
    pub const fn name(self) -> &'static str {
        match self {
            Self::Dma => "DMA",
            Self::Dma32 => "DMA32",
            Self::Normal => "Normal",
            Self::HighMem => "HighMem",
        }
    }

    /// Returns the lower bound (inclusive) of this zone's address range.
    #[inline]
    pub const fn lower_bound(self) -> usize {
        match self {
            Self::Dma => 0,
            Self::Dma32 => 0x0100_0000, // 16 MiB
            Self::Normal => 0x1_0000_0000, // 4 GiB
            Self::HighMem => 0x8000_0000_0000, // 128 TiB (architecture-dependent)
        }
    }

    /// Returns the upper bound (exclusive) of this zone's address range.
    #[inline]
    pub const fn upper_bound(self) -> usize {
        match self {
            Self::Dma => 0x0100_0000, // 16 MiB
            Self::Dma32 => 0x1_0000_0000, // 4 GiB
            Self::Normal => 0x8000_0000_0000, // 128 TiB
            Self::HighMem => usize::MAX,
        }
    }

    /// Returns `true` if the given physical address belongs to this zone.
    #[inline]
    pub const fn contains(self, addr: PhysAddr) -> bool {
        addr.as_usize() >= self.lower_bound() && addr.as_usize() < self.upper_bound()
    }

    /// Returns the fallback order for this zone (zones to try when this one is full).
    #[inline]
    pub const fn fallback_order(self) -> [ZoneType; 3] {
        match self {
            Self::Dma => [ZoneType::Dma32, ZoneType::Normal, ZoneType::HighMem],
            Self::Dma32 => [ZoneType::Normal, ZoneType::HighMem, ZoneType::Dma],
            Self::Normal => [ZoneType::HighMem, ZoneType::Dma32, ZoneType::Dma],
            Self::HighMem => [ZoneType::Normal, ZoneType::Dma32, ZoneType::Dma],
        }
    }

    /// Returns all zone types in priority order.
    #[inline]
    pub const fn all() -> [ZoneType; 4] {
        [ZoneType::Dma, ZoneType::Dma32, ZoneType::Normal, ZoneType::HighMem]
    }
}

impl fmt::Display for ZoneType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.name())
    }
}

// ============================================================================
// 3. Zone — single zone with bitmap
// ============================================================================

/// A memory zone managing a specific address range.
///
/// Each zone has its own bitmap and statistics, allowing independent management
/// without global locks. The zone tracks which frames are allocated and provides
/// allocation/deallocation operations.
pub struct Zone<S: PageSize> {
    /// Type of this zone.
    zone_type: ZoneType,
    /// Lower bound (inclusive) of this zone's address range.
    lower_bound: PhysAddr,
    /// Upper bound (exclusive) of this zone's address range.
    upper_bound: PhysAddr,
    /// Bitmap tracking allocated frames.
    bitmap: IrqSpinlock<BitmapLayout>,
    /// Total number of frames in this zone.
    total_frames: usize,
    /// Number of currently allocated frames.
    allocated_frames: AtomicUsize,
    /// Marker for page size.
    _marker: PhantomData<S>,
}

impl<S: PageSize> Zone<S> {
    /// Creates a new zone for the given address range.
    ///
    /// # Safety
    /// The caller must ensure that:
    /// 1. `bitmap` is a valid, exclusively owned mutable slice.
    /// 2. `bitmap.len() * 64 >= total_frames`.
    /// 3. The address range `[lower, upper)` is valid and mapped.
    /// 4. The zone does not overlap with other zones.
    pub unsafe fn new(
        zone_type: ZoneType,
        lower: PhysAddr,
        upper: PhysAddr,
        bitmap: &'static mut [u64],
    ) -> Result<Self, ZoneError> {
        let total_frames = (upper.as_usize() - lower.as_usize()) / S::SIZE;
        let bitmap_layout = BitmapLayout::new(bitmap, total_frames)?;

        Ok(Self {
            zone_type,
            lower_bound: lower,
            upper_bound: upper,
            bitmap: IrqSpinlock::new(bitmap_layout),
            total_frames,
            allocated_frames: AtomicUsize::new(0),
            _marker: PhantomData,
        })
    }

    /// Returns the type of this zone.
    #[inline]
    pub const fn zone_type(&self) -> ZoneType {
        self.zone_type
    }

    /// Returns the lower bound of this zone.
    #[inline]
    pub const fn lower_bound(&self) -> PhysAddr {
        self.lower_bound
    }

    /// Returns the upper bound of this zone.
    #[inline]
    pub const fn upper_bound(&self) -> PhysAddr {
        self.upper_bound
    }

    /// Returns the total number of frames in this zone.
    #[inline]
    pub const fn total_frames(&self) -> usize {
        self.total_frames
    }

    /// Returns the number of currently allocated frames.
    #[inline]
    pub fn allocated_frames(&self) -> usize {
        self.allocated_frames.load(Ordering::Relaxed)
    }

    /// Returns the number of free frames.
    #[inline]
    pub fn free_frames(&self) -> usize {
        self.total_frames.saturating_sub(self.allocated_frames())
    }

    /// Returns `true` if the given address belongs to this zone.
    #[inline]
    pub const fn contains(&self, addr: PhysAddr) -> bool {
        addr.as_usize() >= self.lower_bound.as_usize()
            && addr.as_usize() < self.upper_bound.as_usize()
    }

    /// Converts a physical address to a frame index within this zone.
    ///
    /// Returns `None` if the address is not in this zone or not aligned.
    #[inline]
    pub fn addr_to_index(&self, addr: PhysAddr) -> Option<usize> {
        if !self.contains(addr) || !addr.is_aligned::<S>() {
            return None;
        }
        Some((addr.as_usize() - self.lower_bound.as_usize()) / S::SIZE)
    }

    /// Converts a frame index to a physical address.
    ///
    /// Returns `None` if the index is out of bounds.
    #[inline]
    pub fn index_to_addr(&self, index: usize) -> Option<PhysAddr> {
        if index >= self.total_frames {
            return None;
        }
        Some(PhysAddr::new(self.lower_bound.as_usize() + index * S::SIZE))
    }

    /// Marks a range of frames as reserved (e.g., kernel image, ACPI tables).
    pub fn reserve_range(&self, start_addr: PhysAddr, count: usize) -> Result<(), ZoneError> {
        let start_index = self
            .addr_to_index(start_addr)
            .ok_or(ZoneError::AddressNotInZone { addr: start_addr })?;

        let mut bitmap = self.bitmap.lock();
        bitmap.set_range(start_index, count)?;
        self.allocated_frames
            .fetch_add(count, Ordering::Relaxed);

        Ok(())
    }

    /// Allocates a single frame from this zone.
    pub fn allocate_frame(&self) -> Result<PhysFrame<S>, ZoneError> {
        let mut bitmap = self.bitmap.lock();
        let index = bitmap.allocate_single()?;
        self.allocated_frames.fetch_add(1, Ordering::Relaxed);

        let addr = self
            .index_to_addr(index)
            .ok_or(ZoneError::OutOfMemory {
                zone_type: self.zone_type,
                requested_frames: 1,
            })?;

        PhysFrame::from_start_address(addr).map_err(|e| e.into())
    }

    /// Allocates `count` contiguous frames from this zone.
    pub fn allocate_contiguous(&self, count: usize) -> Result<PhysFrame<S>, ZoneError> {
        let mut bitmap = self.bitmap.lock();
        let start_index = bitmap.allocate_contiguous(count)?;
        self.allocated_frames
            .fetch_add(count, Ordering::Relaxed);

        let addr = self
            .index_to_addr(start_index)
            .ok_or(ZoneError::OutOfMemory {
                zone_type: self.zone_type,
                requested_frames: count,
            })?;

        PhysFrame::from_start_address(addr).map_err(|e| e.into())
    }

    /// Deallocates a frame back to this zone.
    ///
    /// # Safety
    /// The caller must ensure that the frame was originally allocated from
    /// this zone and is no longer in use.
    pub unsafe fn deallocate_frame(&self, frame: PhysFrame<S>) -> Result<(), ZoneError> {
        let addr = frame.start_address();
        let index = self
            .addr_to_index(addr)
            .ok_or(ZoneError::InvalidFrame {
                frame: addr,
                expected_zone: self.zone_type,
            })?;

        let mut bitmap = self.bitmap.lock();
        bitmap.clear(index)?;
        self.allocated_frames.fetch_sub(1, Ordering::Relaxed);

        Ok(())
    }

    /// Returns statistics for this zone.
    pub fn stats(&self) -> ZoneStats {
        let bitmap = self.bitmap.lock();
        let frag_report = bitmap.fragmentation_report();

        ZoneStats {
            zone_type: self.zone_type,
            lower_bound: self.lower_bound,
            upper_bound: self.upper_bound,
            total_frames: self.total_frames,
            allocated_frames: self.allocated_frames(),
            free_frames: self.free_frames(),
            largest_free_block: frag_report.largest_free_block,
            fragmentation_index: frag_report.fragmentation_index,
        }
    }
}

// ============================================================================
// 4. ZoneManager — manages all zones
// ============================================================================

/// Maximum number of zones supported.
const MAX_ZONES: usize = 4;

/// Manages all memory zones and provides a unified allocation interface.
///
/// The zone manager decides which zone to allocate from based on the caller's
/// requirements (e.g., DMA-capable memory) and handles fallback when a zone
/// is exhausted.
pub struct ZoneManager<S: PageSize> {
    /// Array of zones (some may be `None` if not initialized).
    zones: [Option<&'static Zone<S>>; MAX_ZONES],
    /// Number of initialized zones.
    zone_count: AtomicUsize,
    /// Whether fallback allocation is enabled.
    fallback_enabled: bool,
}

impl<S: PageSize> ZoneManager<S> {
    /// Creates a new, empty zone manager.
    #[inline]
    pub const fn new() -> Self {
        Self {
            zones: [None; MAX_ZONES],
            zone_count: AtomicUsize::new(0),
            fallback_enabled: true,
        }
    }

    /// Registers a zone with the manager.
    ///
    /// # Safety
    /// The caller must ensure that `zone` outlives the manager and remains
    /// valid for the entire duration of the system's operation.
    pub unsafe fn register_zone(&self, zone: &'static Zone<S>) -> Result<(), ZoneError> {
        let count = self.zone_count.load(Ordering::Acquire);
        if count >= MAX_ZONES {
            return Err(ZoneError::NotInitialized);
        }

        // Find the slot for this zone type.
        let slot = match zone.zone_type() {
            ZoneType::Dma => 0,
            ZoneType::Dma32 => 1,
            ZoneType::Normal => 2,
            ZoneType::HighMem => 3,
        };

        // Safety: we're using unsafe to mutate the array because we can't use
        // a lock here (the manager itself might be in a static).
        let zones_ptr = &self.zones as *const _ as *mut [Option<&'static Zone<S>>; MAX_ZONES];
        unsafe { (*zones_ptr)[slot] = Some(zone); }
        self.zone_count.fetch_add(1, Ordering::Release);

        Ok(())
    }

    /// Returns the zone for the given type, or `None` if not initialized.
    #[inline]
    pub fn get_zone(&self, zone_type: ZoneType) -> Option<&'static Zone<S>> {
        let slot = match zone_type {
            ZoneType::Dma => 0,
            ZoneType::Dma32 => 1,
            ZoneType::Normal => 2,
            ZoneType::HighMem => 3,
        };
        self.zones[slot]
    }

    /// Returns the zone that contains the given address.
    pub fn zone_for_address(&self, addr: PhysAddr) -> Option<&'static Zone<S>> {
        for zone_opt in self.zones.iter().flatten() {
            if zone_opt.contains(addr) {
                return Some(*zone_opt);
            }
        }
        None
    }

    /// Allocates a single frame from the preferred zone, with fallback.
    pub fn allocate_frame(&self, preferred: ZoneType) -> Result<PhysFrame<S>, ZoneError> {
        // Try preferred zone first.
        if let Some(zone) = self.get_zone(preferred) {
            if let Ok(frame) = zone.allocate_frame() {
                return Ok(frame);
            }
        }

        // Fallback to other zones if enabled.
        if self.fallback_enabled {
            for &fallback_type in preferred.fallback_order().iter() {
                if let Some(zone) = self.get_zone(fallback_type) {
                    if let Ok(frame) = zone.allocate_frame() {
                        // TODO: Log warning about fallback allocation.
                        return Ok(frame);
                    }
                }
            }
        }

        Err(ZoneError::OutOfMemory {
            zone_type: preferred,
            requested_frames: 1,
        })
    }

    /// Allocates `count` contiguous frames from the preferred zone, with fallback.
    pub fn allocate_contiguous(
        &self,
        count: usize,
        preferred: ZoneType,
    ) -> Result<PhysFrame<S>, ZoneError> {
        // Try preferred zone first.
        if let Some(zone) = self.get_zone(preferred) {
            if let Ok(frame) = zone.allocate_contiguous(count) {
                return Ok(frame);
            }
        }

        // Fallback to other zones if enabled.
        if self.fallback_enabled {
            for &fallback_type in preferred.fallback_order().iter() {
                if let Some(zone) = self.get_zone(fallback_type) {
                    if let Ok(frame) = zone.allocate_contiguous(count) {
                        // TODO: Log warning about fallback allocation.
                        return Ok(frame);
                    }
                }
            }
        }

        Err(ZoneError::OutOfMemory {
            zone_type: preferred,
            requested_frames: count,
        })
    }

    /// Deallocates a frame back to its originating zone.
    ///
    /// # Safety
    /// The caller must ensure that the frame was originally allocated from
    /// this manager and is no longer in use.
    pub unsafe fn deallocate_frame(&self, frame: PhysFrame<S>) -> Result<(), ZoneError> {
        let addr = frame.start_address();
        let zone = self
            .zone_for_address(addr)
            .ok_or(ZoneError::AddressNotInZone { addr })?;

        unsafe { zone.deallocate_frame(frame) }
    }

    /// Enables or disables fallback allocation.
    #[inline]
    pub fn set_fallback_enabled(&mut self, enabled: bool) {
        self.fallback_enabled = enabled;
    }

    /// Returns statistics for all zones.
    pub fn stats(&self) -> ZoneManagerStats {
        let mut zone_stats = [None; MAX_ZONES];
        let mut count = 0;

        for (i, zone_opt) in self.zones.iter().enumerate() {
            if let Some(zone) = zone_opt {
                zone_stats[i] = Some(zone.stats());
                count += 1;
            }
        }

        ZoneManagerStats {
            zone_stats,
            zone_count: count,
            fallback_enabled: self.fallback_enabled,
        }
    }
}

// ============================================================================
// 5. Statistics
// ============================================================================

/// Statistics for a single zone.
#[derive(Debug, Clone, Copy)]
pub struct ZoneStats {
    pub zone_type: ZoneType,
    pub lower_bound: PhysAddr,
    pub upper_bound: PhysAddr,
    pub total_frames: usize,
    pub allocated_frames: usize,
    pub free_frames: usize,
    pub largest_free_block: usize,
    pub fragmentation_index: f64,
}

impl fmt::Display for ZoneStats {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "Zone: {}", self.zone_type.name())?;
        writeln!(
            f,
            "  Range: [{:#x}..{:#x})",
            self.lower_bound, self.upper_bound
        )?;
        writeln!(f, "  Total frames:     {}", self.total_frames)?;
        writeln!(f, "  Allocated frames: {}", self.allocated_frames)?;
        writeln!(f, "  Free frames:      {}", self.free_frames)?;
        writeln!(f, "  Largest free:     {}", self.largest_free_block)?;
        writeln!(f, "  Fragmentation:    {:.4}", self.fragmentation_index)
    }
}

/// Statistics for the entire zone manager.
#[derive(Debug, Clone)]
pub struct ZoneManagerStats {
    pub zone_stats: [Option<ZoneStats>; MAX_ZONES],
    pub zone_count: usize,
    pub fallback_enabled: bool,
}

impl fmt::Display for ZoneManagerStats {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "=== CoTanOS Zone Manager Stats ===")?;
        writeln!(f, "Zones: {}", self.zone_count)?;
        writeln!(f, "Fallback: {}", if self.fallback_enabled { "enabled" } else { "disabled" })?;
        writeln!(f)?;

        for zone_opt in self.zone_stats.iter().flatten() {
            writeln!(f, "{}", zone_opt)?;
        }
        Ok(())
    }
}

// ============================================================================
// 6. Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_zone_type_bounds() {
        assert_eq!(ZoneType::Dma.lower_bound(), 0);
        assert_eq!(ZoneType::Dma.upper_bound(), 0x0100_0000);
        assert_eq!(ZoneType::Dma32.lower_bound(), 0x0100_0000);
        assert_eq!(ZoneType::Dma32.upper_bound(), 0x1_0000_0000);
    }

    #[test]
    fn test_zone_type_contains() {
        assert!(ZoneType::Dma.contains(PhysAddr::new(0x0000_1000)));
        assert!(!ZoneType::Dma.contains(PhysAddr::new(0x0100_0000)));
        assert!(ZoneType::Dma32.contains(PhysAddr::new(0x0100_0000)));
        assert!(ZoneType::Dma32.contains(PhysAddr::new(0xFFFF_FFFF)));
        assert!(!ZoneType::Dma32.contains(PhysAddr::new(0x1_0000_0000)));
    }

    #[test]
    fn test_zone_fallback_order() {
        let order = ZoneType::Dma.fallback_order();
        assert_eq!(order[0], ZoneType::Dma32);
        assert_eq!(order[1], ZoneType::Normal);
        assert_eq!(order[2], ZoneType::HighMem);
    }
}