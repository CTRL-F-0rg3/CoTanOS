use super::elf;
use crate::trampoline_rings as tr;

const USER_STACK_TOP: u64 = 0x7FFF_0000_0000;

/// Ring-3 binaries are multi-megabyte (they link `std` or `tgs-gfx`), so the
/// loader buffer is a `.bss` array sized for the largest image the build can
/// produce — a stack array would both overflow the kernel stack and be far too
/// small for the real ELF, which `read_path` otherwise truncates.
static mut ELF_BUF: [u8; 16 * 1024 * 1024] = [0; 16 * 1024 * 1024];

pub fn spawn_elf(path: &str) -> Result<(), &'static str> {
    extern "C" { fn kprintf(fmt: *const u8, ...); }

    unsafe { kprintf(b"[spawn] step 1: read_path\n\0".as_ptr()); }
    let buf = unsafe { &mut ELF_BUF };

    let n = crate::fs::vfs::root()
        .and_then(|fs| fs.read_path(path, buf))
        .ok_or("elf not found in fs")?;
    unsafe { kprintf(b"[spawn] step 2: read done\n\0".as_ptr()); }

    let loaded = elf::load(&buf[..n])?;
    unsafe { kprintf(b"[spawn] step 3: elf loaded\n\0".as_ptr()); }

    let stack_phys = crate::mm::phys::alloc_frames(4)
        .ok_or("no stack mem")?;
    unsafe { kprintf(b"[spawn] step 4: stack alloc\n\0".as_ptr()); }

    let prot = crate::mm::space::PROT_READ
             | crate::mm::space::PROT_WRITE
             | crate::mm::space::PROT_USER;

    if !loaded.aspace.map_phys(USER_STACK_TOP - 0x4000,
                               stack_phys, 0x4000, prot) {
        return Err("stack map fail");
    }
    unsafe { kprintf(b"[spawn] step 5: stack mapped\n\0".as_ptr()); }

    tr::add_world(tr::RING_USER,
                  loaded.aspace.cr3(),
                  loaded.entry,
                  USER_STACK_TOP,
                  0)
        .ok_or("world table full")?;
    unsafe { kprintf(b"[spawn] step 6: world added\n\0".as_ptr()); }

    Ok(())
}

pub fn spawn_init() -> Result<(), &'static str> {
    spawn_elf("/bin/init.elf")?;

    extern "C" { fn kprintf(fmt: *const u8, ...); }
    unsafe {
        kprintf(b"[boot] userspace init spawned from /bin/init.elf (last stage)\n\0".as_ptr());
    }

    Ok(())
}