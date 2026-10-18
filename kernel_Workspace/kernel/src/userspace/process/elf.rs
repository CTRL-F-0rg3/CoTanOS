//! A minimal static ELF64 loader for the ring-3 userspace.
//!
//! It loads exactly what the `ustd` ring-3 binaries produce: a non-PIE x86_64
//! ELF with page-aligned `PT_LOAD` segments. Each segment is copied into
//! freshly allocated frames and mapped into a new user address space; the BSS
//! (the gap between `p_filesz` and `p_memsz`) is zeroed.

use crate::mm::space::{AddressSpace, ProtFlags, PROT_EXEC, PROT_READ, PROT_USER, PROT_WRITE};

/// The kernel's physical direct-map base (same constant the rest of the kernel
/// uses to reach physical memory by adding the physical address).
const DIRECT_BASE: u64 = 0xFFFF888000000000;

/// A loaded ELF image: an address space and its entry point.
pub struct Loaded {
    pub aspace: AddressSpace,
    pub entry: u64,
}

fn u16_at(b: &[u8], off: usize) -> u16 {
    u16::from_le_bytes([b[off], b[off + 1]])
}

fn u32_at(b: &[u8], off: usize) -> u32 {
    u32::from_le_bytes([b[off], b[off + 1], b[off + 2], b[off + 3]])
}

fn u64_at(b: &[u8], off: usize) -> u64 {
    let mut a = [0u8; 8];
    a.copy_from_slice(&b[off..off + 8]);
    u64::from_le_bytes(a)
}

/// Load a static ELF image. On success the returned [`Loaded`] owns a fresh
/// address space with every `PT_LOAD` segment mapped.
pub fn load(buf: &[u8]) -> Result<Loaded, &'static str> {
    if buf.len() < 64 {
        return Err("elf: too small");
    }
    if &buf[0..4] != b"\x7fELF" {
        return Err("elf: bad magic");
    }
    if buf[4] != 2 {
        return Err("elf: not 64-bit");
    }
    if buf[5] != 1 {
        return Err("elf: not little-endian");
    }
    if u16_at(buf, 16) != 2 {
        return Err("elf: not an executable");
    }
    if u16_at(buf, 18) != 0x3E {
        return Err("elf: not x86_64");
    }

    let entry = u64_at(buf, 24);
    let phoff = u64_at(buf, 32) as usize;
    let phentsize = u16_at(buf, 54) as usize;
    let phnum = u16_at(buf, 56) as usize;

    let aspace = AddressSpace::new().ok_or("elf: aspace create failed")?;

    for i in 0..phnum {
        let off = phoff + i * phentsize;
        if off + 56 > buf.len() {
            return Err("elf: phdr out of range");
        }

        let p_type = u32_at(buf, off);
        let p_flags = u32_at(buf, off + 4);
        let p_offset = u64_at(buf, off + 8) as usize;
        let p_vaddr = u64_at(buf, off + 16);
        let p_filesz = u64_at(buf, off + 32) as usize;
        let p_memsz = u64_at(buf, off + 40) as usize;

        if p_type != 1 {
            continue; // PT_LOAD only
        }
        if p_memsz == 0 {
            continue;
        }
        if p_offset + p_filesz > buf.len() {
            return Err("elf: segment data out of range");
        }

        let map_va = p_vaddr & !0xFFFu64;
        let in_page = (p_vaddr - map_va) as usize;
        let total = in_page + p_memsz;
        let pages = (total + 4095) / 4096;

        let phys = crate::mm::phys::alloc_frames(pages).ok_or("elf: frame alloc failed")?;

        // Copy the file bytes into the direct-mapped frames.
        if p_filesz > 0 {
            let dst = (DIRECT_BASE + phys + in_page as u64) as *mut u8;
            // SAFETY: `dst` is a valid direct-map alias of freshly allocated
            // frames; `p_offset + p_filesz` was bounds-checked above.
            unsafe {
                core::ptr::copy_nonoverlapping(buf.as_ptr().add(p_offset), dst, p_filesz);
            }
        }

        // Zero the BSS (the bytes after the file data up to the memory size).
        if p_memsz > p_filesz {
            let dst = (DIRECT_BASE + phys + (in_page + p_filesz) as u64) as *mut u8;
            // SAFETY: same reasoning as above; the range is within the frames.
            unsafe {
                core::ptr::write_bytes(dst, 0, p_memsz - p_filesz);
            }
        }

        // Build the page permissions from the ELF flags, always user-visible.
        let mut prot: ProtFlags = PROT_USER;
        if p_flags & 4 != 0 {
            prot = prot | PROT_READ;
        }
        if p_flags & 2 != 0 {
            prot = prot | PROT_WRITE;
        }
        if p_flags & 1 != 0 {
            prot = prot | PROT_EXEC;
        }

        if !aspace.map_phys(map_va, phys, pages * 4096, prot) {
            return Err("elf: map failed");
        }
    }

    Ok(Loaded { aspace, entry })
}
