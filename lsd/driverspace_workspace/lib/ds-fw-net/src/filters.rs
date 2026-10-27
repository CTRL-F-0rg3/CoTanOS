//! Receive-filter intent: promiscuous mode, all-multicast, the exact list.
//!
//! Ported from FreeBSD. The bookkeeping follows `sys/net/if.c` (`ifpromisc`,
//! `if_allmulti`, and `ifmultiaddr`'s reference counts); the decision table is
//! `sys/dev/e1000/if_em.c`'s `em_if_multi_set` and `em_if_set_promisc_impl`,
//! translated line for line:
//!
//! ```text
//! IFF_PROMISC                        → unicast-promisc + multicast-promisc
//! mcnt >= table size | IFF_ALLMULTI  → multicast-promisc only
//! otherwise                          → neither: the exact list is programmed
//! ```
//!
//! The intent lives here and the registers stay in the driver, so the same
//! table serves any card. The deferral survives too: `em` latches a
//! `promisc_pending` bit when flags change under interrupt and reapplies the
//! filters from its deferred admin task - [`RxFilters`] latches the same bit,
//! so a driver can call [`RxFilters::set_promisc`] from anywhere and apply the
//! result where it is safe.

use crate::types::MacAddress;

/// How many exact multicast addresses fit before the list truncates and the
/// program falls back to all-multicast.
pub const DEFAULT_MCAST_CAPACITY: usize = 32;

/// A filter-table operation that did not go through.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FilterError {
    /// The exact table is full. The intent is latched as truncated, so the
    /// card is put into all-multicast and no traffic is lost - only precision.
    Full,
    /// Removing an address the list does not hold. Nothing changed.
    NotPresent,
}

/// What the card should be programmed to accept.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FilterProgram {
    /// Every frame: the sniffer case (`UPE | MPE` in em's RCTL).
    AcceptAll,
    /// Every group frame, unicast still filtered (`MPE`): `IFF_ALLMULTI`, or
    /// the exact list overflowed its table.
    AllMulticast,
    /// The exact list fits: unicast by hash, these addresses by table.
    Exact { count: usize },
}

/// One multicast membership, refcounted the way `ifmultiaddr` does it: the
/// same address joined twice is one table entry with two references.
#[derive(Debug, Clone, Copy)]
struct Membership {
    addr: MacAddress,
    refs: u32,
}

/// The stack's receive-filter intent for one interface.
#[derive(Debug)]
pub struct RxFilters<const N: usize = DEFAULT_MCAST_CAPACITY> {
    entries: [Membership; N],
    count: usize,
    /// The list wanted more slots than it got. Cleared by [`Self::clear_mcast`].
    truncated: bool,
    promisc: bool,
    allmulti: bool,
    /// A filter change arrived that the driver has not applied to hardware yet.
    programming_pending: bool,
}

impl<const N: usize> RxFilters<N> {
    /// No filters: exact list empty, everything off.
    pub const fn new() -> Self {
        Self {
            entries: [Membership { addr: MacAddress::ZERO, refs: 0 }; N],
            count: 0,
            truncated: false,
            promisc: false,
            allmulti: false,
            programming_pending: false,
        }
    }

    /// Join a multicast address. Returns whether a table slot was taken
    /// (`false` means it was already present and only the reference count
    /// moved, which does not change what the hardware needs).
    pub fn add_mcast(&mut self, addr: MacAddress) -> Result<bool, FilterError> {
        for entry in self.entries[..self.count].iter_mut() {
            if entry.addr == addr {
                entry.refs += 1;
                return Ok(false);
            }
        }
        if self.count == N {
            // `em_copy_maddr` stops at the table size and the caller then sees
            // `mcnt >= MAX`, which flips multicast-promisc on: no traffic is
            // dropped, the filter just stops being exact.
            self.truncated = true;
            return Err(FilterError::Full);
        }
        self.entries[self.count] = Membership { addr, refs: 1 };
        self.count += 1;
        self.programming_pending = true;
        Ok(true)
    }

    /// Leave a multicast address, dropping the table entry at the last
    /// reference.
    pub fn remove_mcast(&mut self, addr: MacAddress) -> Result<(), FilterError> {
        let Some(index) = self.entries[..self.count].iter().position(|e| e.addr == addr)
        else {
            return Err(FilterError::NotPresent);
        };
        self.entries[index].refs -= 1;
        if self.entries[index].refs == 0 {
            self.count -= 1;
            self.entries.swap(index, self.count);
        }
        self.programming_pending = true;
        Ok(())
    }

    /// Forget the whole list (`if_delallmulti`'s effect on intent).
    pub fn clear_mcast(&mut self) {
        if self.count != 0 || self.truncated {
            self.count = 0;
            self.truncated = false;
            self.programming_pending = true;
        }
    }

    /// Request or drop promiscuous mode. Returns whether it changed.
    pub fn set_promisc(&mut self, on: bool) -> bool {
        let changed = self.promisc != on;
        self.promisc = on;
        self.programming_pending |= changed;
        changed
    }

    /// Request or drop all-multicast mode (`IFF_ALLMULTI`). Returns whether it
    /// changed.
    pub fn set_allmulti(&mut self, on: bool) -> bool {
        let changed = self.allmulti != on;
        self.allmulti = on;
        self.programming_pending |= changed;
        changed
    }

    /// The decision table, in `em_if_multi_set`'s order.
    pub fn desired(&self) -> FilterProgram {
        if self.promisc {
            return FilterProgram::AcceptAll;
        }
        if self.allmulti || self.truncated || self.count >= N {
            return FilterProgram::AllMulticast;
        }
        FilterProgram::Exact { count: self.count }
    }

    /// Latch that hardware programming is owed (for flag changes that arrived
    /// from interrupt context, the `promisc_pending` deferral).
    pub fn mark_programming_pending(&mut self) {
        self.programming_pending = true;
    }

    /// Read-and-clear the pending bit, like `atomic_readandclear` on em's.
    pub fn take_programming_pending(&mut self) -> bool {
        core::mem::take(&mut self.programming_pending)
    }

    /// The exact list, in insertion order.
    #[inline]
    pub fn multicast(&self) -> impl Iterator<Item = MacAddress> + '_ {
        self.entries[..self.count].iter().map(|e| e.addr)
    }

    /// How many addresses are in the table?
    #[inline]
    pub fn mcast_count(&self) -> usize {
        self.count
    }

    /// Did the list want more slots than the table has?
    #[inline]
    pub fn is_truncated(&self) -> bool {
        self.truncated
    }

    /// Is promiscuous mode requested?
    #[inline]
    pub fn is_promisc(&self) -> bool {
        self.promisc
    }

    /// Is all-multicast requested?
    #[inline]
    pub fn is_allmulti(&self) -> bool {
        self.allmulti
    }
}

impl<const N: usize> Default for RxFilters<N> {
    fn default() -> Self {
        Self::new()
    }
}

