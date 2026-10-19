// Logika DE Titanusa w C++: zarządzanie oknami, obszarami roboczymi, fokusem
// i layoutem. Rdzeń (geometria, render, tapety, motyw) zostaje w Rust — tutaj
// jest wyłącznie „logika DE”, wyeksponowana jako stabilny interfejs C.
//
// Handle `TlDe*` jest nieprzezroczysty: Rust widzi tylko wskaźnik i funkcje,
// nigdy wnętrze. Dzięki temu C++ może trzymać `std::vector`/`std::string`
// i zmieniać reprezentację bez ruszania strony Rust.
#pragma once

#include <cstdint>

extern "C" {

typedef struct TlDe TlDe;

/// Położenie jednego okna w layoutcie.
typedef struct {
    uint32_t id;
    int32_t kind; // 0 = kafelkowe, 1 = pływające
    int32_t x;
    int32_t y;
    uint32_t w;
    uint32_t h;
} TlPlaced;

TlDe* tl_de_new(void);
void  tl_de_free(TlDe*);

uint32_t tl_spawn_tiled(TlDe*, const char* title);
uint32_t tl_spawn_floating(TlDe*, const char* title, int32_t x, int32_t y, uint32_t width, uint32_t height);
void     tl_close_window(TlDe*, uint32_t id);

void     tl_focus(TlDe*, uint32_t id);
void     tl_focus_next(TlDe*);
void     tl_focus_prev(TlDe*);
uint32_t tl_focused(const TlDe*);

void     tl_switch_workspace(TlDe*, uint32_t index);
void     tl_move_window_to_workspace(TlDe*, uint32_t id, uint32_t index);
uint32_t tl_workspace_count(const TlDe*);

// Zwraca liczbę umieszczonych okien; `out` może być null (wtedy tylko liczy).
uint32_t tl_layout(const TlDe*, int32_t ax, int32_t ay, uint32_t aw, uint32_t ah, uint32_t gap, TlPlaced* out, uint32_t max);

// Zwraca pełną długość tytułu (bez NUL); wpisuje do `buf` max `buf_len-1` bajtów + NUL.
uint32_t tl_window_title(const TlDe*, uint32_t id, char* buf, uint32_t buf_len);
uint32_t tl_window_count(const TlDe*);

} // extern "C"
