#ifndef _COTAN_SHIM_H
#define _COTAN_SHIM_H

#include "base/display.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

static inline void* cotan_memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;
    for (size_t i = 0; i < n; i++) p[i] = (unsigned char)c;
    return s;
}

static inline void* cotan_memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

static inline int cotan_memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return (int)p1[i] - (int)p2[i];
    }
    return 0;
}

static inline size_t cotan_strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

extern void uart_puts(const char* s);

static int cotan_snprintf(char* buf, size_t size, const char* fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    size_t pos = 0;
    const char* p = fmt;
    
    while (*p && pos + 1 < size) {
        if (*p != '%') { buf[pos++] = *p++; continue; }
        p++;
        int zero_pad = 0, width = 0;
        if (*p == '0') { zero_pad = 1; p++; }
        while (*p >= '0' && *p <= '9') { width = width * 10 + (*p - '0'); p++; }
        
        char temp[32]; int tlen = 0;
        const char* str = NULL;
        
        switch (*p) {
            case 'u': {
                unsigned int val = __builtin_va_arg(args, unsigned int);
                if (val == 0) temp[tlen++] = '0';
                else { char rev[16]; int rlen = 0; while (val > 0) { rev[rlen++] = '0' + (val % 10); val /= 10; } for (int i = rlen - 1; i >= 0; i--) temp[tlen++] = rev[i]; }
                break;
            }
            case 'd': {
                int val = __builtin_va_arg(args, int);
                if (val < 0) { temp[tlen++] = '-'; val = -val; }
                if (val == 0) temp[tlen++] = '0';
                else { char rev[16]; int rlen = 0; while (val > 0) { rev[rlen++] = '0' + (val % 10); val /= 10; } for (int i = rlen - 1; i >= 0; i--) temp[tlen++] = rev[i]; }
                break;
            }
            case 'x': case 'X': {
                unsigned int val = __builtin_va_arg(args, unsigned int);
                if (val == 0) temp[tlen++] = '0';
                else { char rev[16]; int rlen = 0; while (val > 0) { int d = val % 16; rev[rlen++] = (d < 10) ? ('0' + d) : ('a' + d - 10); val /= 16; } for (int i = rlen - 1; i >= 0; i--) temp[tlen++] = rev[i]; }
                break;
            }
            case 'l': {
                p++;
                if (*p == 'x' || *p == 'X') {
                    unsigned long val = __builtin_va_arg(args, unsigned long);
                    if (val == 0) temp[tlen++] = '0';
                    else { char rev[20]; int rlen = 0; while (val > 0) { int d = val % 16; rev[rlen++] = (d < 10) ? ('0' + d) : ('a' + d - 10); val /= 16; } for (int i = rlen - 1; i >= 0; i--) temp[tlen++] = rev[i]; }
                }
                break;
            }
            case 's': { str = __builtin_va_arg(args, const char*); if (!str) str = "(null)"; break; }
            case '%': buf[pos++] = '%'; p++; continue;
            default: buf[pos++] = '%'; if (pos + 1 < size) buf[pos++] = *p; p++; continue;
        }
        p++;
        if (str) {
            int slen = cotan_strlen(str);
            for (int i = 0; i < slen && pos + 1 < size; i++) buf[pos++] = str[i];
        } else {
            char pad = zero_pad ? '0' : ' ';
            while (tlen < width && pos + 1 < size) { buf[pos++] = pad; width--; }
            for (int i = 0; i < tlen && pos + 1 < size; i++) buf[pos++] = temp[i];
        }
    }
    buf[pos] = '\0';
    __builtin_va_end(args);
    return (int)pos;
}

#define memset cotan_memset
#define memcpy cotan_memcpy
#define memcmp cotan_memcmp
#define strlen cotan_strlen
#define snprintf cotan_snprintf
#define printf(...) do { char _buf[256]; cotan_snprintf(_buf, sizeof(_buf), __VA_ARGS__); uart_puts(_buf); } while(0)

#define COTAN_HEAP_SIZE (4 * 1024)
static unsigned char cotan_heap[COTAN_HEAP_SIZE];
static size_t cotan_heap_offset = 0;

static inline void* cotan_malloc(size_t size) {
    size = (size + 7) & ~7;
    if (cotan_heap_offset + size > COTAN_HEAP_SIZE) return NULL;
    void* ptr = &cotan_heap[cotan_heap_offset];
    cotan_heap_offset += size;
    return ptr;
}

static inline void* cotan_realloc(void* ptr, size_t new_size) {
    void* new_ptr = cotan_malloc(new_size);
    if (new_ptr && ptr) cotan_memcpy(new_ptr, ptr, new_size);
    return new_ptr;
}

static inline void cotan_free(void* ptr) { (void)ptr; }

#define malloc cotan_malloc
#define realloc cotan_realloc
#define free cotan_free

#endif
