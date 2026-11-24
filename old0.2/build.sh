#!/bin/bash
set -e

KERNEL_DIR="ZFC-kernel-init"
BUILD_DIR="$KERNEL_DIR/build"
USER_DIR="userspace"
RUST_DIR="ZFC-kernel-core"
BASE_DIR="ZFC-kernel-base"

echo "[*] Cleaning build directory..."
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
mkdir -p $USER_DIR/build

echo "[*] Building Rust Core (with in-kernel drivers)..."
cd $RUST_DIR
cargo build --release -Z build-std=core,compiler_builtins,alloc
cd ..

echo "[*] Building Base Display Library (RISC-V)..."
cd $BASE_DIR
make clean
make all
cd ..

echo "[*] Building kernel (C/ASM)..."
cd $KERNEL_DIR

for f in src/*.c; do
    echo "    CC $f"
    riscv64-elf-gcc -c $f -o build/$(basename $f .c).o \
        -march=rv64imafd -mabi=lp64d -mcmodel=medany -fno-builtin -nostdlib \
        -I include -I src/scheduler -I ../$BASE_DIR/include -Wall -O0
done

find src/scheduler -name "*.c" | while read f; do
    echo "    CC $f"
    riscv64-elf-gcc -c "$f" -o "build/$(basename "$f" .c).o" \
        -march=rv64imafd -mabi=lp64d -mcmodel=medany -fno-builtin -nostdlib \
        -I include -I src/scheduler -Wall -O0
done

find src -name "*.s" | while read f; do
    base=$(basename "$f")
    if [ "$base" != "boot.s" ]; then
        echo "    AS $f"
        riscv64-elf-as "$f" -o "build/$(basename "$f" .s).o"
    fi
done

find src -name "*.S" | while read f; do
    echo "    AS $f"
    riscv64-elf-gcc -c "$f" -o "build/$(basename "$f" .S).o" \
        -march=rv64imafd -mabi=lp64d -mcmodel=medany -fno-builtin -nostdlib \
        -I include -I src/scheduler
done

cd ..

echo "[*] Building user programs..."
for f in $USER_DIR/*.S; do
    if [ -f "$f" ]; then
        name=$(basename $f .S)
        riscv64-elf-as $f -o $USER_DIR/build/${name}.o
        riscv64-elf-ld $USER_DIR/build/${name}.o -o $USER_DIR/build/${name}.bin
    fi
done

if [ -d "$USER_DIR/hello" ]; then
    echo "[*] Building userspace C programs..."
    make -C $USER_DIR || true
fi

echo "[*] Linking kernel (C + Rust + Base)..."
cd $KERNEL_DIR
riscv64-elf-gcc -march=rv64imafd -mabi=lp64d -nostdlib -T link.ld \
    build/*.o \
    ../$RUST_DIR/target/riscv64imafdc-unknown-none-elf/release/libcotan_core.a \
    -L../$BASE_DIR -lcotanbase \
    -o build/CoTanOS.elf

cd ..
echo "[+] Build complete: $BUILD_DIR/CoTanOS.elf"
