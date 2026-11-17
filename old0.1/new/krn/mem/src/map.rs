// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS Physical Frame Bitmap Engine (`map.rs`)
//!
//! This module implements the core bitmap data structure used by the
//! [`PhysicalFrameAllocator`](crate::frm::PhysicalFrameAllocator) to track
//! which physical frames are free and which are allocated.
//!
//! # Design Principles
//!
//! * **Word-oriented**: The bitmap is stored as `&mut [u64]`, not bytes.
//!   64-bit words allow us to scan 64 frames at once using hardware-efficient
//!   operations like `trailing_zeros`, `count_ones`, and bitwise arithmetic.
//!
//! * **Zero = Free, One = Allocated**: This convention makes it trivial to
//!   check if a frame is allocated (`word & (1 << bit) != 0`) and to find
//!   free frames (`!word` has zeros where frames are free).
//!
//! * **Checked arithmetic everywhere**: All operations that could overflow
//!   or go out of bounds return `Result` or `Option`. The bitmap engine
//!   never panics on bad input (corrupt firmware tables, double-frees, etc.).
//!
//! * **Contiguous allocation support**: Beyond single-frame allocation, the
//!   bitmap can find N consecutive free frames in O(total_bits / 64) time,
//!   which is critical for DMA buffers and large page mappings.
//!
//! * **No heap allocation**: The bitmap operates on a caller-provided
//!   `&'static mut [u64]` slice, typically allocated from the early boot
//!   arena. This keeps the allocator self-contained and usable before the
//!   heap is initialized.
//!
//! # Performance Characteristics
//!
//! * **Single-frame allocation**: O(1) amortized with `next_free_hint`,
//!   O(total_bits / 64) worst-case.
//! * **Contiguous N-frame allocation**: O(total_bits / 64) with early exit
//!   optimizations (skipping fully-allocated words).
//! * **Deallocation**: O(1).
//! * **Range operations**: O(range_length / 64).

use core::fmt;


// ============================================================================
// Constants & Poison Patterns
// ============================================================================

/// Poison pattern written to freed frames to detect use-after-free bugs.
/// This is a debug-only feature; in release builds, freed memory is left as-is
/// for performance.
#[cfg(debug_assertions)]
const POISON_PATTERN: u8 = 0xDE;

/// Word-level poison pattern (8 bytes of 0xDE).
#[cfg(debug_assertions)]
const POISON_WORD: u64 = 0xDEDE_DEDE_DEDE_DEDE;

// ============================================================================
// BitmapError
// ============================================================================

/// Errors that can occur during bitmap operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BitmapError {
    /// The requested frame index is out of bounds.
    OutOfBounds {
        index: usize,
        total_bits: usize,
    },
    /// The requested range [start, start+count) is out of bounds.
    RangeOutOfBounds {
        start: usize,
        count: usize,
        total_bits: usize,
    },
    /// The frame at the given index is already allocated (for allocate)
    /// or already free (for deallocate).
    InvalidState {
        index: usize,
        expected: &'static str,
        actual: &'static str,
    },
    /// No contiguous block of the requested size could be found.
    NoContiguousBlock {
        requested: usize,
        largest_available: usize,
    },
    /// The bitmap layout is invalid (e.g., zero total bits).
    InvalidLayout {
        reason: &'static str,
    },
}

impl fmt::Display for BitmapError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::OutOfBounds { index, total_bits } => {
                write!(f, "bitmap index {} out of bounds (total: {})", index, total_bits)
            }
            Self::RangeOutOfBounds { start, count, total_bits } => {
                write!(
                    f,
                    "bitmap range [{}..{}) out of bounds (total: {})",
                    start,
                    start + count,
                    total_bits
                )
            }
            Self::InvalidState { index, expected, actual } => {
                write!(
                    f,
                    "bitmap[{}] invalid state: expected {}, got {}",
                    index, expected, actual
                )
            }
            Self::NoContiguousBlock { requested, largest_available } => {
                write!(
                    f,
                    "no contiguous block of {} frames (largest available: {})",
                    requested, largest_available
                )
            }
            Self::InvalidLayout { reason } => {
                write!(f, "invalid bitmap layout: {}", reason)
            }
        }
    }
}

// ============================================================================
// BitmapLayout
// ============================================================================

