.section .data
.align 16
.global tr_gdt
tr_gdt:
    .quad 0x0000000000000000
    .quad 0x00209A0000000000      
    .quad 0x0000920000000000      
    .quad 0x0020BA0000000000      
    .quad 0x0000B20000000000      
    .quad 0x0020FA0000000000      
    .quad 0x0000F20000000000      
.global tr_tss_desc
tr_tss_desc:
    .space 16

gdt_ptr:
    .short (gdt_ptr - tr_gdt) - 1
    .quad tr_gdt

.align 16
.global tr_tss
tr_tss:
    .space 104

.section .text

.global tr_init
.type tr_init, @function
tr_init:
    mov %rdi, %rax
    mov %eax, (tr_tss + 4)        
    shr $32, %rax
    mov %eax, (tr_tss + 8)        

    lgdt gdt_ptr

    mov $0x10, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es

    pushq $0x08
    leaq 1f(%rip), %rax
    pushq %rax
    lretq
1:
    mov $0x38, %ax
    ltr %ax

    ret



.global isr_hypercall
.type isr_hypercall, @function
isr_hypercall:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    mov %rsp, %rdi
    call tr_hyper

    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax

    iretq

.global isr_default
.type isr_default, @function
isr_default:
    // After `init()` swaps the IDT for the trampoline one, this is the only
    // handler every IRQ reaches. Send an EOI to both PICs so a level-triggered
    // IRQ (keyboard, PIT) is acknowledged and does not re-fire forever; the
    // write is harmless for exceptions and LAPIC interrupts.
    mov $0x20, %al
    out %al, $0x20
    out %al, $0xA0
    iretq

// Debug exception reporters: print the vector, the error code (where the CPU
// pushes one) and CR2 (page faults), then halt. These replace `isr_default`
// for the faults a ring-3 crash is most likely to raise, so the serial log
// shows exactly which one tripped instead of a bare triple-fault reset.

.global isr_pf
.type isr_pf, @function
isr_pf:
    pop %rsi
    mov %cr2, %rdx
    mov $0x0E, %rdi
    call tr_exception
1:  hlt
    jmp 1b

.global isr_gp
.type isr_gp, @function
isr_gp:
    pop %rsi
    xor %rdx, %rdx
    mov $0x0D, %rdi
    call tr_exception
1:  hlt
    jmp 1b

.global isr_ud
.type isr_ud, @function
isr_ud:
    xor %rsi, %rsi
    xor %rdx, %rdx
    mov $0x06, %rdi
    call tr_exception
1:  hlt
    jmp 1b

.global isr_df
.type isr_df, @function
isr_df:
    pop %rsi
    xor %rdx, %rdx
    mov $0x08, %rdi
    call tr_exception
1:  hlt
    jmp 1b

.global tr_restore_ctx
.type tr_restore_ctx, @function
tr_restore_ctx:
    mov %rdi, %rsp
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    iretq