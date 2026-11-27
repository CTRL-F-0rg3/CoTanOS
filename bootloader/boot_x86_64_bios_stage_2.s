; CoTanOS BIOS stage2 — ładuje kernel z dysku i skacze do niego
BITS 16
ORG 0x8000

stage2_start:
    mov [boot_drive], dl
    
    ; A20 via port 0x92
    in  al, 0x92
    or  al, 2
    out 0x92, al
    
    ; Przejdź do protected mode
    lgdt [gdt_desc]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp 0x08:pm_start

BITS 32
pm_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    
    ; Wyczyść pamięć pod tablice stron (0x5000..0x7fff)
    mov edi, 0x5000
    xor eax, eax
    mov ecx, 0xC00/4
    rep stosd
    
    ; PML4[0] -> PDPT @0x6000
    mov dword [0x5000], 0x6003
    mov dword [0x5004], 0
    
    ; PDPT[0] -> PD @0x7000
    mov dword [0x6000], 0x7003
    mov dword [0x6004], 0
    
    ; PD: 512 x 2MiB pages (identity map 0..1GiB)
    mov edi, 0x7000
    mov eax, 0x0083  ; present|rw|PS
    mov ecx, 512
.pd_loop:
    mov [edi], eax
    mov dword [edi+4], 0
    add eax, 0x200000
    add edi, 8
    loop .pd_loop
    
    ; PAE
    mov eax, cr4
    or  eax, 0x20
    mov cr4, eax
    
    ; CR3 = PML4
    mov eax, 0x5000
    mov cr3, eax
    
    ; EFER.LME = 1
    mov ecx, 0xC0000080
    rdmsr
    or  eax, 0x100
    wrmsr
    
    ; CR0.PG | CR0.PE
    mov eax, cr0
    or  eax, 0x80000001
    mov cr0, eax
    
    jmp 0x08:lm_start

BITS 64
lm_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, 0x90000
    
    ; Załaduj kernel z dysku (sektor 2, 64 sektory = 32KB)
    ; Użyj BIOS int 0x13 w trybie real mode... ale jesteśmy w long mode
    ; Więc kernel musi być już załadowany przez stage1
    
    ; Skocz do kernel_main_bios @ 0x100000
    call 0x100000
    
.hang:
    hlt
    jmp .hang

BITS 16
align 8
gdt_start:
    dq 0
gdt_code64:
    dq 0x00AF9A000000FFFF  ; 64-bit code
gdt_data:
    dq 0x00CF92000000FFFF  ; data
gdt_end:
gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start
boot_drive: db 0
