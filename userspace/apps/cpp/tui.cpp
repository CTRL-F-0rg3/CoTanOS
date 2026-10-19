// tui — a static ASCII dashboard, demonstrating TUI-style rendering in C++.
// (The shell is line-oriented, so a "TUI" here is one full-screen text frame.)
#include "apps.hpp"

static void border(Out& o) {
    o.put('+');
    for (int i = 0; i < 36; i++) o.put('-');
    o.put('+');
    o.nl();
}

static void row(Out& o, const char* s) {
    o.put("| ");
    o.put(s);
    int len = 0;
    for (const char* q = s; *q; q++) len++;
    for (int i = len; i < 34; i++) o.put(' ');
    o.put(" |");
    o.nl();
}

extern "C" int cpp_tui(const char* args, char* out, int cap) {
    (void)args;
    Out o{out, cap, 0};

    border(o);
    row(o, "TITANUS SYSTEM MONITOR");
    border(o);
    row(o, "CPU   [##########....] 62%");
    row(o, "MEM   [######......] 41%");
    row(o, "GPU   [############..] 78%");
    row(o, "DISK  [######.......] 33%");
    border(o);
    row(o, "(demo C++ TUI render)");
    border(o);

    o.finish();
    return 0;
}
