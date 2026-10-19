//! System-management commands: the machine, the session, and file utilities
//! that round out the Linux set already in `files`/`text`/`system`.
//!
//! Each one follows the same contract as the rest of [`super`]: a [`Ctx`] and a
//! raw argument string, returning a [`CmdResult`]. Nothing here reaches for the
//! shell, so a test drives any of them through a [`crate::apps::shell::MemoryFs`].

use alloc::string::{String, ToString};
use alloc::vec;
use alloc::vec::Vec;

use super::{CmdResult, Ctx};

/// Split `args` into exactly two whitespace-separated words.
fn two(args: &str) -> Option<(&str, &str)> {
    let mut it = args.split_whitespace();
    let a = it.next()?;
    let b = it.next()?;
    Some((a, b))
}

/// `hostname` — the machine name, as the kernel reports it.
pub fn hostname(ctx: &mut Ctx<'_>, _args: &str) -> CmdResult {
    CmdResult::ok(vec![ctx.sys.uname()])
}

/// `nproc` — how many CPUs the machine has.
pub fn nproc(ctx: &mut Ctx<'_>, _args: &str) -> CmdResult {
    CmdResult::ok(vec![ctx.sys.cpus().to_string()])
}

/// `arch` — the instruction set this kernel was built for.
pub fn arch(_ctx: &mut Ctx<'_>, _args: &str) -> CmdResult {
    CmdResult::ok(vec!["x86_64".to_string()])
}

/// `env` — the session environment: who, what rung, and where.
pub fn env(ctx: &mut Ctx<'_>, _args: &str) -> CmdResult {
    CmdResult::ok(vec![
        format!("user={}", ctx.session.user),
        format!("rung={}", ctx.session.rung),
        format!("home={}", ctx.session.home),
        format!("cwd={}", ctx.session.cwd),
    ])
}

/// `du` — disk usage: the summed size of files under a path, recursive.
pub fn du(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let target = args.split_whitespace().next().unwrap_or(".");
    let start = ctx.resolve(target);
    let mut total = 0u64;
    match du_walk(ctx, &start, &mut total) {
        Ok(()) => CmdResult::ok(vec![format!("{total}\t{start}")]),
        Err(e) => CmdResult::err(format!("du: {start}: {e}")),
    }
}

fn du_walk(ctx: &mut Ctx<'_>, path: &str, total: &mut u64) -> Result<(), String> {
    for e in ctx.fs.read_dir(path)? {
        let full = format!("{}/{}", path.trim_end_matches('/'), e.name);
        if e.is_dir {
            du_walk(ctx, &full, total)?;
        } else {
            *total += e.size as u64;
        }
    }
    Ok(())
}

/// `stat` — what a path is: a file with a size, or a directory with a count.
pub fn stat(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some(name) = args.split_whitespace().next() else {
        return CmdResult::err("stat: usage: stat <path>");
    };
    let path = ctx.resolve(name);
    if let Ok(entries) = ctx.fs.read_dir(&path) {
        return CmdResult::ok(vec![format!("{path}: directory, {} entries", entries.len())]);
    }
    match ctx.fs.read_file(&path) {
        Ok(data) => CmdResult::ok(vec![format!("{path}: file, {} bytes", data.len())]),
        Err(e) => CmdResult::err(format!("stat: {path}: {e}")),
    }
}

/// `cmp` — byte-for-byte comparison. Silent when equal.
pub fn cmp(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some((a, b)) = two(args) else {
        return CmdResult::err("cmp: usage: cmp <file> <file>");
    };
    let (pa, pb) = (ctx.resolve(a), ctx.resolve(b));
    let da = match ctx.fs.read_file(&pa) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("cmp: {a}: {e}")),
    };
    let db = match ctx.fs.read_file(&pb) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("cmp: {b}: {e}")),
    };
    let n = da.len().min(db.len());
    for i in 0..n {
        if da[i] != db[i] {
            return CmdResult::err(format!("cmp: {a} {b} differ: byte {i}"));
        }
    }
    if da.len() != db.len() {
        return CmdResult::err(format!(
            "cmp: {a} {b} differ: length {} vs {}",
            da.len(),
            db.len()
        ));
    }
    CmdResult::silent()
}

/// `shred` — overwrite a file with zeros, then remove it.
pub fn shred(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some(name) = args.split_whitespace().next() else {
        return CmdResult::err("shred: usage: shred <file>");
    };
    let path = ctx.resolve(name);
    if !ctx.allowed(&path) {
        return CmdResult::err("shred: outside this session's home");
    }
    let len = match ctx.fs.read_file(&path) {
        Ok(d) => d.len(),
        Err(e) => return CmdResult::err(format!("shred: {path}: {e}")),
    };
    if let Err(e) = ctx.fs.write_file(&path, &vec![0u8; len]) {
        return CmdResult::err(format!("shred: {path}: {e}"));
    }
    match ctx.fs.remove(&path) {
        Ok(()) => CmdResult::silent(),
        Err(e) => CmdResult::err(format!("shred: {path}: {e}")),
    }
}

