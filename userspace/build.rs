// Compiles the C++ userspace applications in `apps/cpp` into a static library
// linked into this crate, mirroring the kernel's own C bridge (see
// `kernel_Workspace/kernel/build.rs`).
//
// On the kernel target (`x86_64-unknown-none`, `CARGO_CFG_TARGET_OS == "none"`)
// the code is compiled freestanding with clang for `x86_64-unknown-none-elf`;
// on the host (`cargo check`/`cargo test`) it is compiled for the host instead,
// so the userspace crate still type-checks without the cross toolchain.

use std::env;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

fn collect_cpp(dir: &Path, out: &mut Vec<PathBuf>) {
    if let Ok(entries) = fs::read_dir(dir) {
        for entry in entries.flatten() {
            let path = entry.path();
            if path.is_dir() {
                collect_cpp(&path, out);
            } else if path.extension().and_then(|e| e.to_str()) == Some("cpp") {
                out.push(path);
            }
        }
    }
}

fn main() {
    let manifest = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());
    let cpp_dir = manifest.join("apps/cpp");

    println!("cargo:rerun-if-changed=apps/cpp");

    let mut cpp_files = Vec::new();
    collect_cpp(&cpp_dir, &mut cpp_files);
    cpp_files.sort();

    if cpp_files.is_empty() {
        return;
    }

    let freestanding = env::var("CARGO_CFG_TARGET_OS").map(|o| o == "none").unwrap_or(false);

    let mut objs = Vec::new();
    for file in &cpp_files {
        let obj = out_dir.join(format!(
            "{}.o",
            file.file_stem().and_then(|s| s.to_str()).unwrap_or("app")
        ));

        let mut cmd = Command::new("clang");
        if freestanding {
            cmd.arg("--target=x86_64-unknown-none-elf");
            cmd.args(["-ffreestanding", "-fno-builtin", "-fno-stack-protector"]);
            cmd.args([
                "-mno-red-zone",
                "-mcmodel=kernel",
                "-mno-sse",
                "-mno-sse2",
                "-mno-mmx",
                "-mno-80387",
                "-O2",
            ]);
        } else {
            cmd.args(["-fPIC", "-O1"]);
        }
        cmd.args([
            "-std=c++17",
            "-fno-exceptions",
            "-fno-rtti",
            "-fno-threadsafe-statics",
        ]);
        cmd.arg("-I").arg(&cpp_dir);
        cmd.arg("-c").arg(file).arg("-o").arg(&obj);

        let status = cmd
            .status()
            .unwrap_or_else(|e| panic!("failed to run clang for {}: {}", file.display(), e));
        if !status.success() {
            panic!("C++ compilation failed for {}", file.display());
        }
        objs.push(obj);
    }

    let lib = out_dir.join("libcppapps.a");
    let _ = fs::remove_file(&lib);
    let mut ar = Command::new("ar");
    ar.arg("rcs").arg(&lib);
    for obj in &objs {
        ar.arg(obj);
    }
    let status = ar.status().expect("failed to run ar");
    if !status.success() {
        panic!("ar failed");
    }

    println!("cargo:rustc-link-search=native={}", out_dir.display());
    println!("cargo:rustc-link-lib=static=cppapps");
}
