//! Interface flags, the link announcement, and the transmit gate.
//!
//! Ported from FreeBSD `dirtycode/freebsd-src/sys/net/if.h` (the `IFF_*` number
//! space, `LINK_STATE_*`) and `sys/net/if_var.h` (`IF_MINMTU`/`IF_MAXMTU`), plus
//! the queue gate every BSD driver inherits from `if_ethersubr.c`.
//!
//! # The two halves of the flags
//!
//! FreeBSD splits `if_flags` (owned by the stack: `IFF_UP`, `IFF_PROMISC`, ...)
//! from `if_drv_flags` (owned by the driver: `IFF_DRV_RUNNING`,
//! `IFF_DRV_OACTIVE`). Both halves live in one number space so user space sees
//! one word, but the write rules differ, and the split is what stops a driver
//! and the stack from racing on the same bit. [`Ifnet`] keeps the two halves
//! apart for that reason.
//!
//! # `IFF_DRV_OACTIVE` is the whole transmit gate
//!
//! The stack may only hand the driver a frame while the interface is up, the
//! driver reports itself running, and the output-active bit is clear. When the
//! card's ring fills, the driver sets `IFF_DRV_OACTIVE` and the stack stops
//! queueing; when the interrupt drains the ring, the driver clears it and asks
//! for a restart. One bit, both directions of backpressure.

use crate::device::LinkState;

// ── `IFF_*`, values copied from `sys/net/if.h` ─────────────────────────────

/// Interface is administratively up.
pub const IFF_UP: u32 = 0x1;
/// Broadcast address valid.
pub const IFF_BROADCAST: u32 = 0x2;
/// Turn on debugging.
pub const IFF_DEBUG: u32 = 0x4;
/// Is a loopback net.
pub const IFF_LOOPBACK: u32 = 0x8;
/// Is a point-to-point link.
pub const IFF_POINTOPOINT: u32 = 0x10;
/// Calls `if_input` without the net epoch.
pub const IFF_NEEDSEPOCH: u32 = 0x20;
/// (driver) Resources allocated.
pub const IFF_DRV_RUNNING: u32 = 0x40;
/// No address resolution protocol.
pub const IFF_NOARP: u32 = 0x80;
/// Receive all packets.
pub const IFF_PROMISC: u32 = 0x100;
/// Receive all multicast packets.
pub const IFF_ALLMULTI: u32 = 0x200;
/// (driver) Tx hardware queue is full.
pub const IFF_DRV_OACTIVE: u32 = 0x400;
/// Can't hear own transmissions.
pub const IFF_SIMPLEX: u32 = 0x800;
/// Per link layer defined bit.
pub const IFF_LINK0: u32 = 0x1000;
/// Per link layer defined bit.
pub const IFF_LINK1: u32 = 0x2000;
/// Per link layer defined bit.
pub const IFF_LINK2: u32 = 0x4000;
/// Supports multicast.
pub const IFF_MULTICAST: u32 = 0x8000;
/// Unconfigurable using `ioctl(2)`.
pub const IFF_CANTCONFIG: u32 = 0x10000;
/// User-requested promisc mode.
pub const IFF_PPROMISC: u32 = 0x20000;
/// User-requested monitor mode.
pub const IFF_MONITOR: u32 = 0x40000;
/// Static ARP.
pub const IFF_STATICARP: u32 = 0x80000;
/// Sticky ARP.
pub const IFF_STICKYARP: u32 = 0x100000;
/// Interface is winding down.
pub const IFF_DYING: u32 = 0x200000;
/// User-requested allmulti mode.
pub const IFF_PALLMULTI: u32 = 0x800000;
/// Used by netlink.
pub const IFF_NETLINK_1: u32 = 0x1000000;

/// Flags that only the implementation may change, from `IFF_CANTCHANGE`.
pub const IFF_CANTCHANGE: u32 = IFF_BROADCAST
    | IFF_POINTOPOINT
    | IFF_DRV_RUNNING
    | IFF_DRV_OACTIVE
    | IFF_SIMPLEX
    | IFF_MULTICAST
    | IFF_ALLMULTI
    | IFF_PROMISC
    | IFF_DYING
    | IFF_CANTCONFIG
    | IFF_NEEDSEPOCH;

