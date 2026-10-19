#!/usr/bin/env bash
# Build the TrangorgeOS Zig apps into freestanding non-PIE ELF executables.
#
#   ./build.sh              # build hello and calc
#   ./build.sh --release    # optimised
#
# `zig build-exe` is used directly (rather than `zig build`) because the CLI is
# more stable across Zig versions than the build.zig API. The resulting ELF can
# be loaded by the kernel's ring-3 loader: copy it into the image as
# /bin/<name>.elf and spawn it with TGS_SPAWN (0x1001) or from /bin/init.elf.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$HERE"

TARGET="x86_64-freestanding"
MODE="Debug"
[[ "${1:-}" == "--release" ]] && MODE="ReleaseSafe"

mkdir -p zig-out

for app in hello calc; do
    echo "building $app ..."
    zig build-exe "$app.zig" \
        -target "$TARGET" \
        -O "$MODE" \
        -T linker.ld \
        -femit-bin="zig-out/$app"
done

echo
echo "built:"
for app in hello calc; do
    echo "  zig-out/$app"
    if command -v file >/dev/null 2>&1; then
        file "zig-out/$app"
    fi
done
