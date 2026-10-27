#!/usr/bin/env bash
# Build the TrangorgeOS freestanding Rust app into a static non-PIE ELF.
#
#   ./build.sh              # debug
#   ./build.sh --release    # optimised
#
# Requires a nightly toolchain with the `rust-src` component (see
# rust-toolchain.toml at the repo root). Uses `-Zbuild-std=core` because there
# is no prebuilt `core` for a custom JSON target.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$HERE"

TARGET_JSON="$HERE/x86_64-trangorgeos.json"
LINKER_LD="$HERE/linker.ld"

PROFILE_ARGS=()
[[ "${1:-}" == "--release" ]] && PROFILE_ARGS=(--release)

# Link the bare ELF with our script; rustc supplies `_start` (see src/main.rs).
export RUSTFLAGS="-C link-arg=-T$LINKER_LD -C link-arg=-nostdlib -C link-arg=-static -C relocation-model=static"

cargo build \
    --manifest-path "$HERE/hello/Cargo.toml" \
    --target "$TARGET_JSON" \
    -Z json-target-spec \
    -Z build-std=core \
    -Z build-std-features=compiler-builtins-mem \
    "${PROFILE_ARGS[@]}"

PROFILE=debug
[[ "${PROFILE_ARGS[*]:-}" == *release* ]] && PROFILE=release
echo
echo "built: $HERE/hello/target/x86_64-trangorgeos/$PROFILE/hello"