// ── bounds, `if_var.h` ─────────────────────────────────────────────────────

/// The smallest MTU the stack accepts.
pub const IF_MINMTU: usize = 72;
/// The largest MTU the stack accepts.
pub const IF_MAXMTU: usize = 65535;

// ── `LINK_STATE_*` wire values, `if.h` ─────────────────────────────────────

/// Link invalid or unknown.
pub const LINK_STATE_UNKNOWN: u8 = 0;
/// Link is down.
pub const LINK_STATE_DOWN: u8 = 1;
/// Link is up.
pub const LINK_STATE_UP: u8 = 2;

/// The `IFF_*` word as a set.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
#[repr(transparent)]
pub struct IfFlags(pub u32);

impl IfFlags {
    /// No flags.
    #[inline]
    pub const fn empty() -> Self {
        Self(0)
    }

    /// Wrap a raw word.
    #[inline]
    pub const fn from_bits(bits: u32) -> Self {
        Self(bits)
    }

    /// The raw word, for the `SIOCGIFFLAGS` reply.
    #[inline]
    pub const fn bits(self) -> u32 {
        self.0
    }

    /// Are all of `bits` set?
    #[inline]
    pub const fn contains(self, bits: u32) -> bool {
        self.0 & bits == bits
    }

    /// Set bits.
    #[inline]
    pub fn insert(&mut self, bits: u32) {
        self.0 |= bits;
    }

    /// Clear bits.
    #[inline]
    pub fn remove(&mut self, bits: u32) {
        self.0 &= !bits;
    }

    /// Set or clear, in one call.
    #[inline]
    pub fn set(&mut self, bits: u32, on: bool) {
        if on {
            self.insert(bits);
        } else {
            self.remove(bits);
        }
    }
}

/// An MTU outside the range the stack accepts ([`IF_MINMTU`], [`IF_MAXMTU`]).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum MtuError {
    /// Below [`IF_MINMTU`]: the header would not fit alongside a payload.
    BelowMin,
    /// Above [`IF_MAXMTU`]: the stack, not the card, owns this ceiling.
    AboveMax,
}

/// What an interface state change wants the caller to do next.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IfaceEvent {
    /// The link moved: re-announce it to the routing stack.
    LinkChanged,
    /// The interface was brought up.
    WentUp,
    /// The interface was brought down.
    WentDown,
}

/// One interface's control state: flags, MTU, link, and the transmit gate.
#[derive(Debug)]
pub struct Ifnet {
    /// The stack-owned half (`if_flags`): `IFF_UP`, `IFF_PROMISC`, ...
    pub stack_flags: IfFlags,
    /// The driver-owned half (`if_drv_flags`): `IFF_DRV_RUNNING`,
    /// `IFF_DRV_OACTIVE`.
    pub drv_flags: IfFlags,
    /// The payload MTU, always inside [`IF_MINMTU`]=..=[`IF_MAXMTU`].
    pub mtu: usize,
    /// The link as last announced.
    pub link: LinkState,
    /// The line rate that came with the last announcement, bits per second.
    pub link_bps: u64,
}

impl Ifnet {
    /// A down interface with the given MTU (clamped into range) and no carrier.
    pub fn new(mtu: usize) -> Self {
        Self {
            stack_flags: IfFlags::empty(),
            drv_flags: IfFlags::empty(),
            mtu: mtu.clamp(IF_MINMTU, IF_MAXMTU),
            link: LinkState::Down,
            link_bps: 0,
        }
    }

    /// Replace the stack-owned half, returning the previous value.
    pub fn set_stack_flags(&mut self, flags: IfFlags) -> IfFlags {
        core::mem::replace(&mut self.stack_flags, flags)
    }

