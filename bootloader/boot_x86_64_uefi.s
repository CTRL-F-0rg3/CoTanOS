    .text
    .code64
    .intel_syntax noprefix
    .globl jump_to_kernel
    .type  jump_to_kernel, @function
jump_to_kernel:
    cli
    mov rsp, 0x90000
    mov rax, rdi            ; entry -> RAX
    mov rdi, rdx            ; system_table -> RDI (first arg for kernel)
    xor rsi, rsi
    xor rdx, rdx
    xor rcx, rcx
    xor r8,  r8
    xor r9,  r9
    xor r10, r10
    xor r11, r11
    jmp rax
    .size jump_to_kernel, . - jump_to_kernel
