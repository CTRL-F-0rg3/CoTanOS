//! `tfsinject` — write ring-3 ELF binaries into a TrangorgeOS TFS data image.
//!
//! The kernel formats `data.img` with TFS on first boot; this tool does the
//! same formatting on the *host* and then writes `/bin/*.elf` files, so the
//! userspace shell's `titanus` command and the ring-3 init (`/bin/init.elf`)
//! can find them. It mirrors `kernel/src/fs/tangfs/tfs.rs` byte-for-byte.

use std::env;
use std::fs::{File, OpenOptions};
use std::io::{Read, Seek, SeekFrom, Write};
use std::path::{Path, PathBuf};

const BLOCK: usize = 512;
const MAGIC: [u8; 4] = *b"TFS1";
const DIR_BLOCKS: u32 = 16;
const DATA_START: u32 = 18;
const ENTRY_SIZE: usize = 64;
const PER_BLOCK: usize = BLOCK / ENTRY_SIZE;
const MAX_NAME: usize = 48;
const ROOT: u32 = 2;
const K_EMPTY: u8 = 0;
const K_FILE: u8 = 1;
const K_DIR: u8 = 2;

/// host binary name in `<bin-dir>`  ->  TFS path (relative to `/`).
const BINARIES: &[(&str, &str)] = &[
    ("titanus", "bin/init.elf"),
    ("titanus", "bin/titanus.elf"),
    ("tgs-pattern", "bin/pattern.elf"),
    ("tgs-plasma", "bin/plasma.elf"),
    ("tgs-clock", "bin/clock.elf"),
    ("tgs-paint", "bin/paint.elf"),
    ("mandel", "bin/mandel.elf"),
];

fn rd32(b: &[u8], off: usize) -> u32 {
    u32::from_le_bytes([b[off], b[off + 1], b[off + 2], b[off + 3]])
}
fn wr32(b: &mut [u8], off: usize, v: u32) {
    b[off..off + 4].copy_from_slice(&v.to_le_bytes());
}
fn rd64(b: &[u8], off: usize) -> u64 {
    u64::from_le_bytes([
        b[off], b[off + 1], b[off + 2], b[off + 3],
        b[off + 4], b[off + 5], b[off + 6], b[off + 7],
    ])
}
fn wr64(b: &mut [u8], off: usize, v: u64) {
    b[off..off + 8].copy_from_slice(&v.to_le_bytes());
}
fn fnv1a(b: &[u8]) -> u64 {
    let mut h: u64 = 0xcbf2_9ce4_8422_2325;
    for &x in b {
        h ^= x as u64;
        h = h.wrapping_mul(0x0000_0100_0000_01b3);
    }
    h
}

struct Tfs {
    f: File,
    total: u32,
    free: u32,
    count: u32,
    gen: u64,
}

impl Tfs {
    fn open(path: &Path) -> Tfs {
        let f = OpenOptions::new()
            .read(true)
            .write(true)
            .create(true)
            .open(path)
            .unwrap();
        let len = f.metadata().unwrap().len();
        let total = if len < (2 * BLOCK) as u64 {
            f.set_len(128 * 1024 * 1024).unwrap();
            (128 * 1024 * 1024 / BLOCK) as u32
        } else {
            (len / BLOCK as u64) as u32
        };

        let mut t = Tfs {
            f,
            total,
            free: DATA_START,
            count: 0,
            gen: 0,
        };
        t.format_if_blank();
        t
    }

    fn block(&mut self, blk: u32) -> [u8; BLOCK] {
        let mut b = [0u8; BLOCK];
        self.f
            .seek(SeekFrom::Start(blk as u64 * BLOCK as u64))
            .unwrap();
        self.f.read_exact(&mut b).unwrap();
        b
    }
    fn put(&mut self, blk: u32, b: &[u8; BLOCK]) {
        self.f
            .seek(SeekFrom::Start(blk as u64 * BLOCK as u64))
            .unwrap();
        self.f.write_all(b).unwrap();
    }

    fn format_if_blank(&mut self) {
        let sb = self.block(1);
        if &sb[0..4] == &MAGIC {
            self.total = rd32(&sb, 12);
            self.free = rd32(&sb, 24);
            self.count = rd32(&sb, 28);
            self.gen = rd64(&sb, 32);
            return;
        }
        eprintln!("[tfsinject] formatting fresh TFS image");
        self.write_sb();
        let zero = [0u8; BLOCK];
        for i in 0..DIR_BLOCKS {
            self.put(ROOT + i, &zero);
        }
    }

    fn write_sb(&mut self) {
        let mut sb = [0u8; BLOCK];
        sb[0..4].copy_from_slice(&MAGIC);
        wr32(&mut sb, 4, 1);
        wr32(&mut sb, 8, BLOCK as u32);
        wr32(&mut sb, 12, self.total);
        wr32(&mut sb, 16, DIR_BLOCKS);
        wr32(&mut sb, 20, DATA_START);
        wr32(&mut sb, 24, self.free);
        wr32(&mut sb, 28, self.count);
        self.gen = self.gen.wrapping_add(1);
        wr64(&mut sb, 32, self.gen);
        let hash = fnv1a(&sb[..40]);
        wr64(&mut sb, 40, hash);
        self.put(1, &sb);
    }

