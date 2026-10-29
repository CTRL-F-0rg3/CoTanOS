#!/usr/bin/env bash
set -e

echo "==> Uruchamianie testowego środowiska CoTanOS w QEMU..."
qemu-system-riscv64 \
    -machine virt \
    -cpu rv64 \
    -smp 1 \
    -m 128M \
    -nographic \
    -kernel test-boot/kernel.elf