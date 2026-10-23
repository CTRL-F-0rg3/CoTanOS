.global loom_asm_fast_fill
.global loom_asm_fast_copy_nt
.text

# ---
# void loom_asm_fast_fill(uint32_t* dest, uint32_t color, size_t count);
# RDI = dest pointer, RSI = color (32-bit), RDX = pixel count (32-bit dwords)
# ---
loom_asm_fast_fill:
    mov     eax, esi
    mov     rcx, rdx
    cld
    rep     stosd
    ret

# ---
# void loom_asm_fast_copy_nt(uint32_t* dest, const uint32_t* src, size_t dword_count);
# RDI = dest pointer, RSI = src pointer, RDX = dword count (pixels)
# Uses SSE2 Non-Temporal instructions to prevent CPU cache pollution on large framebuffers.
# ---
loom_asm_fast_copy_nt:
    mov     rcx, rdx
    shr     rcx, 2          # Divide dword count by 4 (each XMM register holds 4 x 32-bit pixels = 16 bytes)
    jz      .L_tail         # If less than 4 pixels, jump directly to tail copy

.L_loop_16bytes:
    movdqu  xmm0, [rsi]     # Load 16 bytes (4 pixels) from source unaligned
    movntdq [rdi], xmm0     # Store non-temporal (bypasses L1/L2/L3 cache entirely)
    add     rsi, 16
    add     rdi, 16
    dec     rcx
    jnz     .L_loop_16bytes

    sfence                  # Serializes non-temporal store operations

.L_tail:
    # Handle remaining pixels (0 to 3 dwords) using standard scalar/rep copy
    mov     rcx, rdx
    and     rcx, 3
    jz      .L_done
    rep     movsd

.L_done:
    ret