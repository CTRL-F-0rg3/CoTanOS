// Samodzielny test logiki C++ — kompilowany i uruchamiany przez `just titanus-cpp`,
// bez cargo. Sprawdza to samo co testy Rust przez FFI, ale wprost w C++.

#include "de_logic.hpp"

#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    TlDe* d = tl_de_new();

    uint32_t a = tl_spawn_tiled(d, "a");
    uint32_t b = tl_spawn_tiled(d, "b");
    assert(tl_focused(d) == b);
    tl_focus_next(d);
    assert(tl_focused(d) == a);
    tl_focus_prev(d);
    assert(tl_focused(d) == b);

    TlPlaced placed[8];
    uint32_t n = tl_layout(d, 0, 0, 100, 50, 8, placed, 8);
    assert(n == 2);
    assert(placed[0].w == 46 && placed[0].x == 0);
    assert(placed[1].x == 54);

    uint32_t f = tl_spawn_floating(d, "f", 200, 200, 100, 100);
    n = tl_layout(d, 0, 0, 800, 600, 8, placed, 8);
    assert(n == 3);
    assert(placed[2].kind == 1);
    assert(placed[2].x == 200 && placed[2].y == 200);

    char buf[64];
    uint32_t len = tl_window_title(d, a, buf, sizeof(buf));
    assert(len == 1 && std::strcmp(buf, "a") == 0);

    tl_move_window_to_workspace(d, f, 1);
    assert(tl_workspace_count(d) == 2);
    assert(tl_focused(d) == f);

    tl_close_window(d, f);
    assert(tl_focused(d) == 0);
    assert(tl_window_count(d) == 2);

    tl_de_free(d);
    std::printf("de_logic_test: OK\n");
    return 0;
}