/// Describes the layout of a physical frame bitmap.
///
/// The bitmap is stored as an array of 64-bit words, where each bit represents
/// one physical frame. Bit 0 of word 0 represents frame 0, bit 1 represents
/// frame 1, and so on.
///
/// # Invariants
///
/// * `total_bits > 0`
/// * `words.len() * 64 >= total_bits`
/// * The bitmap may have "padding bits" beyond `total_bits` (if `total_bits`
///   is not a multiple of 64). These padding bits are always marked as
///   allocated (1) to prevent accidental use.
pub struct BitmapLayout {
    /// The backing storage for the bitmap.
    words: &'static mut [u64],
    /// Total number of valid bits (frames) in the bitmap.
    total_bits: usize,
    /// Number of bits currently marked as allocated (1).
    allocated_count: usize,
    /// Optimization hint: the last known free bit index.
    /// This speeds up sequential allocations by avoiding scanning from 0.
    next_free_hint: usize,
    /// Cached value of the largest contiguous free block (for fast OOM checks).
    largest_free_block: usize,
}

impl BitmapLayout {
    /// Creates a new bitmap layout from a caller-provided word array.
    ///
    /// # Safety
    ///
    /// The caller must guarantee that:
    /// 1. `words` is a valid, exclusively owned mutable slice.
    /// 2. `words.len() * 64 >= total_bits`.
    /// 3. The memory backing `words` is not concurrently accessed by other
    ///    code (hardware, firmware, other allocators).
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::InvalidLayout`] if `total_bits == 0` or if
    /// `words` is too small to hold `total_bits` bits.
    pub unsafe fn new(
        words: &'static mut [u64],
        total_bits: usize,
    ) -> Result<Self, BitmapError> {
        if total_bits == 0 {
            return Err(BitmapError::InvalidLayout {
                reason: "total_bits must be > 0",
            });
        }

        let required_words = (total_bits + 63) / 64;
        if words.len() < required_words {
            return Err(BitmapError::InvalidLayout {
                reason: "words array too small for total_bits",
            });
        }

        // Initialize all bits to 0 (free), then mark padding bits as allocated.
        for word in words.iter_mut() {
            *word = 0;
        }

        // Mark padding bits (beyond total_bits) as allocated to prevent use.
        let padding_start = total_bits;
        let padding_end = words.len() * 64;
        if padding_start < padding_end {
            for i in padding_start..padding_end {
                let word_idx = i / 64;
                let bit_idx = i % 64;
                words[word_idx] |= 1u64 << bit_idx;
            }
        }

        Ok(Self {
            words,
            total_bits,
            allocated_count: 0,
            next_free_hint: 0,
            largest_free_block: total_bits, // Initially, all frames are free.
        })
    }

    /// Returns the total number of frames (bits) in the bitmap.
    #[inline]
    pub const fn total_bits(&self) -> usize {
        self.total_bits
    }

    /// Returns the number of frames currently marked as allocated.
    #[inline]
    pub const fn allocated_count(&self) -> usize {
        self.allocated_count
    }

    /// Returns the number of frames currently marked as free.
    #[inline]
    pub const fn free_count(&self) -> usize {
        self.total_bits - self.allocated_count
    }

    /// Returns the number of 64-bit words in the bitmap.
    #[inline]
    pub const fn word_count(&self) -> usize {
        self.words.len()
    }

    // ========================================================================
    // Single-bit operations
    // ========================================================================

    /// Returns `true` if the bit at `index` is set (allocated).
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::OutOfBounds`] if `index >= total_bits`.
    #[inline]
    pub fn test(&self, index: usize) -> Result<bool, BitmapError> {
        if index >= self.total_bits {
            return Err(BitmapError::OutOfBounds {
                index,
                total_bits: self.total_bits,
            });
        }
        let word_idx = index / 64;
        let bit_idx = index % 64;
        Ok((self.words[word_idx] & (1u64 << bit_idx)) != 0)
    }

    /// Sets the bit at `index` to 1 (marks the frame as allocated).
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::OutOfBounds`] if `index >= total_bits`.
    /// Returns [`BitmapError::InvalidState`] if the bit is already set.
    #[inline]
    pub fn set(&mut self, index: usize) -> Result<(), BitmapError> {
        if index >= self.total_bits {
            return Err(BitmapError::OutOfBounds {
                index,
                total_bits: self.total_bits,
            });
        }

        let word_idx = index / 64;
        let bit_idx = index % 64;
        let mask = 1u64 << bit_idx;

        if (self.words[word_idx] & mask) != 0 {
            return Err(BitmapError::InvalidState {
                index,
                expected: "free (0)",
                actual: "allocated (1)",
            });
        }

        self.words[word_idx] |= mask;
        self.allocated_count += 1;

        // Update hint: if we just allocated the hinted frame, advance the hint.
        if index == self.next_free_hint {
            self.next_free_hint = self.find_next_free_from(index + 1).unwrap_or(index);
        }

        Ok(())
    }

    /// Clears the bit at `index` to 0 (marks the frame as free).
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::OutOfBounds`] if `index >= total_bits`.
    /// Returns [`BitmapError::InvalidState`] if the bit is already clear.
    #[inline]
    pub fn clear(&mut self, index: usize) -> Result<(), BitmapError> {
        if index >= self.total_bits {
            return Err(BitmapError::OutOfBounds {
                index,
                total_bits: self.total_bits,
            });
        }

        let word_idx = index / 64;
        let bit_idx = index % 64;
        let mask = 1u64 << bit_idx;

        if (self.words[word_idx] & mask) == 0 {
            return Err(BitmapError::InvalidState {
                index,
                expected: "allocated (1)",
                actual: "free (0)",
            });
        }

        self.words[word_idx] &= !mask;
        self.allocated_count -= 1;

        // Update hint: if we just freed a frame before the hint, move the hint back.
        if index < self.next_free_hint {
            self.next_free_hint = index;
        }

        Ok(())
    }

    // ========================================================================
    // Range operations
    // ========================================================================

    /// Sets all bits in the range `[start, start + count)` to 1 (allocated).
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::RangeOutOfBounds`] if the range exceeds `total_bits`.
    /// Returns [`BitmapError::InvalidState`] if any bit in the range is already set.
    pub fn set_range(&mut self, start: usize, count: usize) -> Result<(), BitmapError> {
        if start >= self.total_bits || start + count > self.total_bits {
            return Err(BitmapError::RangeOutOfBounds {
                start,
                count,
                total_bits: self.total_bits,
            });
        }

        // Check all bits first to avoid partial updates on error.
        for i in start..start + count {
            if self.test(i)? {
                return Err(BitmapError::InvalidState {
                    index: i,
                    expected: "free (0)",
                    actual: "allocated (1)",
                });
            }
        }

        // Now set all bits.
        for i in start..start + count {
            let word_idx = i / 64;
            let bit_idx = i % 64;
            self.words[word_idx] |= 1u64 << bit_idx;
        }

        self.allocated_count += count;
        Ok(())
    }

    /// Clears all bits in the range `[start, start + count)` to 0 (free).
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::RangeOutOfBounds`] if the range exceeds `total_bits`.
    /// Returns [`BitmapError::InvalidState`] if any bit in the range is already clear.
    pub fn clear_range(&mut self, start: usize, count: usize) -> Result<(), BitmapError> {
        if start >= self.total_bits || start + count > self.total_bits {
            return Err(BitmapError::RangeOutOfBounds {
                start,
                count,
                total_bits: self.total_bits,
            });
        }

        // Check all bits first.
        for i in start..start + count {
            if !self.test(i)? {
                return Err(BitmapError::InvalidState {
                    index: i,
                    expected: "allocated (1)",
                    actual: "free (0)",
                });
            }
        }

        // Now clear all bits.
        for i in start..start + count {
            let word_idx = i / 64;
            let bit_idx = i % 64;
            self.words[word_idx] &= !(1u64 << bit_idx);
        }

        self.allocated_count -= count;
        Ok(())
    }

    // ========================================================================
    // Find operations (allocation support)
    // ========================================================================

    /// Finds the first free bit (0) starting from `from` (inclusive).
    ///
    /// Returns `None` if no free bit is found.
    #[inline]
    fn find_next_free_from(&self, from: usize) -> Option<usize> {
        let mut i = from;
        while i < self.total_bits {
            let word_idx = i / 64;
            let bit_idx = i % 64;

            // If the entire word is allocated (!word == 0), skip it.
            let word = self.words[word_idx];
            if word == !0u64 {
                i = (word_idx + 1) * 64;
                continue;
            }

            // Check if there's a free bit in this word at or after bit_idx.
            // `!word` has 1s where bits are free.
            let free_bits = !word;
            // Mask out bits before bit_idx.
            let masked_free = free_bits & (!0u64 << bit_idx);

            if masked_free != 0 {
                // Found a free bit.
                let free_bit_in_word = masked_free.trailing_zeros() as usize;
                let free_index = word_idx * 64 + free_bit_in_word;
                if free_index < self.total_bits {
                    return Some(free_index);
                }
            }

            i = (word_idx + 1) * 64;
        }
        None
    }

    /// Allocates a single free frame, returning its index.
    ///
    /// Uses `next_free_hint` for O(1) amortized performance on sequential allocations.
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::NoContiguousBlock`] if no free frame is available.
    pub fn allocate_single(&mut self) -> Result<usize, BitmapError> {
        // Try the hint first.
        if let Some(index) = self.find_next_free_from(self.next_free_hint) {
            self.set(index)?;
            return Ok(index);
        }

        // Hint was at the end; wrap around and try from 0.
        if self.next_free_hint > 0 {
            self.next_free_hint = 0;
            if let Some(index) = self.find_next_free_from(0) {
                self.set(index)?;
                return Ok(index);
            }
        }

        // Truly out of memory.
        Err(BitmapError::NoContiguousBlock {
            requested: 1,
            largest_available: 0,
        })
    }

    /// Finds a contiguous block of `count` free frames, returning the start index.
    ///
    /// This is the core algorithm for allocating DMA buffers, large pages, and
    /// other contiguous memory regions. It uses a sliding window approach with
    /// word-level optimizations.
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::NoContiguousBlock`] if no contiguous block of the
    /// requested size exists.
    pub fn find_contiguous(&self, count: usize) -> Result<usize, BitmapError> {
        if count == 0 {
            return Err(BitmapError::InvalidLayout {
                reason: "count must be > 0",
            });
        }

        if count > self.total_bits {
            return Err(BitmapError::NoContiguousBlock {
                requested: count,
                largest_available: self.total_bits,
            });
        }

        // Special case: count == 1 is just a single allocation.
        if count == 1 {
            return self
                .find_next_free_from(0)
                .ok_or(BitmapError::NoContiguousBlock {
                    requested: 1,
                    largest_available: 0,
                });
        }

        // General case: scan for `count` consecutive 0s.
        let mut current_run_start = None;
        let mut current_run_length = 0;
        let mut largest_found = 0;

        for i in 0..self.total_bits {
            let word_idx = i / 64;
            let bit_idx = i % 64;
            let is_allocated = (self.words[word_idx] & (1u64 << bit_idx)) != 0;

            if is_allocated {
                // End of a free run.
                if current_run_length > largest_found {
                    largest_found = current_run_length;
                }
                current_run_start = None;
                current_run_length = 0;
            } else {
                // Continue or start a free run.
                if current_run_start.is_none() {
                    current_run_start = Some(i);
                }
                current_run_length += 1;

                if current_run_length >= count {
                    // Found a block large enough!
                    return Ok(current_run_start.unwrap());
                }
            }
        }

        // Check the last run.
        if current_run_length > largest_found {
            largest_found = current_run_length;
        }

        Err(BitmapError::NoContiguousBlock {
            requested: count,
            largest_available: largest_found,
        })
    }

    /// Allocates a contiguous block of `count` frames, returning the start index.
    ///
    /// Combines [`find_contiguous`] and [`set_range`].
    ///
    /// # Errors
    ///
    /// Returns [`BitmapError::NoContiguousBlock`] if no contiguous block exists.
    /// Returns other errors if the range cannot be marked as allocated.
    pub fn allocate_contiguous(&mut self, count: usize) -> Result<usize, BitmapError> {
        let start = self.find_contiguous(count)?;
        self.set_range(start, count)?;
        Ok(start)
    }

    // ========================================================================
    // Statistics & Diagnostics
    // ========================================================================

    /// Computes a fragmentation report for the bitmap.
    ///
    /// This scans the entire bitmap to find the largest contiguous free block
    /// and computes a fragmentation index (0 = no fragmentation, 1 = fully fragmented).
    pub fn fragmentation_report(&self) -> FragmentationReport {
        let mut largest_free = 0;
        let mut current_free = 0;
        let mut free_block_count = 0;

        for i in 0..self.total_bits {
            let word_idx = i / 64;
            let bit_idx = i % 64;
            let is_free = (self.words[word_idx] & (1u64 << bit_idx)) == 0;

            if is_free {
                current_free += 1;
            } else {
                if current_free > 0 {
                    free_block_count += 1;
                    if current_free > largest_free {
                        largest_free = current_free;
                    }
                }
                current_free = 0;
            }
        }

        // Check the last run.
        if current_free > 0 {
            free_block_count += 1;
            if current_free > largest_free {
                largest_free = current_free;
            }
        }

        let total_free = self.free_count();
        let fragmentation_index = if total_free == 0 {
            0.0
        } else {
            1.0 - (largest_free as f64 / total_free as f64)
        };

        FragmentationReport {
            total_frames: self.total_bits,
            allocated_frames: self.allocated_count,
            free_frames: total_free,
            largest_free_block: largest_free,
            free_block_count,
            fragmentation_index,
        }
    }

    /// Dumps the bitmap to a formatter for debugging.
    ///
    /// Prints the bitmap as a hex string, with each word on a new line.
    pub fn dump(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "BitmapLayout {{")?;
        writeln!(f, "  total_bits: {},", self.total_bits)?;
        writeln!(f, "  allocated_count: {},", self.allocated_count)?;
        writeln!(f, "  free_count: {},", self.free_count())?;
        writeln!(f, "  words: [")?;
        for (i, word) in self.words.iter().enumerate() {
            writeln!(f, "    [{:04}] {:#018x},", i, word)?;
        }
        writeln!(f, "  ]")?;
        writeln!(f, "}}")
    }
}

