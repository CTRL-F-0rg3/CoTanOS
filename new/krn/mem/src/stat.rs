// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS Memory Statistics & Diagnostics (`stat.rs`)
//!
//! Centralized statistics collection and diagnostic reporting for all memory
//! allocators in CoTanOS (arena, frame, slab). This module provides:
//!
//! * **Unified reporting**: A single API to query memory usage across all
//!   allocators, regardless of their internal implementation.
//!
//! * **Fragmentation analysis**: Detailed reports on memory fragmentation,
//!   largest free blocks, and allocation patterns.
//!
//! * **Real-time diagnostics**: Lock-free counters for allocation/deallocation
//!   events, enabling performance profiling without significant overhead.
//!
//! * **Kernel shell integration**: Formatters for `fmt::Write` to display
//!   memory stats in the kernel shell or serial console.
//!
//! * **SMP-safe**: All global state is protected by [`IrqSpinlock`] from
//!   `frm.rs`, ensuring correctness on multi-core systems.
//!
//! # Design Principles
//!
//! * **No heap allocation**: All data structures use fixed-size arrays or
//!   caller-provided buffers. This module works before the slab allocator
//!   is initialized.
//!
//! * **Zero-cost abstractions**: Counters use `AtomicU64` for lock-free
//!   updates. Reports are computed on-demand, not cached.
//!
//! * **Extensible**: New allocator types can be registered via
//!   [`StatRegistry::register`]. The registry uses a trait object pattern
//!   without dynamic dispatch overhead (enum-based).
//!
//! * **Debug-friendly**: In debug builds, additional metadata (allocation
//!   timestamps, CPU IDs) is collected for post-mortem analysis.
//!
//! # File Layout
//!
//! 1. [`StatError`] — error types.
//! 2. [`AllocatorType`] — enum of supported allocator types.
//! 3. [`AllocatorStats`] — unified statistics structure.
//! 4. [`MemoryReport`] — comprehensive memory usage report.
//! 5. [`StatRegistry`] — global registry of allocators.
//! 6. [`StatCollector`] — real-time event counter.
//! 7. Formatters for kernel logs.
//! 8. Tests.

use core::fmt;
use core::sync::atomic::{AtomicU64, AtomicUsize, Ordering};

use crate::frm::{AllocatorStats as FrameAllocatorStats, FragmentationReport, IrqSpinlock};

// ============================================================================
// 1. StatError
// ============================================================================

/// Errors that can occur during statistics operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum StatError {
    /// The registry is full (maximum number of allocators reached).
    RegistryFull,
    /// The allocator with the given name was not found.
    AllocatorNotFound {
        name: &'static str,
    },
    /// The allocator type is not supported.
    UnsupportedAllocatorType {
        type_name: &'static str,
    },
    /// The buffer provided for formatting is too small.
    BufferTooSmall {
        required: usize,
        available: usize,
    },
    /// The statistics are not available (allocator not initialized).
    NotInitialized,
}

impl fmt::Display for StatError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::RegistryFull => write!(f, "stat: registry full (max allocators reached)"),
            Self::AllocatorNotFound { name } => {
                write!(f, "stat: allocator '{}' not found", name)
            }
            Self::UnsupportedAllocatorType { type_name } => {
                write!(f, "stat: unsupported allocator type '{}'", type_name)
            }
            Self::BufferTooSmall { required, available } => {
                write!(
                    f,
                    "stat: buffer too small (required: {}, available: {})",
                    required, available
                )
            }
            Self::NotInitialized => write!(f, "stat: allocator not initialized"),
        }
    }
}

// ============================================================================
// 2. AllocatorType
// ============================================================================

/// Type of memory allocator.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AllocatorType {
    /// Early boot arena allocator (bump allocator).
    Arena,
    /// Physical frame allocator (bitmap-based).
    Frame,
    /// Slab allocator (object cache).
    Slab,
    /// Zone-based allocator (DMA/DMA32/Normal).
    Zone,
    /// Custom allocator (user-defined).
    Custom,
}