    /// Set or clear one stack-owned flag. Returns whether it changed.
    pub fn set_stack_flag(&mut self, bit: u32, on: bool) -> bool {
        let before = self.stack_flags.contains(bit);
        self.stack_flags.set(bit, on);
        before != on
    }

    /// Administratively up or down (`if_up` / `if_down`): the `IFF_UP` bit
    /// only. `IFF_DRV_RUNNING` belongs to the driver, and a stack that sets it
    /// reports a card running before its rings exist.
    pub fn set_admin(&mut self, up: bool) -> Option<IfaceEvent> {
        if self.is_up() == up {
            return None;
        }
        self.stack_flags.set(IFF_UP, up);
        Some(if up { IfaceEvent::WentUp } else { IfaceEvent::WentDown })
    }

    /// Is the interface administratively up?
    #[inline]
    pub fn is_up(&self) -> bool {
        self.stack_flags.contains(IFF_UP)
    }

    /// The driver reports its rings allocated (or not).
    pub fn set_running(&mut self, running: bool) {
        self.drv_flags.set(IFF_DRV_RUNNING, running);
    }

    /// Has the driver finished allocating its rings?
    #[inline]
    pub fn is_running(&self) -> bool {
        self.drv_flags.contains(IFF_DRV_RUNNING)
    }

    /// The gate from `if_ethersubr`: while this is false the stack must not
    /// hand the driver another frame.
    #[inline]
    pub fn tx_gate_open(&self) -> bool {
        self.is_up() && self.is_running() && !self.tx_blocked()
    }

    /// Is the output queue stopped (`IFF_DRV_OACTIVE`)?
    #[inline]
    pub fn tx_blocked(&self) -> bool {
        self.drv_flags.contains(IFF_DRV_OACTIVE)
    }

    /// The card's ring filled up: stop the output queue.
    /// Returns whether this actually closed the gate.
    pub fn tx_stop(&mut self) -> bool {
        let was_blocked = self.tx_blocked();
        self.drv_flags.insert(IFF_DRV_OACTIVE);
        !was_blocked
    }

    /// The ring drained: clear `IFF_DRV_OACTIVE`. Returns whether the gate
    /// *reopened*, which is the driver's signal to restart output - a resume
    /// that returns `false` means nothing changed and must not poke the stack.
    pub fn tx_resume(&mut self) -> bool {
        if !self.tx_blocked() {
            return false;
        }
        self.drv_flags.remove(IFF_DRV_OACTIVE);
        self.tx_gate_open()
    }

    /// Set the MTU, refusing anything outside the stack's range.
    pub fn set_mtu(&mut self, mtu: usize) -> Result<usize, MtuError> {
        if mtu < IF_MINMTU {
            return Err(MtuError::BelowMin);
        }
        if mtu > IF_MAXMTU {
            return Err(MtuError::AboveMax);
        }
        self.mtu = mtu;
        Ok(mtu)
    }

    /// `if_link_state_change`: publish a new link state and rate.
    ///
    /// Returns `Some` only when something actually moved. FreeBSD's caller
    /// side discards duplicate announcements anyway; deduplicating here means
    /// a 500 ms poll that finds the same state costs no routing-stack wakeup.
    pub fn set_link(&mut self, state: LinkState, bps: u64) -> Option<IfaceEvent> {
        if self.link == state && self.link_bps == bps {
            return None;
        }
        self.link = state;
        self.link_bps = bps;
        Some(IfaceEvent::LinkChanged)
    }

    /// The `ifi_link_state` byte: the `LINK_STATE_*` value the card's state
    /// maps onto.
    pub fn link_state_value(&self) -> u8 {
        link_state_value(self.link)
    }
}

/// Pack any [`LinkState`] into the wire's `LINK_STATE_*` byte.
pub fn link_state_value(state: LinkState) -> u8 {
    match state {
        LinkState::Up => LINK_STATE_UP,
        LinkState::Down | LinkState::NoCarrier => LINK_STATE_DOWN,
        // Still negotiating is not "down" and not "up"; the header's unknown
        // value is the honest answer.
        LinkState::Negotiating => LINK_STATE_UNKNOWN,
    }
}


