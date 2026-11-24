#include "../lib/syscall.h"

static void puts_(const char *s) {
    unsigned long n = 0;
    while (s[n]) n++;
    sys_write(1, s, n);
}

int main(void) {
    puts_("hello from C in U-mode\n");
    puts_("getpid = ");
    long pid = sys_getpid();
    char c = '0' + (char)pid;
    sys_write(1, &c, 1);
    puts_("\n");
    return 0;
}