// ============================================================================
// FragmentationReport
// ============================================================================

/// Statistics about bitmap fragmentation.
#[derive(Debug, Clone, Copy)]
pub struct FragmentationReport {
    /// Total number of frames in the bitmap.
    pub total_frames: usize,
    /// Number of frames currently allocated.
    pub allocated_frames: usize,
    /// Number of frames currently free.
    pub free_frames: usize,
    /// Size of the largest contiguous free block (in frames).
    pub largest_free_block: usize,
    /// Number of separate free blocks (holes).
    pub free_block_count: usize,
    /// Fragmentation index: 0.0 = no fragmentation, 1.0 = fully fragmented.
    /// Computed as `1.0 - (largest_free_block / total_free)`.
    pub fragmentation_index: f64,
}

impl fmt::Display for FragmentationReport {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "Fragmentation Report:")?;
        writeln!(f, "  Total frames:        {}", self.total_frames)?;
        writeln!(f, "  Allocated frames:    {}", self.allocated_frames)?;
        writeln!(f, "  Free frames:         {}", self.free_frames)?;
        writeln!(f, "  Largest free block:  {}", self.largest_free_block)?;
        writeln!(f, "  Free block count:    {}", self.free_block_count)?;
        writeln!(f, "  Fragmentation index: {:.4}", self.fragmentation_index)
    }
}

