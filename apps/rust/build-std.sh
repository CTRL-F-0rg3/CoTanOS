#!/usr/bin/env bash
# Build a TrangorgeOS userspace program against the *real* Rust standard
# library, reusing the `ustd` sysroot (target spec + linker script + tgs-rt).
#
#   ./build-std.sh            # build apps/rust/std (mandel) for the std target
#   ./build-std.sh --release  # optimised
#   ./build-std.sh --check    # type-check only
#
# This is the self-contained `apps/` counterpart of `ustd/build.sh`. The heavy
# lifting — `_start`, the allocator, the libc symbols `std` links against — is
# done by `tgs-rt`; this script only supplies the linker flags `std` needs on a
# bare target.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$HERE"

export PATH="${CARGO_HOME:-$HOME/.cargo}/bin:$PATH"

TARGET_JSON="$HERE/../../ustd/targets/x86_64-trangorge-uspace.json"
LINKER_LD="$HERE/../../ustd/linker.ld"

for f in "$TARGET_JSON" "$LINKER_LD"; do
    [[ -f "$f" ]] || { echo "build-std.sh: missing $f" >&2; exit 1; }
done

# Empty archives for the `-l` list `std` asks the linker for on a Linux-flavoured
# target. Each name is satisfied by an empty archive that defines nothing.
STUB_DIR="$HERE/target/stub-libs"
mkdir -p "$STUB_DIR"
for lib in util rt pthread m dl c gcc gcc_eh; do
    archive="$STUB_DIR/lib$lib.a"
    if [[ ! -f "$archive" ]]; then
        echo 'int tgs_stub_library_not_used;' > "$STUB_DIR/stub.c"
        (cc -c "$STUB_DIR/stub.c" -o "$STUB_DIR/stub.o" 2>/dev/null \
            || clang -c "$STUB_DIR/stub.c" -o "$STUB_DIR/stub.o" 2>/dev/null)
        ar rcs "$archive" "$STUB_DIR/stub.o"
    fi
done

# Keep tgs-libc alive regardless of archive order (same list as ustd/build.sh).
FORCE_SYMS=()
for sym in malloc calloc realloc free posix_memalign abort \
           write writev read close syscall getcwd \
           clock_gettime __errno_location __xpg_strerror_r \
           pthread_key_create pthread_key_delete \
           pthread_getspecific pthread_setspecific \
           _Unwind_Backtrace _Unwind_GetIP; do
    FORCE_SYMS+=("-C" "link-arg=--undefined=$sym")
done

export RUSTFLAGS="\
-C link-arg=-nostdlib \
-C link-arg=-static \
-C link-arg=-znoexecstack \
-C link-arg=-T$LINKER_LD \
-C link-arg=-L$STUB_DIR \
-C relocation-model=static \
-C link-arg=--gc-sections \
${FORCE_SYMS[@]}"

STD_FLAGS=(
    -Zjson-target-spec
    -Zbuild-std=std,panic_abort
    -Zbuild-std-features=compiler-builtins-mem
)

PROFILE_ARGS=()
CHECK=0
for arg in "$@"; do
    case "$arg" in
        --release) PROFILE_ARGS=(--release) ;;
        --check) CHECK=1 ;;
        *) echo "build-std.sh: unknown option $arg" >&2; exit 2 ;;
    esac
done

MANIFEST="$HERE/std/Cargo.toml"

if [[ "$CHECK" == 1 ]]; then
    exec cargo check --manifest-path "$MANIFEST" --target "$TARGET_JSON" "${STD_FLAGS[@]}" "${PROFILE_ARGS[@]}"
fi

cargo build --manifest-path "$MANIFEST" --target "$TARGET_JSON" "${STD_FLAGS[@]}" "${PROFILE_ARGS[@]}"
