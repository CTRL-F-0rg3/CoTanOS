// apps.hpp — freestanding output-buffer helper for the C++ userspace apps.
//
// No <cstddef>, no <string>, no iostream: these apps are compiled with
// -ffreestanding and linked into the no_std kernel, so everything here is plain
// C-style C++. Every app exposes one `extern "C" int x(const char* args,
// char* out, int cap)` entry point and writes NUL-terminated text to `out`.
#ifndef TGS_CPP_APPS_HPP
#define TGS_CPP_APPS_HPP

struct Out {
    char* buf;
    int cap;
    int len;

    void put(char c) {
        if (len + 1 < cap) buf[len++] = c;
    }

    void put(const char* s) {
        while (*s) put(*s++);
    }

    void put_int(long long v) {
        char tmp[24];
        int i = 0;
        bool neg = v < 0;
        unsigned long long u = neg ? (unsigned long long)(-(v + 1)) + 1
                                   : (unsigned long long)v;
        do {
            tmp[i++] = (char)('0' + (u % 10));
            u /= 10;
        } while (u);
        if (neg) tmp[i++] = '-';
        while (i) put(tmp[--i]);
    }

    void nl() { put('\n'); }

    void finish() { buf[len] = 0; }
};

// Parse one integer, advancing `p` past it. Returns 0 when no digits follow.
static inline long long atoll_simple(const char*& p) {
    while (*p == ' ') p++;
    bool neg = false;
    if (*p == '-') {
        neg = true;
        p++;
    }
    long long v = 0;
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (*p - '0');
        p++;
    }
    return neg ? -v : v;
}

#endif
