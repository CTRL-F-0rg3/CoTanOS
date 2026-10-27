# `apps/` — dedykowane aplikacje TrangorgeOS (kompilowane osobno)

Aplikacje userspace **nie** są wbudowywane w `userspace/` (powłokę jądra). Żyją
tutaj, w katalogu głównym, jako samodzielne programy kompilowane do statycznych
ELF-ów `x86_64`, ładowane przez ring-3 loader jądra
(`kernel_Workspace/kernel/src/userspace/process/elf.rs`, entry `_start`,
non-PIE, segmenty `PT_LOAD`). Plik wynikowy wstawia się do obrazu jako
`/bin/<nazwa>.elf` i uruchamia przez `TGS_SPAWN` (0x1001).

## ABI (wspólne dla wszystkich języków)

Brama syscalli: **`int 0x80`** (DPL 3), rejestry `rax`=numer, `rdi/rsi/rdx/r10/r8/r9`=arg0..5.

| Blok | Zakres | Przykłady |
|------|--------|-----------|
| hyper | `0x0000–0x00FF` | `YIELD=1`, `LOG=2`, `TICK=3` |
| tgs   | `0x1000–0x1FFF` | `TGS_EXIT=0x1000`, `TGS_SPAWN=0x1001`, `TGS_GETPID=0x1002`, `TGS_KEY=0x1040` |
| posix | `0x2000–0x2FFF` | `MMAP=0x2000`, `READ=0x2005`, `WRITE=0x2006` (jeszcze nie w jądrze — patrz `ustd/SYSCALLS.md`) |
| net   | `0x3000–0x3FFF` | sockety (niezaimplementowane) |

Minimalny działający ABI to `LOG` + `TGS_EXIT` — wystarcza na „hello world".

## Języki

| Katalog | Język | Build | Stan |
|---------|-------|-------|------|
| `zig/`  | Zig (freestanding) | `./build.sh` | ✅ skompilowane (`hello`, `calc`) |
| `rust/` | Rust (no_std + target JSON) | `./build.sh` | target + przykład |
| `rust/` (pełne `std`) | Rust | `../ustd/build.sh` | dojrzały framework (`tgs-hal`/`tgs-libc`/`tgs-rt`) |
| `cpp/` | C++ (freestanding) | (przenoszone z `userspace/apps/cpp`) | — |

## Zig

```sh
cd apps/zig
./build.sh              # buduje hello i calc do zig-out/
```

`rt.zig` daje `int 0x80` (`log`, `exit`, `getpid`, `yield_now`) i deklaruje numery
syscalli; `linker.ld` ustawia entry `_start`, non-PIE, bazę `0x0040_0000`.

## Rust (no_std, szybki ELF)

```sh
cd apps/rust
./build.sh              # buduje hello do target/ (wymaga nightly + rust-src)
```

Target `x86_64-trangorgeos.json` to freestanding `x86_64-unknown-none-elf`
(z `-Zbuild-std=core`). Pełny `std` (bez `no_std`) jest w `../ustd/` — to
rekomendowana ścieżka dla normalnych programów Rust.
