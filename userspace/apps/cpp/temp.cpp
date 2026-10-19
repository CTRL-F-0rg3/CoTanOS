// temp — temperature conversion between Celsius, Fahrenheit and Kelvin.
#include "apps.hpp"

extern "C" int cpp_temp(const char* args, char* out, int cap) {
    Out o{out, cap, 0};

    const char* p = args;
    long long v = atoll_simple(p);
    while (*p == ' ') p++;
    char u = *p;
    if (u >= 'a' && u <= 'z') u = (char)(u - 'a' + 'A');

    if (u == 'C') {
        long long f = v * 9 / 5 + 32;
        long long k = v + 273;
        o.put_int(v); o.put(" C = "); o.put_int(f); o.put(" F = ");
        o.put_int(k); o.put(" K"); o.nl();
    } else if (u == 'F') {
        long long c = (v - 32) * 5 / 9;
        long long k = c + 273;
        o.put_int(v); o.put(" F = "); o.put_int(c); o.put(" C = ");
        o.put_int(k); o.put(" K"); o.nl();
    } else if (u == 'K') {
        long long c = v - 273;
        long long f = c * 9 / 5 + 32;
        o.put_int(v); o.put(" K = "); o.put_int(c); o.put(" C = ");
        o.put_int(f); o.put(" F"); o.nl();
    } else {
        o.put("usage: temp <value><C|F|K>  (e.g. 100C)"); o.nl();
    }

    o.finish();
    return 0;
}
