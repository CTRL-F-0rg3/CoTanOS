// rand — deterministic pseudo-random numbers (LCG), for scripts and tests.
#include "apps.hpp"

extern "C" int cpp_rand(const char* args, char* out, int cap) {
    Out o{out, cap, 0};

    const char* p = args;
    long long count = atoll_simple(p);
    long long max = atoll_simple(p);
    if (count <= 0) count = 1;
    if (count > 20) count = 20;   // cap the output so it cannot flood the terminal
    if (max <= 1) max = 100;

    unsigned long long state = 0x9E3779B97F4A7C15ULL;
    for (long long i = 0; i < count; i++) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        o.put_int((long long)((state >> 33) % (unsigned long long)max));
        o.nl();
    }

    o.finish();
    return 0;
}
