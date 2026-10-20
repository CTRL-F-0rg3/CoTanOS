use super::proc as P;
use crate::trampoline_rings as tr;

pub const SYS_EXIT: u64    = 0x1000;
pub const SYS_SPAWN: u64   = 0x1001;
pub const SYS_GETPID: u64  = 0x1002;
pub const SYS_IPC_SEND: u64 = 0x1010;
pub const SYS_IPC_RECV: u64 = 0x1011;
pub const SYS_KEY: u64     = 0x1040;
pub const SYS_MOUSE: u64   = 0x1041;
pub const SYS_MOUSE_RAW: u64 = 0x1042;
pub const SYS_READDIR: u64 = 0x1025;
pub const SYS_RUNCL: u64   = 0x1050;
pub const SYS_UI_OPEN: u64 = 0x1060;

// POSIX block (0x2000+), the numbers `ustd/hal/src/sysnum.rs` declares as the
// surface the ring-3 `std` runtime needs. Only the minimal set is wired up:
// memory (mmap/munmap for the heap), console I/O (write for `println!`), time,
// randomness and process exit. The rest fall through to `_ => u64::MAX`.
pub const SYS_MMAP: u64 = 0x2000;
pub const SYS_MUNMAP: u64 = 0x2001;
pub const SYS_WRITE: u64 = 0x2007;
pub const SYS_EXIT_GROUP: u64 = 0x2031;
pub const SYS_CLOCK_GETTIME: u64 = 0x2039;
pub const SYS_GETRANDOM: u64 = 0x203A;

pub const UI_FB_VA: u64   = 0x5000_0000;
pub const UI_FONT_VA: u64 = 0x6000_0000;
extern "C" {
    fn paging_translate_in(pml4: u64, virt: u64) -> u64;
    fn kprintf(fmt: *const u8, ...);
}

const DIRECT_BASE: u64 = 0xFFFF888000000000;

fn user_copy_in(cr3: u64, src: u64, dst: &mut [u8]) -> bool {
    let mut pos = 0usize;

    while pos < dst.len() {
        let va = src + pos as u64;
        let page = va & !0xFFFu64;
        let off = (va & 0xFFF) as usize;

        let phys = unsafe { paging_translate_in(cr3, page) };

        if phys == u64::MAX {
            return false;
        }

        let n = (4096 - off).min(dst.len() - pos);

        unsafe {
            let p = (DIRECT_BASE + phys + off as u64) as *const u8;
            core::ptr::copy_nonoverlapping(p, dst.as_mut_ptr().add(pos), n);
        }

        pos += n;
    }

    true
}

fn user_cstr(cr3: u64, ptr: u64, buf: &mut [u8]) -> Option<&str> {
    for i in 0..buf.len() {
        let mut b = [0u8; 1];

        if !user_copy_in(cr3, ptr + i as u64, &mut b) {
            return None;
        }

        if b[0] == 0 {
            return core::str::from_utf8(&buf[..i]).ok();
        }

        buf[i] = b[0];
    }

    None
}

/// Copy kernel bytes into the given user address space (across page walks).
fn user_copy_out(cr3: u64, dst: u64, src: &[u8]) -> bool {
    let mut pos = 0usize;

    while pos < src.len() {
        let va = dst + pos as u64;
        let page = va & !0xFFFu64;
        let off = (va & 0xFFF) as usize;

        let phys = unsafe { paging_translate_in(cr3, page) };

        if phys == u64::MAX {
            return false;
        }

        let n = (4096 - off).min(src.len() - pos);

        unsafe {
            let p = (DIRECT_BASE + phys + off as u64) as *mut u8;
            core::ptr::copy_nonoverlapping(src.as_ptr().add(pos), p, n);
        }

        pos += n;
    }

    true
}

/// The next anonymous mapping address the POSIX `mmap` hands out. It starts well
/// above the ring-3 image (which ends around 0x0062_0000, see `ustd/linker.ld`)
/// and grows upward, so the heap can expand without colliding with the image.
static mut MMAP_NEXT: u64 = 0x1000_0000;

/// `void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off)`.
///
/// Anonymous private mappings only, which is all `tgs-hal`'s heap asks for. The
/// address hint is ignored and a fresh mapping is placed at [`MMAP_NEXT`].
fn posix_mmap(cr3: u64, len: u64) -> u64 {
    extern "C" {
        fn paging_map_page_in(pml4: u64, virt: u64, phys: u64, flags: u64) -> bool;
    }

    let len = (len as usize).max(4096);
    let pages = (len + 4095) / 4096;

    let phys = match crate::mm::phys::alloc_frames(pages) {
        Some(p) => p,
        None => return u64::MAX,
    };

    let va = unsafe { MMAP_NEXT };
    // `0x1 | 0x2 | 0x4 | 0x8` is the same "present + writable + user + NX" flag
    // set the framebuffer mapping below uses; data mappings are not executable.
    for i in 0..pages {
        let ok = unsafe {
            paging_map_page_in(
                cr3,
                va + (i * 4096) as u64,
                phys + (i * 4096) as u64,
                0x1 | 0x2 | 0x4 | 0x8,
            )
        };
        if !ok {
            return u64::MAX;
        }
    }

    unsafe { MMAP_NEXT += (pages * 4096) as u64 };
    va
}

