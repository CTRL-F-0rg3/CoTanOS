%ifdef STAGE1
BITS 16
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti
    mov [boot_drive], dl

    ; wczytaj stage2 (64 sektory) na 0x8000:0
    mov si, dap
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jc  disk_err
    jmp 0x0000:0x8000

disk_err:
    mov si, msg
.p: lodsb
    test al, al
    jz .h
    mov ah, 0x0E
    int 0x10
    jmp .p
.h: hlt
    jmp .h

boot_drive: db 0
msg: db "disk error", 0

align 4
dap:
    db 0x10, 0
    dw 64
    dw 0x8000, 0x0000
    dq 1

times 510-($-$$) db 0
dw 0xAA55
%endif
