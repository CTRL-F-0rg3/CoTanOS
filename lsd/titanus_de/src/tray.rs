//! Pasek systemowy w **lewym górnym** rogu: godzina, dźwięk, internet.
//!
//! Pasek jest warstwą wyższą niż okna (patrz [`crate::layers::Layer::Tray`]),
//! więc pływa nad aplikacjami, ale nie rezerwuje obszaru roboczego — w
//! przeciwieństwie do doku, który zabiera prawy pasek ekranu.

use crate::geometry::Rect;
use crate::theme::Theme;

/// Stan sieci pokazywany na pasku.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NetworkState {
    Offline,
    /// Wskazuje siłę sygnału 0..=100.
    Wifi(u8),
    Ethernet,
}

/// Pasek systemowy.
#[derive(Debug, Clone)]
pub struct Tray {
    /// Tekst godziny, np. `"12:34"`.
    pub clock: String,
    /// Głośność 0..=100.
    pub volume: u8,
    pub muted: bool,
    pub network: NetworkState,
}

impl Default for Tray {
    fn default() -> Self {
        Self {
            clock: String::from("00:00"),
            volume: 70,
            muted: false,
            network: NetworkState::Wifi(100),
        }
    }
}

impl Tray {
    /// Prostokąt paska: mała „pigułka” przyklejona do lewego górnego rogu.
    ///
    /// Szerokość jest na sztywno 320 px — wystarcza na godzinę, głośność
    /// i nazwę sieci przy rozmiarze pisma z motywu. Renderer dociągnie tekst.
    pub fn rect(&self, theme: &Theme) -> Rect {
        Rect::new(
            theme.tray_margin as i32,
            theme.tray_margin as i32,
            320,
            34,
        )
    }
}
