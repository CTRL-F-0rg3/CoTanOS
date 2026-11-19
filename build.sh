#!/usr/bin/env bash
# CoTanOS/build.sh – freestanding RV64 kernel, entered from OpenSBI in S-mode
set -euo pipefail

CROSS="${CROSS:-riscv64-elf-}"
CC="${CROSS}gcc"
LD="${CROSS}ld"
OBJDUMP="${CROSS}objdump"
OBJCOPY="${CROSS}objcopy"

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
        -O2 -g -Wall -Wextra -I${KERNEL_DIR}/include -I${KERNEL_DIR}/src/scheduler"

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



echo "[*] ASM (uppercase .S files with preprocessor)..."
for f in "$KERNEL_DIR"/src/*.S; do
    [ -f "$f" ] || continue
    o="$BUILD_DIR/$(basename "$f" .S)_asm.o"
    $CC $CFLAGS -I"$KERNEL_DIR"/include -I"$KERNEL_DIR"/src/scheduler -c "$f" -o "$o"
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



echo "[*] Building user app..."
# 1. Compile assembly to object file
${CC} ${CFLAGS} -c apps/user_test.S -o ${BUILD_DIR}/user_test.o
# 2. Extract raw binary
${OBJCOPY} -O binary ${BUILD_DIR}/user_test.o ${BUILD_DIR}/user_test.bin
# 3. Link binary as an ELF object to embed it in the kernel
# This generates symbols: _binary_user_test_bin_start and _binary_user_test_bin_end
${LD} -r -b binary -o ${BUILD_DIR}/user_test_embed.o ${BUILD_DIR}/user_test.bin
OBJS+=("${BUILD_DIR}/user_test_embed.o")

echo "[*] LD..."
$CC $CFLAGS -nostdlib -Wl,-T,"$LDSCRIPT" -Wl,--build-id=none \
    "${OBJS[@]}" -o "$TARGET"

$OBJDUMP -d "$TARGET" > "$BUILD_DIR/CoTanOS.asm"
echo "[+] $TARGET"
$CROSS-size "$TARGET" 2>/dev/null || true
