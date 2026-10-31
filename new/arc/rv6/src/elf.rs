// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
//! CoTanOS 256xtr — ELF64 loader (RISC-V, static ET_EXEC / ET_DYN).
//!
//! The Linux-compatible userspace ABI means the kernel loads ordinary
//! `riscv64-linux` static binaries (glibc/musl C, C++ and Rust std builds).

use alloc::vec::Vec;

use crate::page::{PageTable, PTE_A, PTE_D, PTE_U, PTE_V, PTE_R, PTE_W, PTE_X};
use crate::proc::Region;

/// Load bias for position-independent (ET_DYN) images.
pub const PIE_BASE: usize = 0x4000_0000;

pub struct LoadedElf {
    pub entry: usize,
    pub brk: usize,
    pub phdr: usize,
    pub phent: usize,
    pub phnum: usize,
    pub base: usize,
}

fn rd_u16(b: &[u8], off: usize) -> u16 {
    u16::from_le_bytes([b[off], b[off + 1]])
}
fn rd_u32(b: &[u8], off: usize) -> u32 {
    u32::from_le_bytes([b[off], b[off + 1], b[off + 2], b[off + 3]])
}
fn rd_u64(b: &[u8], off: usize) -> u64 {
    u64::from_le_bytes([
        b[off], b[off + 1], b[off + 2], b[off + 3], b[off + 4], b[off + 5],
        b[off + 6], b[off + 7],
    ])
}

/// Loads a RISC-V ELF64 image into `pt`; records its regions.
pub fn load(data: &[u8], pt: &PageTable, regions: &mut Vec<Region>) -> Option<LoadedElf> {
    // Ident header: magic, 64-bit (2), little-endian (1).
    if data.len() < 64 || &data[0..4] != b"\x7fELF" || data[4] != 2 || data[5] != 1 {
        crate::console::console_puts("[elf] bad ident\n");
        return None;
    }
    // Machine: EM_RISCV (243).
    if rd_u16(data, 18) != 243 {
        crate::console::console_puts("[elf] bad machine\n");
        return None;
    }
    let e_type = rd_u16(data, 16);
    let e_entry = rd_u64(data, 24) as usize;
    let e_phoff = rd_u64(data, 32) as usize;
    let e_phentsize = rd_u16(data, 54) as usize;
    let e_phnum = rd_u16(data, 56) as usize;
    crate::console::console_puts("[elf] type=");
    crate::console::put_dec(e_type as u64);
    crate::console::console_puts(" phnum=");
    crate::console::put_dec(e_phnum as u64);
    crate::console::console_puts("\n");
    let base = if e_type == 3 { PIE_BASE } else { 0 };

    let mut entry = 0usize;
    let mut max_end = 0usize;
    let mut phdr_va = 0usize;

    for i in 0..e_phnum {
        let off = e_phoff + i * e_phentsize;
        if off + 56 > data.len() {
            crate::console::console_puts("[elf] phdr out of range\n");
            return None;
        }
        let p_type = rd_u32(data, off);
        if p_type != 1 {
            continue; // PT_LOAD only
        }
        let p_offset = rd_u64(data, off + 8) as usize;
        let p_vaddr = rd_u64(data, off + 16) as usize + base;
        let p_filesz = rd_u64(data, off + 32) as usize;
        let p_memsz = rd_u64(data, off + 40) as usize;
        let p_flags = rd_u32(data, off + 4) as usize;

        // The phdr table lives inside the segment covering e_phoff —
        // record its runtime VA for AT_PHDR.
        if e_phoff >= p_offset && e_phoff < p_offset + p_filesz {
            phdr_va = base + p_vaddr + (e_phoff - p_offset);
        }

        let mut flags = PTE_V | PTE_U | PTE_A | PTE_D;
        if p_flags & 4 != 0 {
            flags |= PTE_R;
        }
        if p_flags & 2 != 0 {
            flags |= PTE_W;
        }
        if p_flags & 1 != 0 {
            flags |= PTE_X;
        }

        let start = p_vaddr & !0xFFF;
        let end = (p_vaddr + p_memsz + 0xFFF) & !0xFFF;
        for va in (start..end).step_by(0x1000) {
            let f = crate::proc::allocator().allocate_frame()?;
            let pa = f.start_address().as_usize();
            // The frame may be recycled from a previous process — the
            // uninitialised .bss MUST be zeroed before copying.
            unsafe { core::ptr::write_bytes(pa as *mut u8, 0, 0x1000) };
            // Copy this page's bytes from the file. The segment may start
            // mid-page (p_vaddr unaligned): bytes below p_vaddr stay zero,
            // the tail beyond p_filesz stays zero.
            for k in 0..0x1000 {
                let va_k = va + k;
                if va_k < p_vaddr {
                    continue; // gap before the segment start
                }
                let so = va_k - p_vaddr; // byte offset within the segment
                if so >= p_filesz {
                    break; // beyond the file data
                }
                let file_off = p_offset + so;
                if file_off < data.len() {
                    // SAFETY: pa is an identity-mapped, allocated frame.
                    unsafe {
                        *((pa + k) as *mut u8) = data[file_off];
                    }
                }
            }
            if pt.map(va, pa, flags, crate::proc::allocator()).is_err() {
                crate::console::console_puts("[elf] map failed\n");
                return None;
            }
            regions.push(Region {
                vs: va,
                ve: va + 0x1000,
                prot: (p_flags & 7) as u8,
            });
        }
        if end > max_end {
            max_end = end;
        }
    }

    entry = base + e_entry;
    let loaded = LoadedElf {
        entry,
        brk: max_end,
        phdr: phdr_va,
        phent: e_phentsize,
        phnum: e_phnum,
        base,
    };
    apply_relocations(data, base);
    Some(loaded)
}

/// Applies the static relocations (R_RISCV_RELATIVE / R_RISCV_IRELATIVE)
/// found in the section headers — exactly what the Linux kernel does for
/// statically linked binaries without an interpreter. For IRELATIVE the
/// resolver address is stored; glibc's startup code (`apply_irel`) calls
/// the resolvers at the right moment.
fn apply_relocations(data: &[u8], base: usize) {
    if data.len() < 64 {
        return;
    }
    let e_shoff = rd_u64(data, 40) as usize;
    let e_shentsize = rd_u16(data, 58) as usize;
    let e_shnum = rd_u16(data, 60) as usize;
    if e_shentsize == 0 || e_shnum == 0 || e_shoff == 0 {
        return;
    }

    for i in 0..e_shnum {
        let soff = e_shoff + i * e_shentsize;
        if soff + 64 > data.len() {
            return;
        }
        let sh_type = rd_u32(data, soff + 4);
        if sh_type != 4 {
            continue; // SHT_RELA only
        }
        let sh_offset = rd_u64(data, soff + 24) as usize;
        let sh_size = rd_u64(data, soff + 32) as usize;
        let sh_entsize = rd_u64(data, soff + 56) as usize;
        if sh_entsize == 0 {
            continue;
        }
        for eoff in (sh_offset..sh_offset + sh_size).step_by(sh_entsize) {
            if eoff + 24 > data.len() {
                break;
            }
            let r_offset = rd_u64(data, eoff) as usize;
            let r_info = rd_u64(data, eoff + 8);
            let r_addend = rd_u64(data, eoff + 16) as usize;
            let r_type = r_info as u32;
            // 3 = R_RISCV_RELATIVE, 58 = R_RISCV_IRELATIVE.
            if r_type == 3 || r_type == 58 {
                // SAFETY: the target VA was mapped for this image.
                unsafe { *((base + r_offset) as *mut usize) = base + r_addend };
            }
        }
    }
}