/// `int munmap(void *addr, size_t len)`. The frames are deliberately not freed
/// here — the heap hands out 64 KiB regions and only frees oversized ones, so a
/// no-op is correct for the init demo and keeps the page-table walk simple.
fn posix_munmap(_addr: u64, _len: u64) -> u64 {
    0
}

/// `ssize_t write(int fd, const void *buf, size_t count)`. fd 1/2 are the
/// console; anything else is refused. The bytes are copied out of the user
/// address space and handed to `kprintf` as a `%s` argument, so any `%` in the
/// text is printed literally rather than interpreted as a format specifier.
fn posix_write(cr3: u64, fd: u64, buf: u64, count: u64) -> u64 {
    if fd != 1 && fd != 2 {
        return u64::MAX;
    }

    let mut tmp = [0u8; 1024];
    let n = (count as usize).min(1023);
    if !user_copy_in(cr3, buf, &mut tmp[..n]) {
        return u64::MAX;
    }
    tmp[n] = 0;

    unsafe {
        kprintf(b"%s\0".as_ptr(), tmp.as_ptr());
    }

    count
}

/// `int clock_gettime(clockid_t clk, struct timespec *tp)`. A monotonic
/// software clock; the exact epoch is irrelevant for `Instant`/`SystemTime` in
/// the init demo — only that it advances.
fn posix_clock_gettime(cr3: u64, tp: u64) -> u64 {
    static mut NANOS: u64 = 0;

    let nsec = unsafe { NANOS };
    unsafe { NANOS += 1_000_000 };

    let mut ts = [0u8; 16];
    ts[0..8].copy_from_slice(&0u64.to_le_bytes()); // tv_sec
    ts[8..16].copy_from_slice(&nsec.to_le_bytes()); // tv_nsec

    if !user_copy_out(cr3, tp, &ts) {
        return u64::MAX;
    }
    0
}

/// `ssize_t getrandom(void *buf, size_t buflen, unsigned int flags)`. A tiny
/// LCG — enough for `HashMap`'s `RandomState` (it only needs a seed), not
/// cryptographically strong.
fn posix_getrandom(cr3: u64, buf: u64, buflen: u64) -> u64 {
    static mut SEED: u64 = 0x1234_5678_9abc_def0;

    let mut out = [0u8; 256];
    let n = (buflen as usize).min(256);
    for b in out[..n].iter_mut() {
        unsafe {
            SEED = SEED
                .wrapping_mul(6364136223846793005)
                .wrapping_add(1442695040888963407);
            *b = (SEED >> 33) as u8;
        }
    }

    if !user_copy_out(cr3, buf, &out[..n]) {
        return u64::MAX;
    }
    n as u64
}

static mut ELF_BUF: [u8; 16 * 1024 * 1024] = [0; 16 * 1024 * 1024];

fn do_spawn(cr3: u64, path_ptr: u64, parent_pid: u32) -> i64 {
    let mut tmp = [0u8; 128];

    let path = match user_cstr(cr3, path_ptr, &mut tmp) {
        Some(s) => s,
        None => return -1,
    };

    let buf = unsafe { &mut ELF_BUF };

    let n = match crate::fs::vfs::root() {
        Some(fs) => match fs.read_path(path, buf) {
            Some(n) => n,
            None => return -1,
        },
        None => return -1,
    };

    let loaded = match super::elf::load(&buf[..n]) {
        Ok(l) => l,
        Err(_) => return -1,
    };

    let stack_phys = match crate::mm::phys::alloc_frames(4) {
        Some(p) => p,
        None => return -1,
    };

    let prot = crate::mm::space::PROT_READ
             | crate::mm::space::PROT_WRITE
             | crate::mm::space::PROT_USER;

    if !loaded.aspace.map_phys(tr::USER_STACK_TOP - 0x4000,
                               stack_phys, 0x4000, prot) {
        return -1;
    }

    let world = match tr::add_world(tr::RING_USER,
                                    loaded.aspace.cr3(),
                                    loaded.entry,
                                    tr::USER_STACK_TOP, 0) {
        Some(w) => w,
        None => return -1,
    };

    match P::register(world, parent_pid) {
        Some(pid) => pid as i64,
        None => -1,
    }
}

