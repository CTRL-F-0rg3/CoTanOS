#!/bin/bash
# CoTanOS x86_64 UEFI Boot (Instancja 2)
set -euo pipefail

ESP_DIR="build/esp"
MEM="1G"
CPUS="1"

for code in \
    /usr/share/edk2/x64/OVMF_CODE.4m.fd \
    /usr/share/edk2/ovmf/OVMF_CODE.fd \
    /usr/share/OVMF/OVMF_CODE.fd \
    /usr/share/qemu/OVMF_CODE.fd ; do
  if [ -f "$code" ]; then OVMF_CODE="$code"; break; fi
done
if [ -z "${OVMF_CODE:-}" ]; then
  echo "[-] Nie znaleziono OVMF_CODE.fd — zainstaluj edk2-ovmf"; exit 1
fi

case "$OVMF_CODE" in
  *edk2*) OVMF_VARS="${OVMF_CODE/CODE/VARS}" ;;
  *)      OVMF_VARS="$(dirname "$OVMF_CODE")/OVMF_VARS.fd" ;;
esac
[ -f "$OVMF_VARS" ] || { echo "[-] Brak $OVMF_VARS"; exit 1; }

[ -f "$ESP_DIR/EFI/BOOT/BOOTX64.EFI" ] || {
  echo "[-] Brak $ESP_DIR/EFI/BOOT/BOOTX64.EFI — uruchom ./build.sh"; exit 1
}

mkdir -p build
cp "$OVMF_VARS" build/OVMF_VARS_COPY_UEFI.fd

qemu-system-x86_64 \
  -name "CoTanOS-x86-UEFI" \
  -machine q35,accel=kvm \
  -m "$MEM" -smp "$CPUS" -cpu host \
  -drive if=pflash,format=raw,unit=0,file="$OVMF_CODE",readonly=on \
  -drive if=pflash,format=raw,unit=1,file="build/OVMF_VARS_COPY_UEFI.fd" \
  -drive if=none,id=esp,format=raw,file=fat:rw:"$ESP_DIR" \
  -device ide-hd,bus=ide.0,drive=esp \
  -device virtio-vga-gl \
  -device virtio-keyboard-pci \
  -device virtio-tablet-pci \
  -netdev user,id=net0,hostfwd=tcp::2223-:22 \
  -device virtio-net-pci,netdev=net0 \
  -display gtk,gl=on \
  -no-reboot -no-shutdown &
