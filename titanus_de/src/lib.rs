//! Titanus DE — kafelkowo-pływające środowisko graficzne dla userspace
//! TrangorgeOS.
//!
//! # Podział na warstwy języków
//!
//! * **Rust (rdzeń)** — geometria, motyw, tapety, dok, pasek, render,
//!   kompozycja. W całości zaimplementowane tutaj.
//! * **C++ (logika)** — zarządzanie oknami, obszarami roboczymi, fokusem i
//!   layoutem; kompilowane przez `build.rs` i wołane przez FFI
//!   ([`logic::DeLogic`]). Zaimplementowane w `src/logic/de_logic.cpp`.
//! * **C# (wygląd)** — planowany; [`theme::Theme`] jest strukturą danych, którą
//!   docelowo wypełnia warstwa C#. Runtime C# na gołym systemie to osobny, duży
//!   krok (w repozytorium nie ma jeszcze ani kompilatora, ani runtime .NET).
//!
//! # Uruchomienie (host)
//!
//! ```sh
//! cargo test                 # testy rdzenia + logiki C++ przez FFI
//! cargo run                  # renderuje demonstracyjną ramkę do titanus_frame.ppm
//! cargo run -- --wallpaper tapeta.png   # z tapetą PNG
//! ```

pub mod de;
pub mod dock;
pub mod ffi;
pub mod geometry;
pub mod layers;
pub mod logic;
pub mod render;
pub mod service;
pub mod theme;
pub mod tray;
pub mod wallpaper;
pub mod window;

pub use de::Titanus;
pub use dock::{Dock, DockEntry};
pub use geometry::{Point, Rect, Size};
pub use layers::Layer;
pub use logic::DeLogic;
pub use service::{Service, ServiceManager, ServiceState};
pub use theme::{DEFAULT_THEME_JSON, Theme};
pub use tray::{NetworkState, Tray};
pub use wallpaper::{decode_png, DecodedImage, Fit};
pub use window::{Placed, WindowId, WindowKind};

#[cfg(test)]
mod tests;

