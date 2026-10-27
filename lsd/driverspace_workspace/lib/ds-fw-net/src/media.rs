//! Media selection, ported from FreeBSD's `if_media`.
//!
//! Source: `dirtycode/freebsd-src/sys/net/if_media.h`. The media word is an
//! ABI: `ifconfig` and the drivers decode the same bits, so every mask below is
//! copied from the header rather than reinvented - a driver that invents its own
//! subtype bits produces media lists that parse as a different mode.
//!
//! # Why this exists next to `device::LinkState`
//!
//! [`LinkState`](crate::device::LinkState) answers "can frames flow?" - four
//! states, one bit of meaning for the stack. The media word answers a different
//! question: *what did the two ends agree on*, in enough detail to print
//! `1000baseT/Full` and to force a mode when autonegotiation is wrong. FreeBSD
//! keeps both for the same reason; collapsing them loses the ability to say
//! "the link is up, but at half duplex because somebody forced it".
//!
//! # The word, from the header
//!
//! ```text
//!  bits 0-4    media subtype          (IFM_TMASK)
//!  bits 5-7    network type           (IFM_NMASK: ether=0x20, 802.11=0x80)
//!  bits 8-15   type-specific options  (IFM_OMASK: ETH_MASTER, RXPAUSE...)
//!  bits 16-18  mode, multi-mode devices (IFM_MMASK)
//!  bits 20-27  shared options         (IFM_GMASK: FDX, HDX, FLOW, LOOP)
//!  bits 28-31  instance               (IFM_IMASK)
//! ```

use crate::device::{LinkState, Speed};

// ── masks, `if_media.h` "Masks" ────────────────────────────────────────────

/// Network type field.
pub const NMASK: u32 = 0x000000e0;
/// Media subtype field.
pub const TMASK: u32 = 0x0000001f;
/// Instance field.
pub const IMASK: u32 = 0xf0000000;
/// Instance shift.
pub const ISHIFT: u32 = 28;
/// Type-specific options.
pub const OMASK: u32 = 0x0000ff00;
/// Mode field.
pub const MMASK: u32 = 0x00070000;
/// Mode shift.
pub const MSHIFT: u32 = 16;
/// Shared (global) options.
pub const GMASK: u32 = 0x0ff00000;

// ── network types ──────────────────────────────────────────────────────────

/// Ethernet.
pub const IFM_ETHER: u32 = 0x00000020;
/// IEEE 802.11 wireless.
pub const IFM_IEEE80211: u32 = 0x00000080;
/// ATM.
pub const IFM_ATM: u32 = 0x000000a0;

// ── shared subtypes and options ────────────────────────────────────────────

/// Autoselect the best media - the default every driver starts with.
pub const IFM_AUTO: u32 = 0;
/// A jumper or dipswitch selects the media.
pub const IFM_MANUAL: u32 = 1;
/// No media selected.
pub const IFM_NONE: u32 = 2;

/// Force full duplex.
pub const IFM_FDX: u32 = 0x00100000;
/// Force half duplex.
pub const IFM_HDX: u32 = 0x00200000;
/// Hardware flow control.
pub const IFM_FLOW: u32 = 0x00400000;
/// Put the hardware in loopback.
pub const IFM_LOOP: u32 = 0x08000000;

// ── status bits (the `ifmediareq.ifm_active` field) ────────────────────────

/// The active bit carries meaning.
pub const IFM_AVALID: u32 = 0x00000001;
/// The interface is attached to a working network.
pub const IFM_ACTIVE: u32 = 0x00000002;

// ── Ethernet subtypes, the values `ifconfig` prints ────────────────────────

/// 10BaseT - RJ45.
pub const IFM_10_T: u32 = 3;
/// 10Base2 - Thinnet.
pub const IFM_10_2: u32 = 4;
/// 10Base5 - AUI.
pub const IFM_10_5: u32 = 5;
/// 100BaseTX - RJ45.
pub const IFM_100_TX: u32 = 6;
/// 100BaseFX - Fiber.
pub const IFM_100_FX: u32 = 7;
/// 100BaseT4.
pub const IFM_100_T4: u32 = 8;
/// 1000BaseSX - multi-mode fiber.
pub const IFM_1000_SX: u32 = 11;
/// 1000BaseLX - single-mode fiber.
pub const IFM_1000_LX: u32 = 14;
/// 1000baseCX.
pub const IFM_1000_CX: u32 = 15;
/// 1000baseT - 4 pair cat 5.
pub const IFM_1000_T: u32 = 16;
/// Media types not defined yet.
pub const IFM_UNKNOWN: u32 = 25;
/// 10GBase-T - RJ45.
pub const IFM_10G_T: u32 = 26;
/// Something outside the table.
pub const IFM_OTHER: u32 = 31;

