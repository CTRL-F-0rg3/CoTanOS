# Titanus DE

Kafelkowo-pływające środowisko graficzne dla userspace TrangorgeOS. **Rdzeń**
(geometria, motyw, tapety, dok, pasek, render, kompozycja) jest w **Rust**,
**logika DE** (okna, obszary robocze, fokus, layout) w **C++**, a **wygląd**
(kolory, typografia, wymiary) w **C#** — patrz „Podział na języki” niżej.

## Uruchomienie (host)

```sh
cd titanus_de
cargo test                       # testy rdzenia + logiki C++ + motywu C#
cargo run                        # renderuje titanus_frame.ppm (1280x720)
cargo run -- --wallpaper tapeta.png --fit cover
cargo run -- --theme theme_cs/theme_light.json   # motyw z C#
just titanus-theme               # regeneruje theme.json z warstwy C#
just titanus-interface           # C# (dotnet) steruje DE przez P/Invoke
just titanus-aot                 # C# -> NativeAOT -> natywny ELF -> uruchamia się
just titanus-static              # C# -> w pełni statyczny ELF (musl, static-pie)
```

Na hoście wynik to plik PPM (taka sama droga diagnostyczna jak `demo-gfx` /
`settings-app`). Na systemie ten sam `Titanus::render` rysuje do framebuffera
z jądra (`uspace::kernel::KernelClient`).

## Mapowanie wymagań → moduły

| Wymaganie | Moduł |
|---|---|
| rdzeń = Rust | cała skrzynka |
| kafelkowo **i** pływająco | logika C++ (`de_logic.cpp`) + `window.rs` (`WindowKind`) |
| dok po prawej | `dock.rs` |
| pasek systemowy (godzina/dźwięk/internet) lewy górny | `tray.rs` |
| tapety PNG | `wallpaper.rs` (dekodowanie `png`, fit: stretch/cover/contain/center/tile) |
| warstwowość przez pipeline | `layers.rs` (`Layer`), zbieżne z z-order `gfx-server` i `uspace::gfx::Compositor` |
| workspaces | logika C++ (`de_logic.cpp`) |
| kompozytor | `render.rs` (kompozycja sceny do jednego framebuffera) |
| wygląd (C#) | `theme_cs/` (`Theme.cs` → `theme.json`) + `theme.rs` (`from_json`) |
| usługi (daemony) i autostart | `service.rs` (`ServiceManager`) + panel „Usługi” + C ABI + C# |

## Podział na języki

1. **Rust — rdzeń (zrobione).** Geometria, okna, layout, obszary robocze,
   warstwy, dok, pasek, tapety i kompozycja są w pełni zaimplementowane
   i przetestowane.

2. **C++ — logika (zrobione).** Zarządzanie oknami, obszarami roboczymi,
   fokusem i layoutem żyje w `src/logic/de_logic.cpp`, kompilowanym przez
   `build.rs` (skrzynka `cc`) i wołanym z Rust przez `extern "C"`
   (`src/logic.rs` → `DeLogic`). Jest też samodzielny test C++
   (`src/logic/de_logic_test.cpp`) uruchamiany przez `just titanus-cpp`.

3. **C# — wygląd (zrobione).** Warstwa `theme_cs/` (C#) autorsko definiuje
   kolory/typografię/wymiary (klasa `Theme` z presetami `Dark`/`Light`),
   serializuje do JSON (`theme.json`) przez System.Text.Json, a Rust wczytuje
   go przez `Theme::from_json` (serde_json) i osadza w binarkę
   (`include_str!`). C# działa jako narzędzie autorskie wyglądu na etapie
   budowy — `just titanus-theme`. Runtime C# na gołym OS-ie to nadal osobny,
   duży krok, ale wygląd jest już autentycznie pisany w C#.

## Interfejs C# i uruchamianie na OS

Rdzeń wystawia stabilny **C ABI** (`src/ffi.rs` → `libtitanus_de.so`), a warstwa
`interface_cs/` (C#) steruje nim przez P/Invoke: tworzy okna, ustawia motyw z C#
i renderuje — to jest „interfejs” środowiska.

```sh
just titanus-interface   # C# (dotnet) steruje DE przez P/Invoke
just titanus-aot         # C# -> NativeAOT -> natywny ELF (dynamicznie z glibc)
just titanus-static      # C# -> NativeAOT -> w pełni statyczny ELF (musl, static-pie)
```

`just titanus-aot` kompiluje C# do **natywnego ELF-a**
(`interface_cs/publish_aot/titanus-interface`), a `just titanus-static` do
**w pełni statycznego ELF-a** (`interface_cs/publish_static/titanus-interface`,
`static-pie linked`, bez interpretera) — to jest binarny, który loader
`ustd/compat/elf` (Linux ABI compatibility) może załadować w userspace OS-u
bez glibc. Static-pie wymaga linkera `lld` (bug `ld.bfd` 2.47).

## Pipeline graficzny (gdzie Titanus się wpina)

```
interfejs C# (interface_cs, NativeAOT) ──P/Invoke──► C ABI (ffi.rs)
                                                       │
  scena (Titanus::render, render.rs) ◄────────────────┘
    └─ warstwy (layers.rs) → z-order identyczny z:
         uspace::gfx::Compositor (powierzchnie po z)
         gfx_protocol_workspace/gfx-server (DisplayServer, z_order)
    └─ tapety: wallpaper.rs (png → RGBA8 → fit → framebuffer)
    └─ logika okien: C++ (de_logic.cpp przez build.rs)
    └─ docelowo: vGPU + ds-fw-gpu (driverspace_workspace) + Vise LG (częściowo)
```

## Testy

`cargo test` pokrywa: geometrię, kafelkowanie z przerwami, nakładkę pływającą,
trafianie (pływające wygrywa), obszary robocze (dodawanie/przełączanie/
przenoszenie), kolejność warstw, dekodowanie PNG (round-trip), dopasowanie
tapety (stretch/cover/contain), logikę C++ (spawn/fokus/layout/workspaces przez
FFI), motyw C# (JSON z `theme_cs` == `Theme::dark()`), usługi i autostart
(`ServiceManager`: add/start/stop/restart/autostart) i podstawowy cykl
środowiska. Sam test C++ (`just titanus-cpp`) sprawdza tę samą logikę wprost,
bez cargo.
