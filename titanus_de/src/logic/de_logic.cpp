// Implementacja logiki DE. Wnętrze `TlDe` to zwykłe kontenery C++ — strona
// Rust ich nie widzi i nie musi o nich wiedzieć.

#include "de_logic.hpp"

#include <cstring>
#include <string>
#include <vector>

struct TlDe {
    struct Window {
        uint32_t id = 0;
        std::string title;
        int32_t kind = 0; // 0 = kafelkowe, 1 = pływające
        int32_t x = 0;
        int32_t y = 0;
        uint32_t w = 0;
        uint32_t h = 0;
        bool alive = false;
    };

    // windows[0] to atrapa, więc id == indeks. next_id == windows.size().
    std::vector<Window> windows;
    std::vector<std::vector<uint32_t>> workspaces;
    uint32_t active = 0;
    uint32_t focused = 0; // 0 = brak
    uint32_t next_id = 1;

    TlDe() {
        windows.emplace_back();
        workspaces.emplace_back();
    }

    const std::vector<uint32_t>& active_ws() const { return workspaces[active]; }

    void remove_from_all(uint32_t id) {
        for (auto& ws : workspaces) {
            for (auto it = ws.begin(); it != ws.end();) {
                if (*it == id) {
                    it = ws.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    bool contains_active(uint32_t id) const {
        for (auto x : active_ws()) {
            if (x == id) return true;
        }
        return false;
    }
};

extern "C" {

TlDe* tl_de_new(void) { return new TlDe(); }

void tl_de_free(TlDe* d) { delete d; }

uint32_t tl_spawn_tiled(TlDe* d, const char* title) {
    uint32_t id = d->next_id++;
    d->windows.emplace_back();
    TlDe::Window& win = d->windows[id];
    win.id = id;
    win.title = title ? title : "";
    win.kind = 0;
    win.alive = true;
    d->workspaces[d->active].push_back(id);
    d->focused = id;
    return id;
}

uint32_t tl_spawn_floating(TlDe* d, const char* title, int32_t x, int32_t y, uint32_t width, uint32_t height) {
    uint32_t id = d->next_id++;
    d->windows.emplace_back();
    TlDe::Window& win = d->windows[id];
    win.id = id;
    win.title = title ? title : "";
    win.kind = 1;
    win.x = x;
    win.y = y;
    win.w = width;
    win.h = height;
    win.alive = true;
    d->workspaces[d->active].push_back(id);
    d->focused = id;
    return id;
}

void tl_close_window(TlDe* d, uint32_t id) {
    if (id == 0 || id >= d->windows.size() || !d->windows[id].alive) return;
    d->windows[id].alive = false;
    d->remove_from_all(id);
    if (d->focused == id) {
        d->focused = 0;
        const auto& ws = d->active_ws();
        if (!ws.empty()) d->focused = ws.back();
    }
}

void tl_focus(TlDe* d, uint32_t id) {
    if (d->contains_active(id)) d->focused = id;
}

void tl_focus_next(TlDe* d) {
    const auto& ws = d->active_ws();
    if (ws.empty()) return;
    if (d->focused == 0) {
        d->focused = ws[0];
        return;
    }
    for (uint32_t i = 0; i < ws.size(); ++i) {
        if (ws[i] == d->focused) {
            d->focused = ws[(i + 1) % ws.size()];
            return;
        }
    }
    d->focused = ws[0];
}

void tl_focus_prev(TlDe* d) {
    const auto& ws = d->active_ws();
    if (ws.empty()) return;
    if (d->focused == 0) {
        d->focused = ws.back();
        return;
    }
    for (uint32_t i = 0; i < ws.size(); ++i) {
        if (ws[i] == d->focused) {
            d->focused = ws[(i + ws.size() - 1) % ws.size()];
            return;
        }
    }
    d->focused = ws.back();
}

uint32_t tl_focused(const TlDe* d) { return d->focused; }

void tl_switch_workspace(TlDe* d, uint32_t index) {
    if (index >= d->workspaces.size()) return;
    d->active = index;
    d->focused = 0;
    const auto& ws = d->active_ws();
    if (!ws.empty()) d->focused = ws.back();
}

void tl_move_window_to_workspace(TlDe* d, uint32_t id, uint32_t index) {
    if (id == 0 || id >= d->windows.size() || !d->windows[id].alive) return;
    while (d->workspaces.size() <= index) d->workspaces.emplace_back();
    d->remove_from_all(id);
    d->workspaces[index].push_back(id);
    d->active = index;
    d->focused = id;
}

uint32_t tl_workspace_count(const TlDe* d) { return (uint32_t)d->workspaces.size(); }

uint32_t tl_layout(const TlDe* d, int32_t ax, int32_t ay, uint32_t aw, uint32_t ah, uint32_t gap, TlPlaced* out, uint32_t max) {
    const auto& ws = d->active_ws();
    uint32_t count = 0;

    auto emit = [&](uint32_t id, int32_t kind, int32_t x, int32_t y, uint32_t w, uint32_t h) {
        if (out != nullptr && count < max) {
            out[count].id = id;
            out[count].kind = kind;
            out[count].x = x;
            out[count].y = y;
            out[count].w = w;
            out[count].h = h;
        }
        ++count;
    };

    // Kafelki najpierw: równe kolumny z przerwą `gap`.
    std::vector<uint32_t> tiled;
    for (auto id : ws) {
        if (d->windows[id].alive && d->windows[id].kind == 0) tiled.push_back(id);
    }
    uint32_t n = (uint32_t)tiled.size();
    if (n > 0) {
        uint32_t total_gap = gap * (n - 1);
        uint32_t col_w = (aw > total_gap) ? ((aw - total_gap) / n) : 0;
        uint32_t step = col_w + gap;
        for (uint32_t i = 0; i < n; ++i) {
            emit(tiled[i], 0, ax + (int32_t)(i * step), ay, col_w, ah);
        }
    }

    // Pływające potem: zachowują własną geometrię (nakładka).
    for (auto id : ws) {
        const auto& w = d->windows[id];
        if (w.alive && w.kind == 1) {
            emit(id, 1, w.x, w.y, w.w, w.h);
        }
    }

    return count;
}

uint32_t tl_window_title(const TlDe* d, uint32_t id, char* buf, uint32_t buf_len) {
    if (id == 0 || id >= d->windows.size() || !d->windows[id].alive) {
        if (buf != nullptr && buf_len > 0) buf[0] = '\0';
        return 0;
    }
    const auto& t = d->windows[id].title;
    if (buf == nullptr || buf_len == 0) return (uint32_t)t.size();
    uint32_t n = (uint32_t)t.size();
    if (n > buf_len - 1) n = buf_len - 1;
    if (n > 0) std::memcpy(buf, t.data(), n);
    buf[n] = '\0';
    return (uint32_t)t.size();
}

uint32_t tl_window_count(const TlDe* d) {
    uint32_t n = 0;
    for (const auto& w : d->windows) {
        if (w.alive) ++n;
    }
    return n;
}

} // extern "C"

