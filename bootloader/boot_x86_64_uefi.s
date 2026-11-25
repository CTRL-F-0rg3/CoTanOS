    .text
    .code64
    .intel_syntax noprefix
    .globl jump_to_kernel
    .type  jump_to_kernel, @function

# SysV AMD64 ABI:
#   RDI = kernel entry point
#   RSI = pointer to boot_info structure
jump_to_kernel:
    cli
    mov rsp, 0x90000
    xor rbp, rbp

    mov rax, rdi                        # entry -> RAX
    mov rdi, rsi                        # arg1 = boot_info
    movabs rsi, 0x424F4F5436342121      # arg2 = magic "BOOT64!!"

    xor rdx, rdx
    xor rcx, rcx
    xor r8,  r8
    xor r9,  r9
    xor r10, r10
    xor r11, r11

    jmp rax
    .size jump_to_kernel, . - jump_to_kernel

    .section .note.GNU-stack,"",@progbits
