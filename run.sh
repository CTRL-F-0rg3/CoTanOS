#!/usr/bin/env bash
# CoTanOS/run.sh – launch kernel in QEMU virt via OpenSBI (S-mode entry)
set -euo pipefail

# NOTE: use .bin, NOT .elf – AUR opensbi ELFs have VMA 0 and QEMU
# falls back to loading them as raw blobs (executes the ELF header).
OPENSBI="${OPENSBI:-/usr/share/opensbi/lp64/generic/firmware/fw_dynamic.bin}"
KERNEL="${KERNEL:-ZFC-kernel-init/build/CoTanOS.elf}"
MEM="${MEM:-256M}"
SMP="${SMP:-2}"
GDB="${GDB:-0}"        # GDB=1  -> halt at reset, gdbstub on :1234
TRACE="${TRACE:-0}"    # TRACE=1 -> log traps/MMU to build/qemu.log
TIMEOUT="${TIMEOUT:-0}" # TIMEOUT=10 -> kill after 10s (CI / smoke test)

for f in "$OPENSBI" "$KERNEL"; do
    [ -f "$f" ] || { echo "[-] missing: $f"; exit 1; }
done

mkdir -p build
echo "[*] OpenSBI : $OPENSBI"
echo "[*] kernel  : $KERNEL"
echo "[*] entry   : $(riscv64-elf-readelf -h "$KERNEL" | awk '/Entry point/{print $4}')"
echo "[*] exit    : Ctrl-A then X"

ARGS=(
    -M virt
    -m "$MEM"
    -smp "$SMP"
    -bios "$OPENSBI"
    -kernel "$KERNEL"
    -nographic
    -no-reboot
)

[ "$GDB" = "1" ]   && ARGS+=(-S -gdb tcp::1234)
[ "$TRACE" = "1" ] && ARGS+=(-d int,mmu,guest_errors -D build/qemu.log)

if [ "$TIMEOUT" != "0" ]; then
    exec timeout "${TIMEOUT}s" qemu-system-riscv64 "${ARGS[@]}"
else
    exec qemu-system-riscv64 "${ARGS[@]}"
fi