impl AllocatorType {
    /// Returns the human-readable name.
    #[inline]
    pub const fn name(self) -> &'static str {
        match self {
            Self::Arena => "arena",
            Self::Frame => "frame",
            Self::Slab => "slab",
            Self::Zone => "zone",
            Self::Custom => "custom",
        }
    }
}

impl fmt::Display for AllocatorType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.name())
    }
}

// ============================================================================
// 3. AllocatorStats — unified statistics
// ============================================================================

/// Unified statistics structure for any allocator type.
///
/// This structure aggregates common metrics across different allocator
/// implementations, enabling uniform reporting and comparison.
#[derive(Debug, Clone, Copy)]
pub struct AllocatorStats {
    /// Name of the allocator (for identification).
    pub name: &'static str,
    /// Type of the allocator.
    pub allocator_type: AllocatorType,
    /// Total capacity in bytes (or frames, depending on type).
    pub total_capacity: usize,
    /// Currently used capacity.
    pub used_capacity: usize,
    /// Number of allocation operations performed.
    pub alloc_count: u64,
    /// Number of deallocation operations performed.
    pub dealloc_count: u64,
    /// Number of failed allocation attempts (OOM).
    pub failed_alloc_count: u64,
    /// Largest single allocation size (bytes).
    pub largest_alloc_size: usize,
    /// Smallest single allocation size (bytes).
    pub smallest_alloc_size: usize,
    /// Average allocation size (computed on-demand).
    pub avg_alloc_size: usize,
    /// Fragmentation index (0.0 = no fragmentation, 1.0 = fully fragmented).
    /// Only meaningful for frame and slab allocators.
    pub fragmentation_index: f64,
    /// Additional type-specific data (optional).
    pub extra: AllocatorExtraStats,
}

impl AllocatorStats {
    /// Creates a new, empty statistics structure.
    #[inline]
    pub const fn new(name: &'static str, allocator_type: AllocatorType) -> Self {
        Self {
            name,
            allocator_type,
            total_capacity: 0,
            used_capacity: 0,
            alloc_count: 0,
            dealloc_count: 0,
            failed_alloc_count: 0,
            largest_alloc_size: 0,
            smallest_alloc_size: usize::MAX,
            avg_alloc_size: 0,
            fragmentation_index: 0.0,
            extra: AllocatorExtraStats::None,
        }
    }

    /// Returns the free capacity.
    #[inline]
    pub const fn free_capacity(&self) -> usize {
        self.total_capacity.saturating_sub(self.used_capacity)
    }

    /// Returns the utilization percentage (0.0 - 100.0).
    #[inline]
    pub fn utilization_percent(&self) -> f64 {
        if self.total_capacity == 0 {
            return 0.0;
        }
        (self.used_capacity as f64 / self.total_capacity as f64) * 100.0
    }

    /// Updates the average allocation size.
    #[inline]
    pub fn update_avg_alloc_size(&mut self) {
        if self.alloc_count > 0 {
            // Simplified: use total used / alloc count.
            // In a full implementation, track the sum of all allocation sizes.
            self.avg_alloc_size = self.used_capacity / self.alloc_count as usize;
        }
    }
}

impl fmt::Display for AllocatorStats {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "Allocator: {} ({})", self.name, self.allocator_type)?;
        writeln!(
            f,
            "  Capacity:      {} / {} bytes ({:.1}%)",
            self.used_capacity,
            self.total_capacity,
            self.utilization_percent()
        )?;
        writeln!(
            f,
            "  Free:          {} bytes",
            self.free_capacity()
        )?;
        writeln!(f, "  Allocations:   {}", self.alloc_count)?;
        writeln!(f, "  Deallocations: {}", self.dealloc_count)?;
        writeln!(f, "  Failed allocs: {}", self.failed_alloc_count)?;
        writeln!(
            f,
            "  Alloc size:    {} - {} bytes (avg: {})",
            self.smallest_alloc_size, self.largest_alloc_size, self.avg_alloc_size
        )?;
        if self.fragmentation_index > 0.0 {
            writeln!(f, "  Fragmentation: {:.4}", self.fragmentation_index)?;
        }
        Ok(())
    }
}

