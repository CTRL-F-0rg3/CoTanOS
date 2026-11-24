#include "../lib/syscall.h"

static void puts(const char *s) {
    unsigned long n = 0;
    while (s[n]) n++;
    sys_write(1, s, n);
}

int main(void) {
    puts("\n=== CoTanOS Daemon Manager (Forth-style) v0.1 ===\n");
    puts("Available commands: help, list, ping, exit\n\n");
    
    char buf[64];
    while (1) {
        puts("daemon> ");
        long n = sys_read(0, buf, sizeof(buf) - 1);
        if (n <= 0) continue;
        
        buf[n] = '\0';
        
        // Proste parsowanie (symulacja słów Forth)
        if (buf[0] == 'h' && buf[1] == 'e') {
            puts("Commands:\n  list  - show running daemons\n  ping  - test IPC\n  exit  - halt system\n");
        } else if (buf[0] == 'l' && buf[1] == 'i') {
            puts("[*] logger daemon   : RUNNING (PID 2)\n");
            puts("[*] net daemon      : STOPPED\n");
            puts("[*] storage daemon  : RUNNING (PID 4)\n");
        } else if (buf[0] == 'p' && buf[1] == 'i') {
            puts("[*] IPC ping successful. Latency: 0.001ms\n");
        } else if (buf[0] == 'e' && buf[1] == 'x') {
            puts("[*] Halting system...\n");
            sys_exit(0);
        } else {
            puts("[?] Unknown command. Type 'help'.\n");
        }
    }
    return 0;
}
