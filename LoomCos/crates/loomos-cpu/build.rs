// crates/loomcos-cpu/build.rs
fn main() {
    cc::Build::new()
        .file("asm/fast_ops.s")
        .cpp(true)
        .file("cpp/cpu_accelerator.cpp")
        .file("cpp/noise_refresh.cpp") // Stochastic noise-driven tile scheduler
        .include("include")
        .opt_level(3)
        .compile("loomcos_cpu_backend");

    println!("cargo:rerun-if-changed=cpp/noise_refresh.cpp");
    println!("cargo:rerun-if-changed=include/loomcos_noise_refresh.h");
}