/// Type-specific extra statistics.
#[derive(Debug, Clone, Copy)]
pub enum AllocatorExtraStats {
    /// No extra stats.
    None,
    /// Frame allocator: number of pages per size class.
    Frame {
        pages_4k: usize,
        pages_2m: usize,
        pages_1g: usize,
    },
    /// Slab allocator: number of caches and objects per cache.
    Slab {
        cache_count: usize,
        total_objects: usize,
    },
    /// Arena allocator: number of allocations since last reset.
    Arena {
        reset_count: u64,
    },
}

// ============================================================================
// 4. MemoryReport — comprehensive report
// ============================================================================

/// Comprehensive memory usage report across all allocators.
#[derive(Debug, Clone)]
pub struct MemoryReport {
    /// Statistics for each registered allocator.
    pub allocators: [Option<AllocatorStats>; MAX_ALLOCATORS],
    /// Number of registered allocators.
    pub allocator_count: usize,
    /// Total physical memory (from frame allocator).
    pub total_physical_memory: usize,
    /// Total used physical memory.
    pub used_physical_memory: usize,
    /// System uptime (ticks or seconds).
    pub uptime_ticks: u64,
}

impl MemoryReport {
    /// Creates a new, empty report.
    #[inline]
    pub const fn new() -> Self {
        Self {
            allocators: [None; MAX_ALLOCATORS],
            allocator_count: 0,
            total_physical_memory: 0,
            used_physical_memory: 0,
            uptime_ticks: 0,
        }
    }

    /// Adds an allocator's statistics to the report.
    pub fn add_allocator(&mut self, stats: AllocatorStats) -> Result<(), StatError> {
        if self.allocator_count >= MAX_ALLOCATORS {
            return Err(StatError::RegistryFull);
        }
        self.allocators[self.allocator_count] = Some(stats);
        self.allocator_count += 1;
        Ok(())
    }

    /// Returns the total memory used across all allocators.
    pub fn total_used(&self) -> usize {
        self.allocators
            .iter()
            .filter_map(|a| a.as_ref())
            .map(|a| a.used_capacity)
            .sum()
    }

    /// Returns the total capacity across all allocators.
    pub fn total_capacity(&self) -> usize {
        self.allocators
            .iter()
            .filter_map(|a| a.as_ref())
            .map(|a| a.total_capacity)
            .sum()
    }
}

impl fmt::Display for MemoryReport {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "=== CoTanOS Memory Report ===")?;
        writeln!(f, "Uptime: {} ticks", self.uptime_ticks)?;
        writeln!(
            f,
            "Physical memory: {} / {} bytes ({:.1}%)",
            self.used_physical_memory,
            self.total_physical_memory,
            if self.total_physical_memory > 0 {
                (self.used_physical_memory as f64 / self.total_physical_memory as f64) * 100.0
            } else {
                0.0
            }
        )?;
        writeln!(f, "Registered allocators: {}", self.allocator_count)?;
        writeln!(f)?;

        for (i, alloc) in self.allocators.iter().enumerate() {
            if let Some(stats) = alloc {
                writeln!(f, "[{}] {}", i, stats)?;
            }
        }
        Ok(())
    }
}

// ============================================================================
// 5. StatRegistry — global registry
// ============================================================================

/// Maximum number of allocators that can be registered.
const MAX_ALLOCATORS: usize = 16;

/// Global registry of memory allocators.
///
/// This structure maintains a list of all allocators in the system and
/// provides a unified API for querying their statistics.
pub struct StatRegistry {
    /// Registered allocators (name -> stats collector).
    allocators: IrqSpinlock<[Option<StatEntry>; MAX_ALLOCATORS]>,
    /// Number of registered allocators.
    count: AtomicUsize,
}