/// `truncate` — set a file to exactly `size` bytes (pad with zeros, or cut).
pub fn truncate(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let mut it = args.split_whitespace();
    let (Some(name), Some(size)) = (it.next(), it.next()) else {
        return CmdResult::err("truncate: usage: truncate <file> <size>");
    };
    let size: usize = match size.parse() {
        Ok(n) => n,
        Err(_) => return CmdResult::err("truncate: size must be a number"),
    };
    let path = ctx.resolve(name);
    let mut data = match ctx.fs.read_file(&path) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("truncate: {path}: {e}")),
    };
    data.resize(size, 0);
    match ctx.fs.write_file(&path, &data) {
        Ok(()) => CmdResult::silent(),
        Err(e) => CmdResult::err(format!("truncate: {path}: {e}")),
    }
}

/// `cksum` — a byte-sum checksum and a byte count.
pub fn cksum(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some(name) = args.split_whitespace().next() else {
        return CmdResult::err("cksum: usage: cksum <file>");
    };
    let path = ctx.resolve(name);
    let data = match ctx.fs.read_file(&path) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("cksum: {path}: {e}")),
    };
    let sum: u32 = data.iter().map(|&b| b as u32).sum();
    CmdResult::ok(vec![format!("{sum} {} {path}", data.len())])
}

/// `hexdump` — a hex and ASCII view of a file, sixteen bytes per line.
pub fn hexdump(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some(name) = args.split_whitespace().next() else {
        return CmdResult::err("hexdump: usage: hexdump <file>");
    };
    let path = ctx.resolve(name);
    let data = match ctx.fs.read_file(&path) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("hexdump: {path}: {e}")),
    };
    let mut out = Vec::new();
    for (i, chunk) in data.chunks(16).enumerate() {
        let mut hex = String::new();
        for (j, b) in chunk.iter().enumerate() {
            if j == 8 {
                hex.push(' ');
            }
            hex.push_str(&format!("{b:02x} "));
        }
        let ascii: String = chunk
            .iter()
            .map(|&b| if (0x20..0x7F).contains(&b) { b as char } else { '.' })
            .collect();
        out.push(format!("{i:08x}  {hex:<50}|{ascii}|"));
    }
    CmdResult::ok(out)
}

/// `strings` — printable ASCII runs of four or more characters.
pub fn strings(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some(name) = args.split_whitespace().next() else {
        return CmdResult::err("strings: usage: strings <file>");
    };
    let path = ctx.resolve(name);
    let data = match ctx.fs.read_file(&path) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("strings: {path}: {e}")),
    };
    let mut out = Vec::new();
    let mut run = String::new();
    for &b in &data {
        if (0x20..0x7E).contains(&b) {
            run.push(b as char);
        } else if run.len() >= 4 {
            out.push(core::mem::replace(&mut run, String::new()));
        } else {
            run.clear();
        }
    }
    if run.len() >= 4 {
        out.push(run);
    }
    CmdResult::ok(out)
}

/// `tac` — print a file's lines in reverse order.
pub fn tac(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some(name) = args.split_whitespace().next() else {
        return CmdResult::err("tac: usage: tac <file>");
    };
    let path = ctx.resolve(name);
    let data = match ctx.fs.read_file(&path) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("tac: {path}: {e}")),
    };
    let text = String::from_utf8_lossy(&data);
    let mut lines: Vec<&str> = text.lines().collect();
    lines.reverse();
    CmdResult::ok(lines.into_iter().map(|s| s.to_string()).collect())
}

/// `nl` — number each line.
pub fn nl(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some(name) = args.split_whitespace().next() else {
        return CmdResult::err("nl: usage: nl <file>");
    };
    let path = ctx.resolve(name);
    let data = match ctx.fs.read_file(&path) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("nl: {path}: {e}")),
    };
    let text = String::from_utf8_lossy(&data);
    CmdResult::ok(
        text.lines()
            .enumerate()
            .map(|(i, l)| format!("{:>6}\t{}", i + 1, l))
            .collect(),
    )
}

