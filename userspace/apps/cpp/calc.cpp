// calc — integer expression evaluator: + - * / % ( ), no floating point.
#include "apps.hpp"

static const char* g;

static long long expr();
static long long term();

static void skip() {
    while (*g == ' ') g++;
}

static long long factor() {
    skip();
    if (*g == '(') {
        g++;
        long long v = expr();
        skip();
        if (*g == ')') g++;
        return v;
    }
    bool neg = false;
    if (*g == '-') {
        neg = true;
        g++;
    }
    long long v = 0;
    while (*g >= '0' && *g <= '9') {
        v = v * 10 + (*g - '0');
        g++;
    }
    return neg ? -v : v;
}

static long long term() {
    long long v = factor();
    for (;;) {
        skip();
        char c = *g;
        if (c == '*') {
            g++;
            v *= factor();
        } else if (c == '/') {
            g++;
            long long d = factor();
            if (d) v /= d;
        } else if (c == '%') {
            g++;
            long long d = factor();
            if (d) v %= d;
        } else {
            break;
        }
    }
    return v;
}

static long long expr() {
    long long v = term();
    for (;;) {
        skip();
        char c = *g;
        if (c == '+') {
            g++;
            v += term();
        } else if (c == '-') {
            g++;
            v -= term();
        } else {
            break;
        }
    }
    return v;
}

extern "C" int cpp_calc(const char* args, char* out, int cap) {
    Out o{out, cap, 0};
    g = args;
    o.put_int(expr());
    o.nl();
    o.finish();
    return 0;
}