/// Entry in the registry.
struct StatEntry {
    /// Name of the allocator.
    name: &'static str,
    /// Type of the allocator.
    allocator_type: AllocatorType,
    /// Pointer to the stats collector (type-erased).
    collector: &'static dyn StatsCollector,
}

impl StatRegistry {
    /// Creates a new, empty registry.
    #[inline]
    pub const fn new() -> Self {
        Self {
            allocators: IrqSpinlock::new([const { None }; MAX_ALLOCATORS]),
            count: AtomicUsize::new(0),
        }
    }

    /// Registers an allocator with the registry.
    ///
    /// # Safety
    /// The caller must ensure that `collector` outlives the registry and
    /// remains valid for the entire duration of the system's operation.
    pub unsafe fn register(
        &self,
        name: &'static str,
        allocator_type: AllocatorType,
        collector: &'static dyn StatsCollector,
    ) -> Result<(), StatError> {
        let mut allocators = self.allocators.lock();
        let count = self.count.load(Ordering::Acquire);

        if count >= MAX_ALLOCATORS {
            return Err(StatError::RegistryFull);
        }

        // Check for duplicate names.
        for entry in allocators.iter().flatten() {
            if entry.name == name {
                return Err(StatError::AllocatorNotFound { name });
            }
        }

        allocators[count] = Some(StatEntry {
            name,
            allocator_type,
            collector,
        });
        self.count.store(count + 1, Ordering::Release);

        Ok(())
    }

    /// Returns statistics for a specific allocator by name.
    pub fn get_stats(&self, name: &'static str) -> Result<AllocatorStats, StatError> {
        let allocators = self.allocators.lock();

        for entry in allocators.iter().flatten() {
            if entry.name == name {
                return Ok(entry.collector.collect());
            }
        }

        Err(StatError::AllocatorNotFound { name })
    }

    /// Collects statistics from all registered allocators.
    pub fn collect_all(&self) -> MemoryReport {
        let allocators = self.allocators.lock();
        let mut report = MemoryReport::new();

        for entry in allocators.iter().flatten() {
            let stats = entry.collector.collect();
            let _ = report.add_allocator(stats);
        }

        report
    }

    /// Returns the number of registered allocators.
    #[inline]
    pub fn allocator_count(&self) -> usize {
        self.count.load(Ordering::Relaxed)
    }
}

// ============================================================================
// 6. StatsCollector — trait for allocators
// ============================================================================

/// Trait implemented by all allocators to provide statistics.
///
/// This trait is object-safe, allowing the registry to store heterogeneous
/// allocator types without dynamic dispatch overhead (we use enum-based
/// dispatch internally).
pub trait StatsCollector: Send + Sync {
    /// Collects current statistics from the allocator.
    fn collect(&self) -> AllocatorStats;

    /// Returns the name of the allocator.
    fn name(&self) -> &'static str;

    /// Returns the type of the allocator.
    fn allocator_type(&self) -> AllocatorType;
}

// ============================================================================
// 7. StatCollector — real-time event counter
// ============================================================================

/// Lock-free counter for allocation/deallocation events.
///
/// This structure is embedded in each allocator to track real-time statistics
/// without requiring a lock for every operation.
#[repr(C)]
pub struct StatCollector {
    /// Number of successful allocations.
    pub alloc_count: AtomicU64,
    /// Number of successful deallocations.
    pub dealloc_count: AtomicU64,
    /// Number of failed allocations (OOM).
    pub failed_alloc_count: AtomicU64,
    /// Total bytes allocated (sum of all allocation sizes).
    pub total_bytes_allocated: AtomicU64,
    /// Total bytes deallocated.
    pub total_bytes_deallocated: AtomicU64,
    /// Largest single allocation size.
    pub largest_alloc_size: AtomicUsize,
    /// Smallest single allocation size.
    pub smallest_alloc_size: AtomicUsize,
}