// ── Ethernet options (share the option bits with the extended variant) ─────

/// Master mode (1000baseT).
pub const IFM_ETH_MASTER: u32 = 0x00000100;
/// Receive PAUSE frames.
pub const IFM_ETH_RXPAUSE: u32 = 0x00000200;
/// Transmit PAUSE frames.
pub const IFM_ETH_TXPAUSE: u32 = 0x00000400;
/// Extended media variants.
pub const IFM_ETH_XTYPE: u32 = 0x00007800;

/// Shift the extended variant next to `TMASK`.
pub const IFM_ETH_XSHIFT: u32 = 6;

// ── the word as a type ─────────────────────────────────────────────────────

/// One media word. The bit layout is [`Media`]'s documentation and the
/// header's ABI; this newtype exists so a duplex flag can never be passed where
/// a subtype is expected.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Default)]
#[repr(transparent)]
pub struct Media(pub u32);

impl Media {
    /// The raw word, for talking to `SIOCGIFMEDIA`.
    #[inline]
    pub const fn raw(self) -> u32 {
        self.0
    }

    /// Wrap a raw word.
    #[inline]
    pub const fn new(word: u32) -> Self {
        Self(word)
    }

    /// `IFM_TYPE(x)`: the network type field.
    #[inline]
    pub const fn kind(self) -> u32 {
        self.0 & NMASK
    }

    /// `IFM_SUBTYPE(x)`. Ethernet special-cases the extraction because it
    /// borrows option bits for subtypes past 31 (`IFM_ETHER_SUBTYPE_GET`).
    #[inline]
    pub const fn subtype(self) -> u32 {
        if self.kind() == IFM_ETHER {
            self.0 & (TMASK | IFM_ETH_XTYPE)
        } else {
            self.0 & TMASK
        }
    }

    /// `IFM_OPTIONS` - type-specific plus shared option bits.
    #[inline]
    pub const fn options(self) -> u32 {
        self.0 & (OMASK | GMASK)
    }

    /// `IFM_MODE(x)`.
    #[inline]
    pub const fn mode(self) -> u32 {
        self.0 & MMASK
    }

    /// `IFM_INST(x)`.
    #[inline]
    pub const fn instance(self) -> u32 {
        (self.0 & IMASK) >> ISHIFT
    }

    /// Does the word carry this bit anywhere?
    #[inline]
    pub const fn has(self, bit: u32) -> bool {
        self.0 & bit != 0
    }

    /// `IFM_MAKEWORD(type, subtype, options, instance)`.
    #[inline]
    pub const fn make(kind: u32, subtype: u32, options: u32, instance: u32) -> Self {
        Self(kind | subtype | options | (instance << ISHIFT))
    }

    /// An Ethernet word for a subtype below 32 - `IFM_ETHER | subtype | opts`.
    ///
    /// Subtypes 32 and up go through the extension mechanism
    /// ([`IFM_ETH_XTYPE`]); this covers the table every copper card reports.
    #[inline]
    pub const fn ether(subtype: u32, options: u32) -> Self {
        Self::make(IFM_ETHER, subtype, options, 0)
    }

    /// Is this full duplex?
    #[inline]
    pub const fn is_full_duplex(self) -> bool {
        self.has(IFM_FDX)
    }

    /// The line rate this word names, in the units [`Speed`] uses.
    ///
    /// Zero for `autoselect` and for subtypes with no fixed rate: the card has
    /// not committed to one yet, and inventing a number would be a lie the
    /// stack could route on.
    pub fn speed(self) -> Speed {
        match self.0 & TMASK {
            // 10 Mbit: 10BaseT/2/5, STP, FL.
            3 | 4 | 5 | 12 | 13 => Speed::M10,
            // 100 Mbit: TX, FX, T4, VG, T2.
            6 | 7 | 8 | 9 | 10 => Speed::M100,
            // 1 Gbit: SX, LX, CX, T, KX.
            11 | 14 | 15 | 16 | 30 => Speed::G1000,
            // 2.5 Gbit.
            21 | 37 => Speed::G2500,
            // 10 Gbit: LR, SR, CX4, twinax variants, T.
            18 | 19 | 20 | 22 | 23 | 24 | 26 => Speed::G10000,
            _ => Speed(0),
        }
    }
}

