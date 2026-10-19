// stats — min / max / sum / average of a list of integers.
#include "apps.hpp"

extern "C" int cpp_stats(const char* args, char* out, int cap) {
    Out o{out, cap, 0};

    const char* p = args;
    long long n = 0, sum = 0, mn = 0, mx = 0;
    for (;;) {
        while (*p == ' ') p++;
        if (*p == 0) break;
        bool neg = false;
        if (*p == '-') {
            neg = true;
            p++;
        }
        if (*p < '0' || *p > '9') break;
        long long v = 0;
        while (*p >= '0' && *p <= '9') {
            v = v * 10 + (*p - '0');
            p++;
        }
        if (neg) v = -v;
        if (n == 0) {
            mn = mx = v;
        } else {
            if (v < mn) mn = v;
            if (v > mx) mx = v;
        }
        sum += v;
        n++;
    }

    if (n == 0) {
        o.put("usage: stats <number>..."); o.nl();
        o.finish();
        return 1;
    }

    o.put("count = "); o.put_int(n); o.nl();
    o.put("sum   = "); o.put_int(sum); o.nl();
    o.put("min   = "); o.put_int(mn); o.nl();
    o.put("max   = "); o.put_int(mx); o.nl();
    o.put("avg   = "); o.put_int(sum / n); o.nl();
    o.finish();
    return 0;
}