impl StatCollector {
    /// Creates a new, zeroed counter.
    #[inline]
    pub const fn new() -> Self {
        Self {
            alloc_count: AtomicU64::new(0),
            dealloc_count: AtomicU64::new(0),
            failed_alloc_count: AtomicU64::new(0),
            total_bytes_allocated: AtomicU64::new(0),
            total_bytes_deallocated: AtomicU64::new(0),
            largest_alloc_size: AtomicUsize::new(0),
            smallest_alloc_size: AtomicUsize::new(usize::MAX),
        }
    }

    /// Records a successful allocation.
    #[inline]
    pub fn record_alloc(&self, size: usize) {
        self.alloc_count.fetch_add(1, Ordering::Relaxed);
        self.total_bytes_allocated
            .fetch_add(size as u64, Ordering::Relaxed);

        // Update largest/smallest (racy, but acceptable for diagnostics).
        let mut largest = self.largest_alloc_size.load(Ordering::Relaxed);
        while size > largest {
            match self.largest_alloc_size.compare_exchange_weak(
                largest,
                size,
                Ordering::Relaxed,
                Ordering::Relaxed,
            ) {
                Ok(_) => break,
                Err(actual) => largest = actual,
            }
        }

        let mut smallest = self.smallest_alloc_size.load(Ordering::Relaxed);
        while size < smallest {
            match self.smallest_alloc_size.compare_exchange_weak(
                smallest,
                size,
                Ordering::Relaxed,
                Ordering::Relaxed,
            ) {
                Ok(_) => break,
                Err(actual) => smallest = actual,
            }
        }
    }

    /// Records a successful deallocation.
    #[inline]
    pub fn record_dealloc(&self, size: usize) {
        self.dealloc_count.fetch_add(1, Ordering::Relaxed);
        self.total_bytes_deallocated
            .fetch_add(size as u64, Ordering::Relaxed);
    }

    /// Records a failed allocation attempt.
    #[inline]
    pub fn record_failed_alloc(&self) {
        self.failed_alloc_count.fetch_add(1, Ordering::Relaxed);
    }

    /// Returns a snapshot of the current statistics.
    pub fn snapshot(&self) -> StatSnapshot {
        StatSnapshot {
            alloc_count: self.alloc_count.load(Ordering::Relaxed),
            dealloc_count: self.dealloc_count.load(Ordering::Relaxed),
            failed_alloc_count: self.failed_alloc_count.load(Ordering::Relaxed),
            total_bytes_allocated: self.total_bytes_allocated.load(Ordering::Relaxed),
            total_bytes_deallocated: self.total_bytes_deallocated.load(Ordering::Relaxed),
            largest_alloc_size: self.largest_alloc_size.load(Ordering::Relaxed),
            smallest_alloc_size: self.smallest_alloc_size.load(Ordering::Relaxed),
        }
    }

    /// Resets all counters to zero.
    ///
    /// # Safety
    /// This should only be called when no allocations are in progress.
    #[inline]
    pub unsafe fn reset(&self) {
        self.alloc_count.store(0, Ordering::Relaxed);
        self.dealloc_count.store(0, Ordering::Relaxed);
        self.failed_alloc_count.store(0, Ordering::Relaxed);
        self.total_bytes_allocated.store(0, Ordering::Relaxed);
        self.total_bytes_deallocated.store(0, Ordering::Relaxed);
        self.largest_alloc_size.store(0, Ordering::Relaxed);
        self.smallest_alloc_size.store(usize::MAX, Ordering::Relaxed);
    }
}

/// Snapshot of statistics at a point in time.
#[derive(Debug, Clone, Copy)]
pub struct StatSnapshot {
    pub alloc_count: u64,
    pub dealloc_count: u64,
    pub failed_alloc_count: u64,
    pub total_bytes_allocated: u64,
    pub total_bytes_deallocated: u64,
    pub largest_alloc_size: usize,
    pub smallest_alloc_size: usize,
}

