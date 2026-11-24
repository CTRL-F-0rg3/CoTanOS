#!/bin/bash
# CoTanOS RISC-V 64 OpenSBI Boot (Instancja 3)

KERNEL_IMG="build/cotanos-riscv64.bin"
OPENSBI_IMG="build/opensbi-riscv64.bin"
DISK_IMG="disk_riscv.img"
MEM="1G"
CPUS="1"

if [ ! -f "$DISK_IMG" ]; then
  qemu-img create -f raw "$DISK_IMG" 5G
fi

BIOS_ARG=""
if [ -f "$OPENSBI_IMG" ]; then
  BIOS_ARG="-bios $OPENSBI_IMG"
fi

qemu-system-riscv64 \
  -name "CoTanOS-RISCV" \
  -machine virt,accel=tcg \
  -m "$MEM" \
  -smp "$CPUS" \
  -cpu rv64 \
  $BIOS_ARG \
  -kernel "$KERNEL_IMG" \
  -drive file="$DISK_IMG",format=raw,if=virtio \
  -device virtio-gpu-device \
  -device virtio-keyboard-device \
  -device virtio-tablet-device \
  -netdev user,id=net0,hostfwd=tcp::2224-:22 \
  -device virtio-net-device,netdev=net0 \
  -display gtk,gl=on \
  -no-reboot \
  -no-shutdown &