    fn find(&mut self, dir: u32, name: &str) -> Option<(u32, usize)> {
        for blk in 0..DIR_BLOCKS {
            let b = self.block(dir + blk);
            for i in 0..PER_BLOCK {
                let off = i * ENTRY_SIZE;
                if b[off + MAX_NAME] == K_EMPTY {
                    continue;
                }
                let slen = b[off..off + MAX_NAME]
                    .iter()
                    .position(|&c| c == 0)
                    .unwrap_or(MAX_NAME);
                if &b[off..off + slen] == name.as_bytes() {
                    return Some((dir + blk, i));
                }
            }
        }
        None
    }

    fn entry_slot(&mut self, dir: u32, name: &str) -> Result<(u32, usize), String> {
        for blk in 0..DIR_BLOCKS {
            let mut b = self.block(dir + blk);
            for i in 0..PER_BLOCK {
                let off = i * ENTRY_SIZE;
                if b[off + MAX_NAME] == K_EMPTY {
                    let mut nb = [0u8; MAX_NAME];
                    nb[..name.len()].copy_from_slice(name.as_bytes());
                    b[off..off + MAX_NAME].copy_from_slice(&nb);
                    self.put(dir + blk, &b);
                    return Ok((dir + blk, i));
                }
            }
        }
        Err("directory full".into())
    }

    fn mkdir(&mut self, parent: u32, name: &str) -> Result<u32, String> {
        if name.is_empty() || name.len() >= MAX_NAME {
            return Err("name too long".into());
        }
        if let Some((b, slot)) = self.find(parent, name) {
            let blk = self.block(b);
            let off = slot * ENTRY_SIZE;
            return Ok(rd32(&blk, off + 56));
        }
        if self.free + DIR_BLOCKS > self.total {
            return Err("disk full".into());
        }
        let first = self.free;
        let zero = [0u8; BLOCK];
        for i in 0..DIR_BLOCKS {
            self.put(first + i, &zero);
        }
        let (b, slot) = self.entry_slot(parent, name)?;
        let mut blk = self.block(b);
        let off = slot * ENTRY_SIZE;
        blk[off + MAX_NAME] = K_DIR;
        wr32(&mut blk, off + 52, 0);
        wr32(&mut blk, off + 56, first);
        self.put(b, &blk);
        self.free += DIR_BLOCKS;
        self.count += 1;
        self.write_sb();
        Ok(first)
    }

    fn write_file(&mut self, dir: u32, name: &str, data: &[u8]) -> Result<(), String> {
        if name.is_empty() || name.len() >= MAX_NAME {
            return Err("name too long".into());
        }
        if let Some((b, slot)) = self.find(dir, name) {
            let mut blk = self.block(b);
            blk[slot * ENTRY_SIZE + MAX_NAME] = K_EMPTY;
            self.put(b, &blk);
            self.count = self.count.saturating_sub(1);
        }
        let need = ((data.len() + 511) / 512).max(1) as u32;
        if self.free + need > self.total {
            return Err("disk full".into());
        }
        for i in 0..need {
            let mut blk = [0u8; BLOCK];
            let start = i as usize * 512;
            let end = (start + 512).min(data.len());
            if start < data.len() {
                blk[..end - start].copy_from_slice(&data[start..end]);
            }
            self.put(self.free + i, &blk);
        }
        let (b, slot) = self.entry_slot(dir, name)?;
        let mut blk = self.block(b);
        let off = slot * ENTRY_SIZE;
        blk[off + MAX_NAME] = K_FILE;
        wr32(&mut blk, off + 52, data.len() as u32);
        wr32(&mut blk, off + 56, self.free);
        self.put(b, &blk);
        self.free += need;
        self.count += 1;
        self.write_sb();
        Ok(())
    }

    fn resolve_dir(&mut self, path: &str) -> Result<u32, String> {
        let mut cur = ROOT;
        for part in path.split('/').filter(|s| !s.is_empty()) {
            cur = self.mkdir(cur, part)?;
        }
        Ok(cur)
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        eprintln!("usage: tfsinject <data.img> <bin-dir>");
        std::process::exit(2);
    }
    let img = PathBuf::from(&args[1]);
    let bin_dir = PathBuf::from(&args[2]);

    let mut tfs = Tfs::open(&img);
    let mut wrote = 0usize;

    for &(host, tfspath) in BINARIES {
        let mut src = bin_dir.clone();
        src.push(host);
        if !src.exists() {
            eprintln!("[tfsinject] skip (not built): {host}");
            continue;
        }
        let data = std::fs::read(&src).unwrap_or_else(|e| {
            eprintln!("[tfsinject] cannot read {host}: {e}");
            std::process::exit(1);
        });
        let (dir, file) = match tfspath.rsplit_once('/') {
            Some((d, f)) => (d, f),
            None => ("", tfspath),
        };
        let dblock = tfs.resolve_dir(dir).unwrap_or_else(|e| {
            eprintln!("[tfsinject] {tfspath}: {e}");
            std::process::exit(1);
        });
        tfs.write_file(dblock, file, &data).unwrap_or_else(|e| {
            eprintln!("[tfsinject] {tfspath}: {e}");
            std::process::exit(1);
        });
        eprintln!("[tfsinject] wrote /{tfspath} ({} bytes)", data.len());
        wrote += 1;
    }

    eprintln!("[tfsinject] {wrote} binary(s) in {img:?}");
}

