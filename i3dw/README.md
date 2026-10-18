# i3dw — KDE-Plasma-like floating desktop for TrangorgeOS userspace

`i3dw` is a desktop environment for the userspace, modelled after **KDE Plasma**:
windows **float** and stack in a z-order (Plasma defaults to floating), a
**bottom panel** holds the launcher button, one task button per window and the
system tray, and a **Kickoff-style launcher** lists the apps.

It is the floating sibling of `allde` (which is a *niri-like tiling* desktop).
Both are pure renderers over a `&mut [u32]` framebuffer, so they run unchanged
on the host and, once the ring-3 std runtime is wired, over the kernel-provided
framebuffer.

## Features

| area | what it does |
|---|---|
| `wm` | floating window manager: z-order, focus/raise, minimize, maximize, move, resize, hit-testing |
| `panel` | bottom taskbar: launcher button, task buttons, system tray (clock + date) |
| `launcher` | Kickoff-like start menu listing the apps |
| `apps` | Welcome, Terminal, Pliki (file manager), Monitor systemu, Ustawienia |
| `theme` | Breeze-inspired palette + gradient / rounded-rect helpers |

## Run

```sh
cd i3dw && cargo test          # unit tests (wm invariants + end-to-end render)
cargo run                       # scripted session -> i3dw_frame.ppm
```

In the running OS the environment is launched from the in-system terminal with
`i3dw` (which switches input from the kernel terminal to the desktop), the same
way `allde` is launched.
