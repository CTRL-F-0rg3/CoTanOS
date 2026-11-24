package main

import "core:fmt"
import "core:os"

SYS_WRITE :: 64
SYS_EXIT :: 93

@(export)
_start :: proc "c" () {
    msg := "scheduler daemon started\n"
    syscall3(SYS_WRITE, 1, uintptr(&msg[0]), uintptr(len(msg)))
    for {
        syscall3(SYS_EXIT, 0, 0, 0)
    }
}

syscall3 :: proc(n, a, b, c: uintptr) -> uintptr {
    ret: uintptr
    asm {
        ecall
        : a7 = n
        : a0 = a
        : a1 = b
        : a2 = c
        : a0 = ret
    }
    return ret
}
