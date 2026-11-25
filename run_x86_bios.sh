#!/bin/bash
# CoTanOS x86_64 BIOS Boot (Instancja 1)
set -euo pipefail

DISK_IMG="disk_x86_bios.img"
MEM="1G"
CPUS="1"

if [ ! -f "$DISK_IMG" ]; then
  echo "[-] Brak $DISK_IMG — uruchom najpierw ./build.sh"; exit 1
fi

qemu-system-x86_64 \
  -name "CoTanOS-x86-BIOS" \
  -machine q35,accel=kvm \
  -m "$MEM" -smp "$CPUS" -cpu host \
  -drive file="$DISK_IMG",format=raw,if=none,id=boot0 \
  -device ide-hd,bus=ide.0,drive=boot0 \
  -device virtio-vga-gl \
  -device virtio-keyboard-pci \
  -device virtio-tablet-pci \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device virtio-net-pci,netdev=net0 \
  -display gtk,gl=on \
  -no-reboot -no-shutdown &