/// `rmdir` — remove an empty directory, refusing a non-empty one.
pub fn rmdir(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some(name) = args.split_whitespace().next() else {
        return CmdResult::err("rmdir: usage: rmdir <dir>");
    };
    let path = ctx.resolve(name);
    match ctx.fs.read_dir(&path) {
        Ok(entries) if entries.is_empty() => match ctx.fs.remove(&path) {
            Ok(()) => CmdResult::silent(),
            Err(e) => CmdResult::err(format!("rmdir: {path}: {e}")),
        },
        Ok(_) => CmdResult::err(format!("rmdir: {path}: directory not empty")),
        Err(e) => CmdResult::err(format!("rmdir: {path}: {e}")),
    }
}

/// `ln` — link one file to another. This filesystem has no hard links, so it is
/// an honest copy, documented here rather than pretended to be something else.
pub fn ln(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let Some((src, dst)) = two(args) else {
        return CmdResult::err("ln: usage: ln <source> <link>");
    };
    let (src, dst) = (ctx.resolve(src), ctx.resolve(dst));
    let data = match ctx.fs.read_file(&src) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("ln: {src}: {e}")),
    };
    match ctx.fs.write_file(&dst, &data) {
        Ok(()) => CmdResult::silent(),
        Err(e) => CmdResult::err(format!("ln: {dst}: {e}")),
    }
}

/// `fold` — wrap text at a width. `-w N` sets the width (default 80).
pub fn fold(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let _ = ctx;
    let mut parts = args.split_whitespace();
    let mut width = 80usize;
    let mut text = String::new();
    while let Some(p) = parts.next() {
        if p == "-w" {
            match parts.next().and_then(|v| v.parse::<usize>().ok()) {
                Some(w) => width = w.max(1),
                None => return CmdResult::err("fold: -w needs a width"),
            }
        } else {
            if !text.is_empty() {
                text.push(' ');
            }
            text.push_str(p);
        }
    }
    if text.is_empty() {
        return CmdResult::err("fold: usage: fold [-w N] <text>");
    }
    let mut out = Vec::new();
    let mut start = 0;
    while start < text.len() {
        let mut end = (start + width).min(text.len());
        while end < text.len() && !text.is_char_boundary(end) {
            end -= 1;
        }
        out.push(text[start..end].to_string());
        start = end;
    }
    CmdResult::ok(out)
}

/// `yes` — repeat a string. Capped at 24 lines so an accidental run cannot flood
/// the scrollback.
pub fn yes(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let text = if args.trim().is_empty() { "y" } else { args.trim() };
    const CAP: usize = 24;
    let mut out = Vec::new();
    for _ in 0..CAP {
        out.push(text.to_string());
    }
    CmdResult::ok(out)
}

/// `factor` — the prime factorization of each argument.
pub fn factor(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let nums: Vec<i64> = args
        .split_whitespace()
        .filter_map(|p| p.parse::<i64>().ok())
        .collect();
    if nums.is_empty() {
        return CmdResult::err("factor: usage: factor <number>...");
    }
    let mut out = Vec::new();
    for n in nums {
        let mut factors: Vec<i64> = Vec::new();
        let mut m = n;
        let mut d = 2i64;
        while d * d <= m {
            while m % d == 0 {
                factors.push(d);
                m /= d;
            }
            d += if d == 2 { 1 } else { 2 };
        }
        if m > 1 {
            factors.push(m);
        }
        let fs: Vec<String> = factors.iter().map(|f| f.to_string()).collect();
        out.push(format!("{n}: {}", fs.join(" ")));
    }
    CmdResult::ok(out)
}

/// `cal` — a month calendar. `cal [month [year]]`, defaulting to 1 1970.
pub fn cal(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let mut it = args.split_whitespace();
    let month: u32 = it.next().and_then(|m| m.parse().ok()).unwrap_or(1);
    let year: i64 = it.next().and_then(|y| y.parse().ok()).unwrap_or(1970);
    if !(1..=12).contains(&month) {
        return CmdResult::err("cal: month must be 1..12");
    }
    CmdResult::ok(calendar(month, year))
}

fn calendar(month: u32, year: i64) -> Vec<String> {
    const MONTHS: [&str; 12] = [
        "January", "February", "March", "April", "May", "June", "July", "August", "September",
        "October", "November", "December",
    ];
    let days = days_in_month(month, year);
    let dow = first_weekday(month, year);
    let mut out = Vec::new();
    out.push(format!("    {} {year}", MONTHS[(month - 1) as usize]));
    out.push("Su Mo Tu We Th Fr Sa".to_string());
    let mut line = String::new();
    for _ in 0..dow {
        line.push_str("   ");
    }
    for day in 1..=days {
        line.push_str(&format!("{day:>2} "));
        if (dow + day) % 7 == 0 {
            out.push(line.trim_end().to_string());
            line.clear();
        }
    }
    if !line.is_empty() {
        out.push(line.trim_end().to_string());
    }
    out
}

