#!/bin/bash
# CoTanOS x86_64 BIOS Boot (Instancja 1)

KERNEL_IMG="build/cotanos-x86_64.bin"
DISK_IMG="disk_x86_bios.img"
MEM="1G" # Zmniejszono do 1G, aby uruchomić 3 maszyny naraz bez przeciążania RAM
CPUS="1"

if [ ! -f "$DISK_IMG" ]; then
  qemu-img create -f raw "$DISK_IMG" 5G
fi

qemu-system-x86_64 \
  -name "CoTanOS-x86-BIOS" \
  -machine q35,accel=kvm \
  -m "$MEM" \
  -smp "$CPUS" \
  -cpu host \
  -kernel "$KERNEL_IMG" \
  -drive file="$DISK_IMG",format=raw,if=virtio \
  -device virtio-vga-gl \
  -device virtio-keyboard-pci \
  -device virtio-tablet-pci \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device virtio-net-pci,netdev=net0 \
  -display gtk,gl=on \
  -no-reboot \
  -no-shutdown &
