//! Strażnik zawieszonego nadajnika i stabilizator linku.
//!
//! Port z FreeBSD: `em_watchdog` (`sys/dev/e1000/if_em.c`) i
//! flapping-guard z `em_update_link_status` / `if_link_state_change`
//! (`sys/net/if.c`).
//!
//! Karta, która przyjęła ramki do TX i milczy — ani kompletuje, ani nie
//! zgłasza błędu — wygląda jak wolna sieć, a nie jak awaria. FreeBSD liczy
//! ticki bez postępu i po `EM_TX_TIMEOUT` resetuje kartę. Ten moduł daje
//! sygnał; reset inicjuje sterownik.

use crate::device::LinkState;

/// Ile cykli bez postępu TX zanim uznamy zawieszenie.
///
/// FreeBSD: `EM_TX_TIMEOUT` to 5 sekund przy ticku 1s → 5 cykli.
/// Tu w tickach `tick`, nie w sekundach — sterownik wołający co 1s
/// dostaje te same 5s.
pub const TX_TIMEOUT_TICKS: u32 = 5;

/// Ile zgodnych odczytów linku zanim zmiana zostanie ogłoszona.
///
/// Tłumi flapping przy wpinaniu kabla: PHY raportuje Up/Down/Up przez
/// ~100ms, a każdy surowy odczyt budziłby `if_link_state_change`.
pub const LINK_STABLE_TICKS: u8 = 3;

/// Wynik sprawdzenia strażnika TX.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TxHang {
    /// TX rusza się albo kolejka pusta — OK.
    Healthy,
    /// Kolejka niepusta i licznik stoi — czekamy, jeszcze nie timeout.
    Stalled { ticks_without_progress: u32 },
    /// `TX_TIMEOUT_TICKS` bez postępu przy niepustej kolejce — resetuj kartę.
    Hung,
}

/// Strażnik zawieszonego nadajnika — port `em_watchdog`.
///
/// Sterownik woła [`Watchdog::tick`] cyklicznie (jak FreeBSD `callout` co
/// 1s), podając liczbę skompletowanych TX od startu (monotoniczną) i czy
/// kolejka TX jest pusta.
#[derive(Debug, Clone, Copy)]
pub struct Watchdog {
    last_completed: u64,
    ticks_without_progress: u32,
}

impl Watchdog {
    /// Nowy strażnik; licznik startuje od bieżącej wartości kompletacji.
    #[inline]
    pub fn new(completed: u64) -> Self {
        Self { last_completed: completed, ticks_without_progress: 0 }
    }

    /// Jeden tick. Przy [`TxHang::Hung`] sterownik ma zresetować kartę
    /// (jak `em` po `EM_TX_TIMEOUT`).
    pub fn tick(&mut self, completed: u64, tx_queue_empty: bool) -> TxHang {
        if tx_queue_empty {
            // Nic do wysłania — licznik stoi z definicji, nie jest to awaria.
            self.last_completed = completed;
            self.ticks_without_progress = 0;
            return TxHang::Healthy;
        }
        if completed != self.last_completed {
            self.last_completed = completed;
            self.ticks_without_progress = 0;
            return TxHang::Healthy;
        }
        self.ticks_without_progress += 1;
        if self.ticks_without_progress >= TX_TIMEOUT_TICKS {
            TxHang::Hung
        } else {
            TxHang::Stalled { ticks_without_progress: self.ticks_without_progress }
        }
    }

    /// Ile ticków bez postępu (do logu przed resetem).
    #[inline]
    pub fn ticks_without_progress(&self) -> u32 {
        self.ticks_without_progress
    }
}


/// Stabilizator stanu linku — guard przed flappingiem.
///
/// Surowy odczyt PHY wchodzi przez [`LinkDebounce::sample`]; ogłoszona
/// zmiana wychodzi dopiero po `LINK_STABLE_TICKS` zgodnych próbkach.
/// Mapowanie na [`LinkState`]: stabilne Up → `Up`, stabilne Down →
/// `NoCarrier` (karta żyje, kabla nie ma — nie `Down`, które znaczy
/// "karta martwa").
#[derive(Debug, Clone, Copy)]
pub struct LinkDebounce {
    stable: bool,
    pending: bool,
    pending_count: u8,
}

impl LinkDebounce {
    /// Start od znanego stanu (zwykle `false` — kabel wypięty).
    #[inline]
    pub fn new(link_up: bool) -> Self {
        Self { stable: link_up, pending: link_up, pending_count: 0 }
    }

    /// Jedna próbka surowego stanu PHY. Zwraca `Some(LinkState)` tylko gdy
    /// zmiana się ustabilizowała — wtedy wołaj `Ifnet::set_link`.
    pub fn sample(&mut self, link_up: bool) -> Option<LinkState> {
        if link_up == self.stable {
            self.pending_count = 0;
            return None;
        }
        if link_up != self.pending {
            self.pending = link_up;
            self.pending_count = 1;
            return None;
        }
        self.pending_count += 1;
        if self.pending_count >= LINK_STABLE_TICKS {
            self.stable = self.pending;
            self.pending_count = 0;
            Some(if self.stable { LinkState::Up } else { LinkState::NoCarrier })
        } else {
            None
        }
    }

    /// Aktualnie ogłoszony (stabilny) stan.
    #[inline]
    pub fn stable_up(&self) -> bool {
        self.stable
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn empty_queue_is_healthy_no_matter_what() {
        let mut w = Watchdog::new(10);
        for _ in 0..10 {
            assert_eq!(w.tick(10, true), TxHang::Healthy);
        }
        assert_eq!(w.ticks_without_progress(), 0);
    }

    #[test]
    fn progress_resets_the_counter() {
        let mut w = Watchdog::new(0);
        assert_eq!(w.tick(0, false), TxHang::Stalled { ticks_without_progress: 1 });
        assert_eq!(w.tick(5, false), TxHang::Healthy);
        assert_eq!(w.tick(5, false), TxHang::Stalled { ticks_without_progress: 1 });
    }

    #[test]
    fn five_stalled_ticks_means_hung() {
        let mut w = Watchdog::new(7);
        for i in 1..TX_TIMEOUT_TICKS {
            assert_eq!(w.tick(7, false), TxHang::Stalled { ticks_without_progress: i });
        }
        assert_eq!(w.tick(7, false), TxHang::Hung);
        assert_eq!(w.tick(7, false), TxHang::Hung);
        assert_eq!(w.tick(8, false), TxHang::Healthy);
    }

    #[test]
    fn single_spike_does_not_announce() {
        let mut d = LinkDebounce::new(false);
        assert_eq!(d.sample(false), None);
        assert_eq!(d.sample(true), None);
        assert_eq!(d.sample(false), None);
        assert_eq!(d.sample(false), None);
        assert!(!d.stable_up());
    }

    #[test]
    fn three_consistent_samples_announce_up() {
        let mut d = LinkDebounce::new(false);
        assert_eq!(d.sample(true), None);
        assert_eq!(d.sample(true), None);
        assert_eq!(d.sample(true), Some(LinkState::Up));
        assert!(d.stable_up());
        assert_eq!(d.sample(true), None);
    }

    #[test]
    fn loss_announces_no_carrier_not_down() {
        let mut d = LinkDebounce::new(true);
        assert_eq!(d.sample(false), None);
        assert_eq!(d.sample(false), None);
        assert_eq!(d.sample(false), Some(LinkState::NoCarrier));
    }
}
