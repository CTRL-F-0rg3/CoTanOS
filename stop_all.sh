#!/bin/bash
echo "Zatrzymywanie instancji CoTanOS..."

pkill -TERM -f "qemu-system-x86_64.*CoTanOS"  || true
pkill -TERM -f "qemu-system-riscv64.*CoTanOS" || true

sleep 2

pkill -KILL -f "qemu-system-x86_64.*CoTanOS"  || true
pkill -KILL -f "qemu-system-riscv64.*CoTanOS" || true

rm -f build/OVMF_VARS_COPY_UEFI.fd
echo "Zatrzymano."