/// The reply to `SIOCGIFMEDIA`, shaped as `struct ifmediareq`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct MediaReq {
    /// What the hardware is on now.
    pub current: u32,
    /// Status changes the caller does not care about.
    pub mask: u32,
    /// [`IFM_AVALID`] / [`IFM_ACTIVE`] bits.
    pub active: u32,
    /// How many media are on the list.
    pub count: usize,
}

impl MediaReq {
    /// Is the active bit meaningful?
    #[inline]
    pub fn has_status(self) -> bool {
        self.active & IFM_AVALID != 0
    }

    /// Is the interface attached to a working network?
    #[inline]
    pub fn is_active(self) -> bool {
        self.active & IFM_ACTIVE != 0
    }
}

/// How many media one interface may advertise.
///
/// A fixed list rather than a queue because this runs `no_std` with no
/// allocator: every Ethernet driver ever written advertises at most a handful
/// of words, and 24 is several times that.
pub const MAX_MEDIA: usize = 24;

/// FreeBSD's `struct ifmedia`: the media a card supports, what the user picked,
/// and what the hardware currently reports.
#[derive(Debug)]
pub struct IfMedia<const N: usize = MAX_MEDIA> {
    supported: [Media; N],
    count: usize,
    /// Changes the caller does not care about (`ifm_mask`).
    dontcare: u32,
    /// The user-selected word (`ifm_media`).
    selected: Media,
    /// What the hardware is on now (`ifm_current`).
    current: Media,
    /// [`IFM_AVALID`] / [`IFM_ACTIVE`] status bits.
    status: u32,
}

impl<const N: usize> IfMedia<N> {
    /// An empty list. `dontcare_mask` is the `ifmedia_init` mask of status bits
    /// a caller may ignore.
    pub const fn new(dontcare_mask: u32) -> Self {
        Self {
            supported: [Media(0); N],
            count: 0,
            dontcare: dontcare_mask,
            selected: Media(0),
            current: Media(0),
            status: 0,
        }
    }

    /// Advertise one media. Returns `false` only when the list is full; adding
    /// a word that is already present is a no-op that still reports success,
    /// because the caller's intent - "this media exists" - already holds.
    pub fn add(&mut self, word: Media) -> bool {
        if self.contains(word) {
            return true;
        }
        if self.count == N {
            return false;
        }
        self.supported[self.count] = word;
        self.count += 1;
        true
    }

    /// Drop every advertised media (`ifmedia_removeall`).
    pub fn removeall(&mut self) {
        self.count = 0;
    }

    /// Pick the media the user (or the driver default) selected.
    /// `ifmedia_set`.
    pub const fn set(&mut self, word: Media) {
        self.selected = word;
    }

    /// Is `word` on the advertised list? This is `ifmedia_validate_word`'s
    /// question: a set of a media the card never advertised is a caller bug,
    /// and applying it anyway produces a link that cannot come up.
    pub fn validate(&self, word: Media) -> bool {
        self.contains(word)
    }

    /// List membership scan.
    pub fn contains(&self, word: Media) -> bool {
        self.supported[..self.count].iter().any(|m| *m == word)
    }

    /// The driver's status callback: what the hardware landed on, and whether
    /// the answer is valid and active.
    pub fn report_status(&mut self, current: Media, avalid: bool, active: bool) {
        self.current = current;
        self.status = (if avalid { IFM_AVALID } else { 0 }) | (if active { IFM_ACTIVE } else { 0 });
    }

    /// The `SIOCGIFMEDIA` reply.
    pub fn request(&self) -> MediaReq {
        MediaReq {
            current: self.current.raw(),
            mask: self.dontcare,
            active: self.status,
            count: self.count,
        }
    }

    /// What the hardware reports right now.
    #[inline]
    pub const fn negotiated(&self) -> Media {
        self.current
    }

    /// What the user selected.
    #[inline]
    pub const fn selected(&self) -> Media {
        self.selected
    }

    /// The status bits as a [`LinkState`].
    ///
    /// `IFM_ACTIVE` alone means a working network (the header's own status
    /// table: Ethernet prints "no carrier" versus "active" from exactly these
    /// two bits). `IFM_AVALID` without `IFM_ACTIVE` is a known down link -
    /// `NoCarrier`. Neither bit set means the card has not said yet, which is
    /// `Negotiating`: not operational, but wrong to report as dead.
    pub fn link_state(&self) -> LinkState {
        if self.status & IFM_ACTIVE != 0 {
            LinkState::Up
        } else if self.status & IFM_AVALID != 0 {
            LinkState::NoCarrier
        } else {
            LinkState::Negotiating
        }
    }

