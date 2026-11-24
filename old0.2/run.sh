#!/usr/bin/env bash
set -euo pipefail
OPENSBI="${OPENSBI:-/usr/share/opensbi/lp64/generic/firmware/fw_dynamic.bin}"
KERNEL="${KERNEL:-ZFC-kernel-init/build/CoTanOS.elf}"
MEM="${MEM:-512M}"
SMP="${SMP:-2}"
GDB="${GDB:-0}"
TRACE="${TRACE:-0}"
TIMEOUT="${TIMEOUT:-0}"

for f in "$OPENSBI" "$KERNEL"; do [ -f "$f" ] || { echo "[-] missing: $f"; exit 1; }; done
mkdir -p build
echo "[*] OpenSBI : $OPENSBI"
echo "[*] kernel  : $KERNEL"
echo "[*] entry   : $(riscv64-elf-readelf -h "$KERNEL" | awk '/Entry point/{print $4}')"
echo "[*] exit    : Ctrl-A then X"

ARGS=(
    -M virt -m "$MEM" -smp "$SMP" -bios "$OPENSBI" -kernel "$KERNEL"
    -display gtk
    -serial mon:stdio
    -no-reboot
    -device virtio-gpu-pci
    -drive file=disk.img,if=none,format=raw,id=hd0
    -device virtio-blk-device,drive=hd0
    -netdev user,id=n0
    -device virtio-net-device,netdev=n0
)
[ "$GDB" = "1" ]   && ARGS+=(-S -gdb tcp::1234)
[ "$TRACE" = "1" ] && ARGS+=(-d int,mmu,guest_errors -D build/qemu.log)

if [ "$TIMEOUT" != "0" ]; then
    exec timeout "${TIMEOUT}s" qemu-system-riscv64 "${ARGS[@]}"
else
    exec qemu-system-riscv64 "${ARGS[@]}"
fi