// ============================================================================
// Iterators
// ============================================================================

/// Iterator over free frame indices in the bitmap.
pub struct FreeFrameIter<'a> {
    bitmap: &'a BitmapLayout,
    current: usize,
}

impl<'a> FreeFrameIter<'a> {
    /// Creates a new iterator over free frames.
    pub fn new(bitmap: &'a BitmapLayout) -> Self {
        Self {
            bitmap,
            current: 0,
        }
    }
}

impl<'a> Iterator for FreeFrameIter<'a> {
    type Item = usize;

    fn next(&mut self) -> Option<Self::Item> {
        while self.current < self.bitmap.total_bits {
            let index = self.current;
            self.current += 1;

            if self.bitmap.test(index).ok()? == false {
                return Some(index);
            }
        }
        None
    }
}

/// Iterator over allocated frame indices in the bitmap.
pub struct AllocatedFrameIter<'a> {
    bitmap: &'a BitmapLayout,
    current: usize,
}

impl<'a> AllocatedFrameIter<'a> {
    /// Creates a new iterator over allocated frames.
    pub fn new(bitmap: &'a BitmapLayout) -> Self {
        Self {
            bitmap,
            current: 0,
        }
    }
}

impl<'a> Iterator for AllocatedFrameIter<'a> {
    type Item = usize;

