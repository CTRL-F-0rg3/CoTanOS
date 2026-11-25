; kernel_abi.inc — wspólny kontrakt BIOS/UEFI <-> kernel
%define KERNEL_LOAD_ADDR   0x100000    ; 1 MiB
%define KERNEL_STACK_TOP   0x90000     ; pod PML4/PT

; Stan rejestrów przy skoku do kernela (long mode):
;   RDI = wskaźnik na boot_info (patrz niżej)
;   RSI = magic "BOOT64!!"
;   wszystkie inne rejestry = 0 (żeby kernel nie zależał od śmieci)
;   RSP = KERNEL_STACK_TOP
;   IF = 0

; boot_info (32 B, wyrównane do 8):
struc boot_info
    .magic      resq 1   ; 'BOOT64!!'
    .mem_lower  resd 1
    .mem_upper  resd 1
    .fb_ptr     resq 1   ; 0 = brak framebuffera
    .fb_width   resd 1
    .fb_height  resd 1
    ._pad       resd 1
endstruc
%define BOOT_INFO_MAGIC 0x424F4F5436342121  ; "BOOT64!!"
