//! FreeBSD-ported network mechanisms, glued into the kernel NIC layer.
//!
//! The port lives in `driverspace_workspace/lib/ds-fw-net` (source: the FreeBSD
//! tree under `dirtycode/freebsd-src`). This module is the glue: it surfaces
//! the ported types under one namespace - `crate::nic::netfw` - and adds the
//! few conversions a driver needs to adopt them without rewriting the existing
//! stack in `crate::nic`.
//!
//! # What was ported
//!
//! | FreeBSD source | Mechanism | Here as |
//! |----------------|-----------|---------|
//! | `sys/net/if.h`, `if_var.h` | `IFF_*` flags, `LINK_STATE_*`, MTU bounds, the `IFF_DRV_OACTIVE` gate | [`Ifnet`], [`IfFlags`] |
//! | `sys/net/if_media.h` | the `if_media` word ABI | [`Media`], [`IfMedia`] |
//! | `sys/net/if.c` + `if_em.c` | refcounted `ifpromisc`/`if_allmulti` intent | [`RxFilters`] |
//! | `if_em.c` `em_watchdog` | TX-hang watchdog, link flapping guard | [`Watchdog`], [`LinkDebounce`] |
//! | Linux NAPI | budgeted polling | [`Napi`] |
//! | `if_em.c` descriptor accounting | RX/TX ring ownership | [`RxRing`], [`TxRing`] |
//!
//! # The conversions
//!
//! The kernel's address newtypes are byte-identical to the framework's, so the
//! `From` impls below are free. The framework's [`fw::NetworkError`] is a strict
//! subset of the kernel's, so the error mapping up is total and lossless - the
//! kernel-only variants (`MmioUnmapped`, `FrameTooLarge`) are not in the `match`.

use ds_fw_net as fw;

// ── the ported mechanisms, one namespace ────────────────────────────────────

pub use fw::{
    // interface control state (`if.h` / `if_var.h`)
    IfFlags, IfaceEvent, Ifnet, MtuError, link_state_value, IF_MAXMTU, IF_MINMTU,
    // the media word ABI (`if_media.h`)
    IfMedia, Media, MediaReq,
    // receive-filter intent (`if.c` / `if_em.c`)
    FilterError, FilterProgram, RxFilters, DEFAULT_MCAST_CAPACITY,
    // TX-hang watchdog and link debounce (`if_em.c`)
    LinkDebounce, TxHang, Watchdog, LINK_STABLE_TICKS, TX_TIMEOUT_TICKS,
    // budgeted polling (NAPI)
    Napi, PollOutcome, DEFAULT_BUDGET, MAX_BUDGET, MIN_BUDGET,
    // ring ownership
    BufferId, Owner, RxRing, TxRing,
    // device-layer vocabulary the drivers report through
    Duplex, Features, LinkState, Speed, Stats,
    // the checksum the offload path bypasses
    Checksum,
};

pub use fw::{
    IFF_ALLMULTI, IFF_BROADCAST, IFF_CANTCHANGE, IFF_CANTCONFIG, IFF_DEBUG, IFF_DRV_OACTIVE,
    IFF_DRV_RUNNING, IFF_DYING, IFF_LINK0, IFF_LINK1, IFF_LINK2, IFF_LOOPBACK, IFF_MONITOR,
    IFF_MULTICAST, IFF_NEEDSEPOCH, IFF_NETLINK_1, IFF_NOARP, IFF_PALLMULTI, IFF_POINTOPOINT,
    IFF_PPROMISC, IFF_PROMISC, IFF_SIMPLEX, IFF_STATICARP, IFF_STICKYARP, IFF_UP, LINK_STATE_DOWN,
    LINK_STATE_UNKNOWN, LINK_STATE_UP,
};

pub use fw::{
    IFM_1000_T, IFM_100_FX, IFM_100_T4, IFM_100_TX, IFM_10G_T, IFM_10_2, IFM_10_5, IFM_10_T,
    IFM_ACTIVE, IFM_AUTO, IFM_AVALID, IFM_ETHER, IFM_FDX, IFM_FLOW, IFM_HDX, IFM_LOOP, IFM_MANUAL,
    IFM_NONE,
};

// ── address conversions: byte-identical newtypes ────────────────────────────

impl From<super::types::MacAddress> for fw::MacAddress {
    #[inline]
    fn from(m: super::types::MacAddress) -> Self {
        fw::MacAddress(m.0)
    }
}

impl From<fw::MacAddress> for super::types::MacAddress {
    #[inline]
    fn from(m: fw::MacAddress) -> Self {
        super::types::MacAddress(m.0)
    }
}

impl From<super::types::Ipv4Address> for fw::Ipv4Address {
    #[inline]
    fn from(a: super::types::Ipv4Address) -> Self {
        fw::Ipv4Address(a.0)
    }
}

impl From<fw::Ipv4Address> for super::types::Ipv4Address {
    #[inline]
    fn from(a: fw::Ipv4Address) -> Self {
        super::types::Ipv4Address(a.0)
    }
}

// ── error conversions: the framework's vocabulary is a subset ───────────────

impl From<fw::NetworkError> for super::error::NetworkError {
    fn from(err: fw::NetworkError) -> Self {
        match err {
            fw::NetworkError::DeviceNotReady => Self::DeviceNotReady,
            fw::NetworkError::DeviceNeedsReset => Self::DeviceNeedsReset,
            fw::NetworkError::UnsupportedFeatures { offered, requested } => {
                Self::UnsupportedFeatures { offered, requested }
            }
            fw::NetworkError::InvalidQueueSize => Self::InvalidQueueSize,
            fw::NetworkError::QueueFull => Self::QueueFull,
            fw::NetworkError::NoFreeDescriptor => Self::NoFreeDescriptor,
            fw::NetworkError::BadDescriptor => Self::BadDescriptor,
            fw::NetworkError::DmaAddressUnavailable => Self::DmaAddressUnavailable,
            fw::NetworkError::TransmissionFailed => Self::TransmissionFailed,
            fw::NetworkError::ReceiveFailed => Self::ReceiveFailed,
            fw::NetworkError::BufferTooSmall => Self::BufferTooSmall,
            fw::NetworkError::Timeout => Self::Timeout,
        }
    }
}

impl From<fw::PacketError> for super::error::PacketError {
    fn from(err: fw::PacketError) -> Self {
        match err {
            fw::PacketError::Truncated => Self::Truncated,
            fw::PacketError::InvalidEtherType => Self::InvalidEtherType,
            fw::PacketError::InvalidArp => Self::InvalidArp,
            fw::PacketError::InvalidIpv4Version => Self::InvalidIpv4Version,
            fw::PacketError::InvalidIpv4HeaderLength => Self::InvalidIpv4HeaderLength,
            fw::PacketError::InvalidIpv4Length => Self::InvalidIpv4Length,
            fw::PacketError::InvalidIpv4Checksum => Self::InvalidIpv4Checksum,
            fw::PacketError::FragmentedIpv4 => Self::FragmentedIpv4,
            fw::PacketError::InvalidIcmp => Self::InvalidIcmp,
            fw::PacketError::InvalidIcmpChecksum => Self::InvalidIcmpChecksum,
            fw::PacketError::UnsupportedProtocol => Self::UnsupportedProtocol,
        }
    }
}
