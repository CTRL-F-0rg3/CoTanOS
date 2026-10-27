// Kompiluje logikę DE z C++ do biblioteki statycznej linkowanej przez Rust.
//
// Rdzeń pozostaje w Rust, a logika DE żyje w `src/logic/de_logic.cpp` — ten
// skrypt woła `cc` (skrzynkę cargo), żeby skompilować ją g++/clang++ i
// wystawić jako `static` lib. Strzeżemy się tylko jednego pliku, więc build
// jest szybki i przewidywalny.

fn main() {
    cc::Build::new()
        .cpp(true)
        .std("c++17")
        .file("src/logic/de_logic.cpp")
        .compile("de_logic");

    println!("cargo:rerun-if-changed=src/logic/de_logic.cpp");
    println!("cargo:rerun-if-changed=src/logic/de_logic.hpp");
}
