#!/bin/bash
# Master script do równoległego uruchamiania wszystkich instancji CoTanOS

echo "Rozpoczynanie uruchamiania wszystkich instancji CoTanOS..."

# Upewnij się, że skrypty są wykonywalne
chmod +x run_x86_bios.sh run_x86_uefi.sh run_riscv.sh

# Uruchomienie w tle
echo "Uruchamianie x86_64 BIOS..."
./run_x86_bios.sh

echo "Uruchamianie x86_64 UEFI..."
./run_x86_uefi.sh

echo "Uruchamianie RISC-V 64..."
./run_riscv.sh

echo "Wszystkie instancje zostały uruchomione w tle."
echo "Możesz zamknąć ten terminal. Okna QEMU GTK pozostaną otwarte."
echo ""
echo "Przekierowania portów SSH:"
echo "  x86_64 BIOS : ssh -p 2222 user@localhost"
echo "  x86_64 UEFI : ssh -p 2223 user@localhost"
echo "  RISC-V      : ssh -p 2224 user@localhost"
