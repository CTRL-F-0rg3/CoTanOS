#include "CoTan_types.h"
#include "CoTan_uart.h"
#include "CoTan_pmm.h"
#include "CoTan_mm.h"

#define UTEXT_VA 0x40000000UL
#define USTACK_VA 0x40002000UL
#define UTRAPFRAME_VA 0x40003000UL

static int map_user_page(u64 *root, u64 va, u64 pa, u64 flags) {
    u64 *tbl = root;
    for (int level = 2; level > 0; level--) {
        u64 idx = (va >> (12 + 9 * level)) & 0x1FF;
        u64 pte = tbl[idx];
        if (!(pte & PTE_V)) {
            u64 child = (u64)pmm_alloc_page();
            if (!child) return -1;
            volatile u64 *z = (volatile u64 *)child;
            for (int i = 0; i < 512; i++) z[i] = 0;
            tbl[idx] = ((child >> 12) << 10) | PTE_V;
            pte = tbl[idx];
        }
        tbl = (u64 *)(((pte >> 10) << 12));
    }
    u64 idx = (va >> 12) & 0x1FF;
    tbl[idx] = ((pa >> 12) << 10) | flags;
    return 0;
}

extern void user_enter(u64 *trapframe, u64 root_pa);

void start_user_process(void) {
    u64 root_pa = (u64)pmm_alloc_page();
    u64 code_pa = (u64)pmm_alloc_page();
    u64 stack_pa = (u64)pmm_alloc_page();
    u64 tf_pa = (u64)pmm_alloc_page();

    if (!root_pa || !code_pa || !stack_pa || !tf_pa) {
        uart_puts("[user] PANIC: no pages\n");
        return;
    }

    volatile u64 *z;
    z = (volatile u64 *)root_pa; for (int i=0; i<512; i++) z[i]=0;
    z = (volatile u64 *)code_pa; for (int i=0; i<512; i++) z[i]=0;
    z = (volatile u64 *)stack_pa; for (int i=0; i<512; i++) z[i]=0;
    z = (volatile u64 *)tf_pa; for (int i=0; i<512; i++) z[i]=0;

    u64 *root = (u64 *)root_pa;
    extern u64 g_root_pt[];
    for (int i = 0; i < 512; i++) if (g_root_pt[i] & PTE_V) root[i] = g_root_pt[i];

    map_user_page(root, UTEXT_VA, code_pa, PTE_V | PTE_R | PTE_X | PTE_U | PTE_A | PTE_D);
    map_user_page(root, USTACK_VA, stack_pa, PTE_V | PTE_R | PTE_W | PTE_U | PTE_A | PTE_D);
    map_user_page(root, UTRAPFRAME_VA, tf_pa, PTE_V | PTE_R | PTE_W | PTE_U | PTE_A | PTE_D);

    // Simple program: sys_write(1, "hello from U-mode\n", 18)
    // Place message AFTER instructions in the same page
    volatile u32 *code = (volatile u32 *)code_pa;
    int idx = 0;
    
    // li a0, 1
    code[idx++] = 0x00100513;
    // li a1, UTEXT_VA + 0x100 (message location, safely after code)
    // UTEXT_VA = 0x40000000, so 0x40000100
    code[idx++] = 0x400005b7; // lui a1, 0x40000
    code[idx++] = 0x10058593; // addi a1, a1, 0x100
    // li a2, 18
    code[idx++] = 0x01200613;
    // li a7, 64 (sys_write)
    code[idx++] = 0x04000893;
    // ecall
    code[idx++] = 0x00000073;
    // ebreak
    code[idx++] = 0x0000006f;  // j . (park, no breakpoint)

    // Message at offset 0x100 (256 bytes from start, safely after instructions)
    volatile char *msg = (volatile char *)(code_pa + 0x100);
    const char *str = "hello from U-mode\n";
    for (int i = 0; i < 18; i++) msg[i] = str[i];

    u64 *tf = (u64 *)tf_pa;
    tf[0] = UTEXT_VA;
    tf[1] = USTACK_VA + 4096;
    
    uart_puts("[user] entering U-mode...\n");
    user_enter(tf, root_pa);
    
    uart_puts("[user] returned from U-mode\n");
}