    fn next(&mut self) -> Option<Self::Item> {
        while self.current < self.bitmap.total_bits {
            let index = self.current;
            self.current += 1;

            if self.bitmap.test(index).ok()? == true {
                return Some(index);
            }
        }
        None
    }
}

impl BitmapLayout {
    /// Returns an iterator over all free frame indices.
    pub fn free_frames(&self) -> FreeFrameIter<'_> {
        FreeFrameIter::new(self)
    }

    /// Returns an iterator over all allocated frame indices.
    pub fn allocated_frames(&self) -> AllocatedFrameIter<'_> {
        AllocatedFrameIter::new(self)
    }
}

// ============================================================================
// Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    fn make_bitmap(total_bits: usize) -> BitmapLayout {
        let word_count = (total_bits + 63) / 64;
        let words = vec![0u64; word_count].leak();
        unsafe { BitmapLayout::new(words, total_bits).unwrap() }
    }

    #[test]
    fn test_basic_set_clear() {
        let mut bitmap = make_bitmap(128);

        // Initially all free.
        assert_eq!(bitmap.free_count(), 128);
        assert_eq!(bitmap.allocated_count(), 0);

        // Set bit 0.
        bitmap.set(0).unwrap();
        assert!(bitmap.test(0).unwrap());
        assert_eq!(bitmap.allocated_count(), 1);

        // Clear bit 0.
        bitmap.clear(0).unwrap();
        assert!(!bitmap.test(0).unwrap());
        assert_eq!(bitmap.allocated_count(), 0);
    }

    #[test]
    fn test_double_set_error() {
        let mut bitmap = make_bitmap(64);
        bitmap.set(10).unwrap();
        assert!(matches!(
            bitmap.set(10),
            Err(BitmapError::InvalidState { .. })
        ));
    }

    #[test]
    fn test_double_clear_error() {
        let mut bitmap = make_bitmap(64);
        assert!(matches!(
            bitmap.clear(10),
            Err(BitmapError::InvalidState { .. })
        ));
    }

    #[test]
    fn test_out_of_bounds() {
        let mut bitmap = make_bitmap(100);
        assert!(matches!(
            bitmap.test(100),
            Err(BitmapError::OutOfBounds { .. })
        ));
        assert!(matches!(
            bitmap.set(100),
            Err(BitmapError::OutOfBounds { .. })
        ));
    }

    #[test]
    fn test_allocate_single() {
        let mut bitmap = make_bitmap(128);

        let idx1 = bitmap.allocate_single().unwrap();
        assert!(bitmap.test(idx1).unwrap());

        let idx2 = bitmap.allocate_single().unwrap();
        assert!(bitmap.test(idx2).unwrap());
        assert_ne!(idx1, idx2);
    }

    #[test]
    fn test_allocate_contiguous() {
        let mut bitmap = make_bitmap(256);

        // Allocate 10 contiguous frames.
        let start = bitmap.allocate_contiguous(10).unwrap();
        for i in start..start + 10 {
            assert!(bitmap.test(i).unwrap());
        }

        // Frames before and after should be free.
        if start > 0 {
            assert!(!bitmap.test(start - 1).unwrap());
        }
        if start + 10 < 256 {
            assert!(!bitmap.test(start + 10).unwrap());
        }
    }

    #[test]
    fn test_contiguous_oom() {
        let mut bitmap = make_bitmap(100);

        // Allocate 90 frames.
        bitmap.allocate_contiguous(90).unwrap();

        // Try to allocate 20 contiguous frames (only 10 free).
        assert!(matches!(
            bitmap.allocate_contiguous(20),
            Err(BitmapError::NoContiguousBlock { .. })
        ));
    }

    #[test]
    fn test_fragmentation_report() {
        let mut bitmap = make_bitmap(100);

        // Allocate frames 10-19 and 30-39 (two holes).
        bitmap.set_range(10, 10).unwrap();
        bitmap.set_range(30, 10).unwrap();

        let report = bitmap.fragmentation_report();
        assert_eq!(report.total_frames, 100);
        assert_eq!(report.allocated_frames, 20);
        assert_eq!(report.free_frames, 80);
        assert_eq!(report.free_block_count, 3); // [0..10), [20..30), [40..100)
        assert_eq!(report.largest_free_block, 60); // [40..100)
    }

    #[test]
    fn test_iterators() {
        let mut bitmap = make_bitmap(10);

        bitmap.set(2).unwrap();
        bitmap.set(5).unwrap();
        bitmap.set(7).unwrap();

        let free: Vec<_> = bitmap.free_frames().collect();
        assert_eq!(free, vec![0, 1, 3, 4, 6, 8, 9]);

        let allocated: Vec<_> = bitmap.allocated_frames().collect();
        assert_eq!(allocated, vec![2, 5, 7]);
    }
}