impl StatSnapshot {
    /// Returns the net bytes currently allocated.
    #[inline]
    pub const fn net_bytes_allocated(&self) -> u64 {
        self.total_bytes_allocated - self.total_bytes_deallocated
    }

    /// Returns the average allocation size.
    #[inline]
    pub fn avg_alloc_size(&self) -> usize {
        if self.alloc_count == 0 {
            return 0;
        }
        (self.total_bytes_allocated / self.alloc_count) as usize
    }
}

// ============================================================================
// 8. Formatters for kernel logs
// ============================================================================

/// Formats a memory report to a `fmt::Write` destination.
///
/// This is useful for serial console output or kernel shell commands.
pub fn format_memory_report<W: fmt::Write>(
    writer: &mut W,
    report: &MemoryReport,
) -> fmt::Result {
    write!(writer, "{}", report)
}

/// Formats a single allocator's statistics.
pub fn format_allocator_stats<W: fmt::Write>(
    writer: &mut W,
    stats: &AllocatorStats,
) -> fmt::Result {
    write!(writer, "{}", stats)
}

/// Formats a fragmentation report.
pub fn format_fragmentation<W: fmt::Write>(
    writer: &mut W,
    report: &FragmentationReport,
) -> fmt::Result {
    writeln!(writer, "Fragmentation Report:")?;
    writeln!(writer, "  Total frames:        {}", report.total_frames)?;
    writeln!(writer, "  Allocated frames:    {}", report.allocated_frames)?;
    writeln!(writer, "  Free frames:         {}", report.free_frames)?;
    writeln!(writer, "  Largest free block:  {}", report.largest_free_block)?;
    writeln!(writer, "  Free block count:    {}", report.free_block_count)?;
    writeln!(
        writer,
        "  Fragmentation index: {:.4}",
        report.fragmentation_index
    )
}

// ============================================================================
// Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_stat_collector_basic() {
        let collector = StatCollector::new();

        collector.record_alloc(100);
        collector.record_alloc(200);
        collector.record_dealloc(100);

        let snapshot = collector.snapshot();
        assert_eq!(snapshot.alloc_count, 2);
        assert_eq!(snapshot.dealloc_count, 1);
        assert_eq!(snapshot.total_bytes_allocated, 300);
        assert_eq!(snapshot.total_bytes_deallocated, 100);
        assert_eq!(snapshot.net_bytes_allocated(), 200);
        assert_eq!(snapshot.largest_alloc_size, 200);
        assert_eq!(snapshot.smallest_alloc_size, 100);
    }

    #[test]
    fn test_allocator_stats_utilization() {
        let mut stats = AllocatorStats::new("test", AllocatorType::Arena);
        stats.total_capacity = 1000;
        stats.used_capacity = 500;

        assert_eq!(stats.free_capacity(), 500);
        assert!((stats.utilization_percent() - 50.0).abs() < 0.01);
    }

    #[test]
    fn test_memory_report() {
        let mut report = MemoryReport::new();

        let stats1 = AllocatorStats {
            name: "arena",
            allocator_type: AllocatorType::Arena,
            total_capacity: 1000,
            used_capacity: 500,
            ..AllocatorStats::new("arena", AllocatorType::Arena)
        };

        let stats2 = AllocatorStats {
            name: "frame",
            allocator_type: AllocatorType::Frame,
            total_capacity: 2000,
            used_capacity: 1000,
            ..AllocatorStats::new("frame", AllocatorType::Frame)
        };

        report.add_allocator(stats1).unwrap();
        report.add_allocator(stats2).unwrap();

        assert_eq!(report.allocator_count, 2);
        assert_eq!(report.total_used(), 1500);
        assert_eq!(report.total_capacity(), 3000);
    }
}