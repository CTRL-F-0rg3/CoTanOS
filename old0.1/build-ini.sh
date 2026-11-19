#!/usr/bin/env bash
set -e

BUILD_DIR="ini/target"
mkdir -p "$BUILD_DIR"

TARGET_PREFIX="riscv64-elf-"

echo "==> [Kontener] Kompilacja boot.s i main.c..."
${TARGET_PREFIX}gcc -march=rv64gc -mabi=lp64d -c ini/boot.s -o "$BUILD_DIR/boot.o"
${TARGET_PREFIX}gcc -march=rv64gc -mabi=lp64d -ffreestanding -nostdlib -nodefaultlibs -g -c ini/main.c -o "$BUILD_DIR/main.o"

echo "==> [Kontener] Linkowanie do bootloader.elf..."
${TARGET_PREFIX}ld -T ini/link.ld "$BUILD_DIR/boot.o" "$BUILD_DIR/main.o" -o "$BUILD_DIR/bootloader.elf"

echo "==> [Kontener] Kompilacja zakończona sukcesem!"