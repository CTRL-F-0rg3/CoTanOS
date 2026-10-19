//! Typy geometryczne współdzielone przez całe środowisko.
//!
//! `allde` przekazuje wymiary jako gołe `i32`/`u32`, przez co łatwo pomylić
//! `(x, y, w, h)` z `(x, y, w, h)` w innej kolejności. Titanus wprowadza
//! nazwane typy — [`Size`], [`Point`], [`Rect`] — żeby każda sygnatura mówiła,
//! co przyjmuje, a kompilator łapał podstawienie złego typu w miejscu wywołania.

/// Rozmiar w pikselach (szerokość × wysokość).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct Size {
    pub w: u32,
    pub h: u32,
}

impl Size {
    pub const fn new(w: u32, h: u32) -> Self {
        Self { w, h }
    }

    /// Powierzchnia w pikselach, liczona na `u64`, żeby nie przepełnić `u32`
    /// na dużym ekranie (4K ma ~8,3 mln pikseli, ale mnożenie pośrednie
    /// `w * h` na `u32` przepełnia się już przy 65536×65536).
    pub const fn area(self) -> u64 {
        self.w as u64 * self.h as u64
    }
}

/// Punkt na ekranie, w pikselach.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct Point {
    pub x: i32,
    pub y: i32,
}

impl Point {
    pub const fn new(x: i32, y: i32) -> Self {
        Self { x, y }
    }

    pub const ZERO: Self = Self::new(0, 0);
}

/// Prostokąt: pozycja lewego górnego rogu plus rozmiar.
///
/// Lewa/górna krawędź należy do prostokąta, prawa/dolna już nie — tak samo jak
/// w `allde` i w `uspace::gfx`. Dzięki temu sąsiednie prostokąty nie nakładają
/// się o jeden piksel.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Rect {
    pub x: i32,
    pub y: i32,
    pub w: u32,
    pub h: u32,
}

impl Rect {
    pub const fn new(x: i32, y: i32, w: u32, h: u32) -> Self {
        Self { x, y, w, h }
    }

    /// Prawa krawędź (wyłączna).
    pub const fn right(self) -> i32 {
        self.x + self.w as i32
    }

    /// Dolna krawędź (wyłączna).
    pub const fn bottom(self) -> i32 {
        self.y + self.h as i32
    }

    /// Czy punkt leży wewnątrz prostokąta.
    pub fn contains(self, p: Point) -> bool {
        p.x >= self.x && p.x < self.right() && p.y >= self.y && p.y < self.bottom()
    }

    /// Czy prostokąt ma jakikolwiek niezerowy rozmiar.
    pub const fn is_empty(self) -> bool {
        self.w == 0 || self.h == 0
    }
}