pub fn handle(world: usize, c: &mut tr::CpuCtx) {
    let cr3 = tr::world_cr3(world);

    let me = P::by_world(world)
        .map(|p| p.pid)
        .unwrap_or(0);

    let num = c.rax;
    let a0 = c.rdi;
    let a1 = c.rsi;
    let a2 = c.rdx;

    c.rax = match num {
        SYS_EXIT => tr::exit_from(world, c, a0 as i32),

        SYS_SPAWN => do_spawn(cr3, a0, me) as u64,

        SYS_GETPID => me as u64,

        SYS_IPC_SEND => {
            let msg = P::IpcMsg {
                from: me,
                a0: a1,
                a1: a2,
                a2: 0,
                a3: 0,
            };

            if P::send(a0 as u32, msg) { 0 } else { u64::MAX }
        }

        SYS_IPC_RECV => {
            match P::recv(me) {
                Some(m) => {
                    c.rdi = m.a0;
                    c.rsi = m.a1;
                    m.from as u64
                }
                None => u64::MAX,
            }
        }

        SYS_KEY => {
            match crate::drivers::usb::class::hid::keyboard::take_char() {
                Some(ch) => ch as u64,
                None => 0,
            }
        }
        SYS_MOUSE => {
            // Poll the PS/2 mouse driver. `mouse_init` is idempotent-ish but
            // resets the device, so it is run once. Returns the button mask in
            // `rax` and sign-extended dx/dy in `rdi`/`rsi`; `rax == u64::MAX`
            // means no packet was ready.
            extern "C" {
                fn mouse_init();
                fn mouse_poll(
                    dx: *mut i32,
                    dy: *mut i32,
                    dz: *mut i32,
                    buttons: *mut i32,
                ) -> i32;
            }

            static mut MOUSE_READY: bool = false;

            unsafe {
                if !MOUSE_READY {
                    mouse_init();
                    MOUSE_READY = true;
                }

                let (mut dx, mut dy, mut dz, mut btn) = (0i32, 0i32, 0i32, 0i32);
                if mouse_poll(&mut dx, &mut dy, &mut dz, &mut btn) == 1 {
                    c.rdi = (dx as i64) as u64;
                    c.rsi = (dy as i64) as u64;
                    btn as u64
                } else {
                    u64::MAX
                }
            }
        }
        SYS_MOUSE_RAW => {
            // One raw PS/2 mouse byte, straight from the controller. The
            // packet decode (assembly, sign extension, buttons) is a
            // *userspace* driver (`tgs-hal/src/mouse.rs`); this path only
            // hands over bytes and does not call `editor/mouse.c`.
            crate::ps2mouse::init_once();
            match crate::ps2mouse::raw_byte() {
                Some(b) => b as u64,
                None => u64::MAX,
            }
        }
        SYS_READDIR => {
            let idx = a0 as usize;
            let cap = (a2 as usize).min(127);

            match crate::fs::vfs::root().and_then(|fs| fs.list_path("/")) {
                Some(v) if idx < v.len() => {
                    let e = &v[idx];
                    let bytes = &e.name[..];
                    let n = bytes.len().min(cap);

                    if !user_copy_out(cr3, a1, &bytes[..n]) {
                        return c.rax = u64::MAX;
                    }

                    let _ = user_copy_out(cr3, a1 + n as u64, &[0u8]);

                    c.rsi = e.size;

                    if e.is_dir { 2 } else { 1 }
                }
                _ => 0,
            }
        }

        // The triang-lang VM (`runcl`) is a host-side crate and is not linked
        // into the kernel, so this syscall is reported as unsupported rather
        // than left to fail at link time.
        SYS_RUNCL => u64::MAX,

        SYS_UI_OPEN => {
            extern "C" {
                fn hdmi_caps_raw(w: *mut u32, h: *mut u32,
                                 s: *mut u32, phys: *mut u64);
                fn kvirt_to_phys(ptr: *mut core::ffi::c_void) -> u64;
                fn paging_map_page_in(pml4: u64, virt: u64,
                                      phys: u64, flags: u64) -> bool;
            }

            extern "C" {
                static font8x8: u8;
            }

            unsafe {
                let (mut w, mut h, mut s, mut fp) = (0u32, 0u32, 0u32, 0u64);

                hdmi_caps_raw(&mut w, &mut h, &mut s, &mut fp);

                if fp == 0 || w == 0 {
                    return c.rax = u64::MAX;
                }

                /* fb: RW + user + NX (W^X) */
                let fb_bytes = (s * h * 4) as u64;
                let pages = (fb_bytes + 4095) / 4096;

                for i in 0..pages {
                    paging_map_page_in(cr3,
                                       UI_FB_VA + i * 4096,
                                       fp + i * 4096,
                                       0x1 | 0x2 | 0x4 | 0x8);
                }

                /* font: RO + user + NX */
                let font_phys =
                    kvirt_to_phys(&font8x8 as *const u8 as *mut core::ffi::c_void);

                paging_map_page_in(cr3, UI_FONT_VA, font_phys,
                                   0x1 | 0x4 | 0x8);

                c.rdi = ((w as u64) << 16) | h as u64;
                c.rsi = s as u64;
                c.rdx = UI_FB_VA;

                0
            }
        }

        SYS_MMAP => posix_mmap(cr3, a1),

        SYS_MUNMAP => posix_munmap(a0, a1),

        SYS_WRITE => posix_write(cr3, a0, a1, a2),

        SYS_EXIT_GROUP => tr::exit_from(world, c, a0 as i32),

        SYS_CLOCK_GETTIME => posix_clock_gettime(cr3, a1),

        SYS_GETRANDOM => posix_getrandom(cr3, a0, a1),

        _ => u64::MAX,
    };
}

