// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
/*
 * CoTanOS userspace — /bin/init (C, glibc static).
 *
 * The POSIX demo: prints system information through real libc calls
 * (uname, sysinfo, clock_gettime, getpid), then chains the language
 * demo programs via execve (the kernel implements the Linux syscall
 * ABI on RISC-V, so this is an ordinary static riscv64-linux binary).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

extern char **environ;

int main(int argc, char **argv)
{
    struct utsname u;
    struct sysinfo si;
    struct timespec ts;

    if (argc > 1 && strcmp(argv[1], "done") == 0) {
        printf("\n[init] language demo chain completed: C, C++ and Rust "
               "(full std) all ran.\n");
        printf("[init] CoTanOS userspace is alive. Exiting (the kernel "
               "will restart me).\n");
        return 0;
    }

    clock_gettime(CLOCK_MONOTONIC, &ts);
    uname(&u);
    sysinfo(&si);

    printf("\n==== CoTanOS userspace - POSIX layer v1 ====\n");
    printf("[init] %s %s on %s (Linux-compatible syscall ABI)\n",
           u.sysname, u.release, u.machine);
    printf("[init] RAM: %lu MiB total / %lu MiB free, uptime %ld s\n",
           si.totalram / 1024 / 1024, si.freeram / 1024 / 1024, si.uptime);
    printf("[init] pid=%d, monotonic clock=%ld.%03ld s, argc=%d\n",
           getpid(), ts.tv_sec, ts.tv_nsec / 1000000, argc);
    printf("[init] chaining the language demo programs...\n");

    char *nargv[] = {"/bin/hello-cpp", NULL};
    execve("/bin/hello-cpp", nargv, environ);
    printf("[init] execve failed: %s\n", strerror(errno));
    return 1;
}