fn days_in_month(month: u32, year: i64) -> u32 {
    match month {
        2 => {
            if is_leap(year) {
                29
            } else {
                28
            }
        }
        4 | 6 | 9 | 11 => 30,
        _ => 31,
    }
}

fn is_leap(y: i64) -> bool {
    (y % 4 == 0 && y % 100 != 0) || y % 400 == 0
}

/// Day of week of the 1st, 0=Sunday..6=Saturday (Zeller's congruence).
fn first_weekday(month: u32, year: i64) -> u32 {
    let m = if month >= 3 { month } else { month + 12 } as i64;
    let y = if month >= 3 { year } else { year - 1 };
    let k = y % 100;
    let j = y / 100;
    let h = (1 + 13 * (m + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    // Zeller: 0=Saturday,1=Sunday..6=Friday → 0=Sunday..6=Saturday.
    ((h + 6) % 7) as u32
}

/// `paste` — join corresponding lines of two files with a tab.
pub fn paste(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let mut files = args.split_whitespace();
    let (Some(a), Some(b)) = (files.next(), files.next()) else {
        return CmdResult::err("paste: usage: paste <file> <file>");
    };
    let (pa, pb) = (ctx.resolve(a), ctx.resolve(b));
    let da = match ctx.fs.read_file(&pa) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("paste: {a}: {e}")),
    };
    let db = match ctx.fs.read_file(&pb) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("paste: {b}: {e}")),
    };
    let ta = String::from_utf8_lossy(&da);
    let tb = String::from_utf8_lossy(&db);
    let la: Vec<&str> = ta.lines().collect();
    let lb: Vec<&str> = tb.lines().collect();
    let n = la.len().max(lb.len());
    let mut out = Vec::new();
    for i in 0..n {
        let x = la.get(i).copied().unwrap_or("");
        let y = lb.get(i).copied().unwrap_or("");
        out.push(format!("{x}\t{y}"));
    }
    CmdResult::ok(out)
}

/// `comm` — compare two sorted files: lines only in the first, only in the
/// second, and common, tab-indented the way comm(1) does.
pub fn comm(ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let mut files = args.split_whitespace();
    let (Some(a), Some(b)) = (files.next(), files.next()) else {
        return CmdResult::err("comm: usage: comm <file> <file>");
    };
    let (pa, pb) = (ctx.resolve(a), ctx.resolve(b));
    let da = match ctx.fs.read_file(&pa) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("comm: {a}: {e}")),
    };
    let db = match ctx.fs.read_file(&pb) {
        Ok(d) => d,
        Err(e) => return CmdResult::err(format!("comm: {b}: {e}")),
    };
    let la: Vec<String> = String::from_utf8_lossy(&da).lines().map(|s| s.to_string()).collect();
    let lb: Vec<String> = String::from_utf8_lossy(&db).lines().map(|s| s.to_string()).collect();
    let mut out = Vec::new();
    let (mut i, mut j) = (0usize, 0usize);
    while i < la.len() && j < lb.len() {
        if la[i] < lb[j] {
            out.push(la[i].clone());
            i += 1;
        } else if la[i] > lb[j] {
            out.push(format!("\t{}", lb[j]));
            j += 1;
        } else {
            out.push(format!("\t\t{}", la[i]));
            i += 1;
            j += 1;
        }
    }
    while i < la.len() {
        out.push(la[i].clone());
        i += 1;
    }
    while j < lb.len() {
        out.push(format!("\t{}", lb[j]));
        j += 1;
    }
    CmdResult::ok(out)
}

/// `tr` — translate characters: each character of the text present in SET1 is
/// replaced by the character at the same position in SET2 (last of SET2 repeats).
pub fn tr(_ctx: &mut Ctx<'_>, args: &str) -> CmdResult {
    let mut it = args.split_whitespace();
    let (Some(set1), Some(set2), Some(first)) = (it.next(), it.next(), it.next()) else {
        return CmdResult::err("tr: usage: tr <set1> <set2> <text>");
    };
    let rest: Vec<&str> = core::iter::once(first).chain(it).collect();
    let full = rest.join(" ");
    let s1: Vec<char> = set1.chars().collect();
    let s2: Vec<char> = set2.chars().collect();
    if s1.is_empty() {
        return CmdResult::err("tr: set1 must not be empty");
    }
    let mut out = String::new();
    for c in full.chars() {
        match s1.iter().position(|&x| x == c) {
            Some(pos) => {
                let repl = if pos < s2.len() { s2[pos] } else { *s2.last().unwrap() };
                out.push(repl);
            }
            None => out.push(c),
        }
    }
    CmdResult::ok(vec![out])
}




