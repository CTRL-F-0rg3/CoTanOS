//! Network driver framework: card contract + BSD-ported helpers.
//!
//! Ported from FreeBSD (`dirtycode/freebsd-src`):
//! `sys/net/if.h` + `if_var.h` (flags, link states) -> [`iface`],
//! `sys/net/if_media.h` (media word ABI) -> [`media`],
//! `sys/net/if.c` (`ifpromisc`, `if_allmulti`, refcounted `ifmultiaddr`)
//! + `sys/dev/e1000/if_em.c` (`em_if_multi_set`, `em_if_set_promisc_impl`)
//! -> [`filters`],
//! NAPI budgeted polling (Linux model the stack needs) -> [`napi`],
//! ring ownership -> [`ring`].
//!
//! # Boundary rule
//!
//! The framework speaks `kapi-abi`'s [`DsError`] at its boundary - the same
//! convention `ds-fw-block` and `ds-fw-gpu` use - while the device layer keeps
//! its richer [`NetworkError`] internally. [`to_ds_error`] is the conversion.

#![no_std]

pub mod checksum;
pub mod device;
pub mod error;
pub mod filters;
pub mod iface;
pub mod media;
pub mod napi;
pub mod ring;
pub mod types;
pub mod watchdog;

pub use checksum::{checksum, fill, fold, is_valid, ones_complement_sum, Checksum};
pub use device::{Duplex, Features, LinkState, NetworkDevice, PollResult, RxFrame, Speed, Stats, TxFrame};
pub use error::{NetworkError, PacketError};
pub use filters::{FilterError, FilterProgram, RxFilters, DEFAULT_MCAST_CAPACITY};
pub use iface::{
    IfFlags, IfaceEvent, Ifnet, MtuError, IF_MAXMTU, IF_MINMTU, link_state_value,
};
pub use iface::{
    IFF_ALLMULTI, IFF_BROADCAST, IFF_CANTCHANGE, IFF_CANTCONFIG, IFF_DEBUG, IFF_DRV_OACTIVE,
    IFF_DRV_RUNNING, IFF_DYING, IFF_LINK0, IFF_LINK1, IFF_LINK2, IFF_LOOPBACK, IFF_MONITOR,
    IFF_MULTICAST, IFF_NEEDSEPOCH, IFF_NETLINK_1, IFF_NOARP, IFF_PALLMULTI, IFF_POINTOPOINT,
    IFF_PPROMISC, IFF_PROMISC, IFF_SIMPLEX, IFF_STATICARP, IFF_STICKYARP, IFF_UP, LINK_STATE_DOWN,
    LINK_STATE_UNKNOWN, LINK_STATE_UP,
};
pub use media::{IfMedia, Media, MediaReq};
pub use media::{
    IFM_1000_T, IFM_100_FX, IFM_100_T4, IFM_100_TX, IFM_10G_T, IFM_10_2, IFM_10_5, IFM_10_T,
    IFM_ACTIVE, IFM_AUTO, IFM_AVALID, IFM_ETHER, IFM_FDX, IFM_FLOW, IFM_HDX, IFM_LOOP, IFM_MANUAL,
    IFM_NONE,
};
pub use napi::{Napi, PollOutcome, DEFAULT_BUDGET, MAX_BUDGET, MIN_BUDGET};
pub use ring::{BufferId, Owner, RxRing, TxRing};
pub use types::{Ethertype, Ipv4Address, MacAddress, HEADER_LEN, MAX_FRAME, MIN_FRAME};
pub use watchdog::{LinkDebounce, TxHang, Watchdog, LINK_STABLE_TICKS, TX_TIMEOUT_TICKS};

use kapi_abi::errors::DsError;

/// Convert a device-layer error to the driverspace ABI error.
///
/// `QueueFull` maps to `DeviceBusy` (backpressure the stack retries), timeouts
/// stay timeouts, everything else is a device fault. `BufferTooSmall` maps to
/// the ABI's buffer error - the only variant pair that names the same thing.
#[inline]
pub fn to_ds_error(err: NetworkError) -> DsError {
    match err {
        NetworkError::QueueFull => DsError::DeviceBusy,
        NetworkError::Timeout => DsError::Timeout,
        NetworkError::BufferTooSmall => DsError::BufferTooSmall,
        _ => DsError::DeviceFault,
    }
}
