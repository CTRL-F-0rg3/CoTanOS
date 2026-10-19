//! Model warstw: stała kolejność rysowania przez cały pipeline graficzny.
//!
//! # Dlaczego to osobny moduł
//!
//! Warstwowość musi być **jednym** pojęciem od sceny aż do scanoutu, nie
//! rozsypaną po plikach garścią magicznych liczb. Ta sama kolejność (`z` rosnące
//! = na wierzchu) występuje w:
//!
//! * `uspace::gfx::Compositor` — sortuje powierzchnie po `surface.z`,
//! * `gfx_protocol_workspace/crates/gfx-server/src/compositor.rs` — `DisplayServer`
//!   sortuje po `surface.z_order`,
//! * tutaj — [`Layer`] przypisuje każdej kategorii stałą warstwę.
//!
//! Titanus renderuje warstwy od najniższej do najwyższej, więc każda kolejna
//! po prostu nadpisuje poprzednią tam, gdzie się przecinają. To ta sama reguła,
//! której używają oba kompozytory — dzięki temu warstwa zachowuje sens, gdy
//! scena trafi do kompozytora sprzętowego przez vGPU, a nie zostanie dorysowana
//! w buforze CPU.

/// Warstwa sceny. Niższa wartość = głębiej (rysowana wcześniej).
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum Layer {
    /// Tapeta — tło całej sceny.
    Wallpaper = 0,
    /// Okna aplikacji (kafelki i pływające).
    Window = 10,
    /// Dok z aplikacjami po prawej stronie.
    Dock = 20,
    /// Pasek systemowy w lewym górnym rogu (godzina, dźwięk, sieć).
    Tray = 30,
    /// Kursor — zawsze na samym wierzchu.
    Cursor = 40,
}

impl Layer {
    /// Wszystkie warstwy, od najgłębszej do najpłytszej.
    pub const ALL: [Layer; 5] = [
        Layer::Wallpaper,
        Layer::Window,
        Layer::Dock,
        Layer::Tray,
        Layer::Cursor,
    ];
}
