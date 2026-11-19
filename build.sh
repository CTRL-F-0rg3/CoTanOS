#!/usr/bin/env bash
# CoTanOS/build.sh – freestanding RV64 kernel, entered from OpenSBI in S-mode
set -euo pipefail

CROSS="${CROSS:-riscv64-elf-}"
CC="${CROSS}gcc"
LD="${CROSS}ld"
OBJDUMP="${CROSS}objdump"

KERNEL_DIR="ZFC-kernel-init"
BUILD_DIR="${KERNEL_DIR}/build"
TARGET="${BUILD_DIR}/CoTanOS.elf"
LDSCRIPT="${KERNEL_DIR}/src/link.ld"

# zicsr    – CSR instructions (binutils >= 2.38)
# zifencei – fence.i
# medany   – kernel lives above 2 GiB (0x80xxxxxx), medlow cannot reach it
# no-relax – keep boot code layout predictable
CFLAGS="-march=rv64imac_zicsr_zifencei -mabi=lp64 -mcmodel=medany -mno-relax
        -ffreestanding -fno-builtin -fno-common -nostdlib
        -O2 -g -Wall -Wextra -I${KERNEL_DIR}/include"

mkdir -p "$BUILD_DIR"
rm -f "$BUILD_DIR"/*.o

OBJS=()
echo "[*] ASM..."
for f in "$KERNEL_DIR"/src/*.s; do
    case "$(basename "$f")" in
        boot.s) echo "    skip boot.s (M-mode stub, OpenSBI does this)"; continue ;;
    esac
    o="$BUILD_DIR/$(basename "$f" .s)_asm.o"
    $CC $CFLAGS -c "$f" -o "$o"
    OBJS+=("$o")
done

echo "[*] C..."
for f in "$KERNEL_DIR"/src/*.c; do
    o="$BUILD_DIR/$(basename "$f" .c).o"
    $CC $CFLAGS -c "$f" -o "$o"
    OBJS+=("$o")
done


echo "[*] ASM (scheduler donor)..."
for f in "$KERNEL_DIR"/src/scheduler/*.S; do
    [ -f "$f" ] || continue
    o="$BUILD_DIR/sched_$(basename "$f" .S)_asm.o"
    $CC $CFLAGS -I"$KERNEL_DIR"/src/scheduler -I"$KERNEL_DIR"/include -c "$f" -o "$o"
    OBJS+=("$o")
done


echo "[*] C (scheduler donor)..."
for f in "$KERNEL_DIR"/src/scheduler/*.c; do
    [ -f "$f" ] || continue
    o="$BUILD_DIR/sched_$(basename "$f" .c).o"
    $CC $CFLAGS -I"$KERNEL_DIR"/src/scheduler -I"$KERNEL_DIR"/include -c "$f" -o "$o"
    OBJS+=("$o")
done

echo "[*] LD..."
$CC $CFLAGS -nostdlib -Wl,-T,"$LDSCRIPT" -Wl,--build-id=none \
    "${OBJS[@]}" -o "$TARGET"

$OBJDUMP -d "$TARGET" > "$BUILD_DIR/CoTanOS.asm"
echo "[+] $TARGET"
$CROSS-size "$TARGET" 2>/dev/null || true