    /// The negotiated rate: real only while the link is active. A carrier-less
    /// link has no rate to report, and reporting the advertised one would let
    /// the stack route as if frames could flow.
    pub fn speed(&self) -> Speed {
        if self.status & IFM_ACTIVE != 0 {
            self.current.speed()
        } else {
            Speed(0)
        }
    }

    /// How many media are advertised?
    #[inline]
    pub const fn count(&self) -> usize {
        self.count
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn the_word_decomposes_the_way_ifconfig_prints_it() {
        let w = Media::ether(IFM_1000_T, IFM_FDX);
        assert_eq!(w.kind(), IFM_ETHER);
        assert_eq!(w.subtype(), IFM_1000_T);
        assert!(w.has(IFM_FDX), "the shared option lives in the GMASK half");
        assert!(w.is_full_duplex());
        assert_eq!(w.speed(), Speed::G1000, "1000baseT is a gigabit word");
    }

    #[test]
    fn make_word_packs_every_field() {
        let w = Media::make(IFM_ETHER, IFM_100_TX, IFM_HDX, 3);
        assert_eq!(w.kind(), IFM_ETHER);
        assert_eq!(w.subtype(), IFM_100_TX);
        assert!(w.has(IFM_HDX));
        assert_eq!(w.instance(), 3);
        assert_eq!(w.mode(), 0);
    }

    #[test]
    fn the_speed_table_covers_the_rates_a_copper_card_reports() {
        assert_eq!(Media::ether(IFM_10_T, 0).speed(), Speed::M10);
        assert_eq!(Media::ether(IFM_100_TX, 0).speed(), Speed::M100);
        assert_eq!(Media::ether(IFM_10G_T, 0).speed(), Speed::G10000);
        assert_eq!(Media::ether(IFM_AUTO, 0).speed(), Speed(0), "autoselect has committed to nothing");
    }

    #[test]
    fn the_list_only_validates_what_was_advertised() {
        let mut m = IfMedia::<4>::new(0);
        let ten = Media::ether(IFM_10_T, 0);
        let hundred = Media::ether(IFM_100_TX, 0);
        assert!(m.add(ten));
        assert!(m.add(hundred));
        assert!(m.validate(ten));
        assert!(!m.validate(Media::ether(IFM_10G_T, 0)), "never advertised");
        assert_eq!(m.count(), 2);
        m.removeall();
        assert_eq!(m.count(), 0);
        assert!(!m.validate(ten));
    }

    #[test]
    fn a_full_list_refuses_rather_than_overwriting() {
        let mut m = IfMedia::<1>::new(0);
        assert!(m.add(Media::ether(IFM_10_T, 0)));
        assert!(m.add(Media::ether(IFM_10_T, 0)), "re-adding what is there still holds");
        assert!(!m.add(Media::ether(IFM_100_TX, 0)), "but a second slot does not exist");
        assert_eq!(m.count(), 1);
    }

    #[test]
    fn status_bits_map_to_the_four_link_states() {
        let mut m = IfMedia::<4>::new(0);
        let w = Media::ether(IFM_1000_T, IFM_FDX);
        assert_eq!(m.link_state(), LinkState::Negotiating, "the card has not said yet");
        m.report_status(w, true, false);
        assert_eq!(m.link_state(), LinkState::NoCarrier, "valid, not active: no carrier");
        m.report_status(w, true, true);
        assert_eq!(m.link_state(), LinkState::Up);
        assert_eq!(m.speed(), Speed::G1000, "a rate exists only while active");
    }

    #[test]
    fn the_ifmediareq_reply_carries_what_user_space_reads() {
        let mut m = IfMedia::<4>::new(0x2);
        m.add(Media::ether(IFM_10_T, IFM_FDX));
        m.add(Media::ether(IFM_100_TX, IFM_FDX));
        m.set(Media::ether(IFM_100_TX, IFM_FDX));
        m.report_status(Media::ether(IFM_100_TX, IFM_FDX), true, true);

        let req = m.request();
        assert_eq!(req.count, 2);
        assert_eq!(req.current, Media::ether(IFM_100_TX, IFM_FDX).raw());
        assert_eq!(req.mask, 0x2, "the dontcare mask survives into the reply");
        assert!(req.has_status());
        assert!(req.is_active());
        assert_eq!(m.selected(), Media::ether(IFM_100_TX, IFM_FDX));
        assert_eq!(m.negotiated(), Media::ether(IFM_100_TX, IFM_FDX));
    }
}



