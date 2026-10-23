.global loom_asm_tile_diff_32x32
.global loom_asm_tile_copy_32x32
.global loom_asm_lfsr_step
.global loom_asm_batch_blit_tiles
.text

# ==============================================================================
# 1. loom_asm_tile_diff_32x32
# Args:
#   RDI = current tile buffer pointer (32x32 dwords = 4096 bytes)
#   RSI = previous tile buffer pointer (32x32 dwords = 4096 bytes)
# Returns:
#   EAX = 1 if tile changed, 0 if identical
# Optimized using 16-byte XMM chunks (1024 iterations total)
# ==============================================================================
loom_asm_tile_diff_32x32:
    xor     eax, eax                # Default return: 0 (no difference)
    mov     rcx, 256                # 256 chunks of 16 bytes = 4096 bytes (32x32 dwords)

.L_diff_loop:
    movdqu  xmm0, [rdi]             # Load current chunk
    movdqu  xmm1, [rsi]             # Load previous chunk
    pcmpeqd xmm0, xmm1              # Compare dwords for equality (all bits 1 if equal)
    pmovmskb edx, xmm0              # Extract mask of matching bytes
    cmp     edx, 0xFFFF             # Are all 16 bytes identical?
    jne     .L_found_difference     # If not equal, flag tile as modified

    add     rdi, 16
    add     rsi, 16
    dec     rcx
    jnz     .L_diff_loop
    ret

.L_found_difference:
    mov     eax, 1                  # Return 1: tile has changes
    ret

# ==============================================================================
# 2. loom_asm_tile_copy_32x32
# Args:
#   RDI = destination framebuffer tile pointer
#   RSI = source tile buffer pointer
# Copies a 32x32 tile using SSE2 Non-Temporal stores (bypassing L1/L2 cache)
# ==============================================================================
loom_asm_tile_copy_32x32:
    mov     rcx, 256                # 256 chunks of 16 bytes

.L_copy_loop:
    movdqu  xmm0, [rsi]             # Load source 16 bytes
    movntdq [rdi], xmm0             # Stream directly to memory (Non-Temporal)
    add     rsi, 16
    add     rdi, 16
    dec     rcx
    jnz     .L_copy_loop

    sfence                          # Memory barrier for non-temporal writes
    ret

# ==============================================================================
# 3. loom_asm_lfsr_step
# Args:
#   RDI = pointer to 32-bit LFSR seed state
# Returns:
#   EAX = next pseudorandom noise value
# Polynomial: x^32 + x^31 + x^29 + x^1 + 1 (Galois configuration)
# ==============================================================================
loom_asm_lfsr_step:
    mov     eax, [rdi]              # Load current LFSR state
    test    eax, eax
    jnz     .L_lfsr_nonzero
    mov     eax, 0xACE13F51         # Fallback seed if zero
.L_lfsr_nonzero:
    mov     edx, eax                # Copy state for shift operations
    shr     edx, 1                  # Shift right
    bt      eax, 0                  # Check lowest bit
    jnc     .L_lfsr_store
    xor     edx, 0x80000057         # Galois feedback polynomial mask

.L_lfsr_store:
    mov     [rdi], edx              # Save new LFSR state
    mov     eax, edx                # Return new state
    ret

# ==============================================================================
# 4. loom_asm_batch_blit_tiles
# Args:
#   RDI = array of destination tile pointers
#   RSI = array of source tile pointers
#   RDX = number of tiles to blit in batch
# ==============================================================================
loom_asm_batch_blit_tiles:
    test    rdx, rdx
    jz      .L_batch_done
    push    rbx
    mov     rbx, rdx                # Counter

.L_batch_loop:
    mov     rax, [rdi]              # Get dest pointer
    mov     rcx, [rsi]              # Get source pointer
    
    # Inner copy of one 32x32 tile (4096 bytes)
    push    rcx
    push    rax
    push    rbx
    push    rsi
    push    rdi

    mov     rdi, rax
    mov     rsi, rcx
    call    loom_asm_tile_copy_32x32

    pop     rdi
    pop     rsi
    pop     rbx
    pop     rcx
    pop     rax

    add     rdi, 8                  # Next pointer in array
    add     rsi, 8                  # Next pointer in array
    dec     rbx
    jnz     .L_batch_loop

    pop     rbx
.L_batch_done:
    ret