// crates/loomcos-core/build.rs
fn main() {
    cc::Build::new()
        .cpp(true)
        .file("cpp/render_helper.cpp")
        .file("cpp/alpha_blend.cpp")
        .file("cpp/window_class.cpp")
        .file("cpp/window_runtime.cpp")
        .file("cpp/vector_scene.cpp")
        .file("cpp/color_utils.cpp")
        .file("cpp/effects_renderer.cpp")
        .file("cpp/diff_renderer.cpp")
        .file("cpp/area_restore.cpp")
        .file("cpp/animation_engine.cpp") // Smooth interpolation & easing animation engine
        .include("include")
        .opt_level(3)
        .compile("loomcos_cpp_helper");

    println!("cargo:rerun-if-changed=cpp/animation_engine.cpp");
    println!("cargo:rerun-if-changed=include/loomcos_anim.h");
}