#!/bin/bash
# CoTanOS x86_64 UEFI Boot (Instancja 2)

KERNEL_IMG="build/cotanos-x86_64.efi"
DISK_IMG="disk_x86_uefi.img"
MEM="1G"
CPUS="1"

if [ -f "/usr/share/edk2/x64/OVMF_CODE.4m.fd" ]; then
  OVMF_CODE="/usr/share/edk2/x64/OVMF_CODE.4m.fd"
  OVMF_VARS="/usr/share/edk2/x64/OVMF_VARS.4m.fd"
elif [ -f "/usr/share/edk2/x64/OVMF_CODE.4m.fd" ]; then
  OVMF_CODE="/usr/share/edk2/x64/OVMF_CODE.4m.fd"
  OVMF_VARS="/usr/share/edk2/x64/OVMF_VARS.4m.fd"
else
  echo "Błąd: Nie znaleziono plików OVMF."
  exit 1
fi

if [ ! -f "$DISK_IMG" ]; then
  qemu-img create -f raw "$DISK_IMG" 5G
fi

cp "$OVMF_VARS" build/OVMF_VARS_COPY_UEFI.fd

qemu-system-x86_64 \
  -name "CoTanOS-x86-UEFI" \
  -machine q35,accel=kvm \
  -m "$MEM" \
  -smp "$CPUS" \
  -cpu host \
  -drive if=pflash,format=raw,unit=0,file="$OVMF_CODE",readonly=on \
  -drive if=pflash,format=raw,unit=1,file="build/OVMF_VARS_COPY_UEFI.fd" \
  -drive file="$DISK_IMG",format=raw,if=virtio \
  -device virtio-vga-gl \
  -device virtio-keyboard-pci \
  -device virtio-tablet-pci \
  -netdev user,id=net0,hostfwd=tcp::2223-:22 \
  -device virtio-net-pci,netdev=net0 \
  -display gtk,gl=on \
  -no-reboot \
  -no-shutdown &
