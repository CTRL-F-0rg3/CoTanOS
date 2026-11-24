#!/bin/bash
# Bezpieczne zamykanie wszystkich instancji QEMU CoTanOS

echo "Zatrzymywanie instancji CoTanOS..."

# Wysłanie sygnału ACPI do delikatnego zamknięcia (jeśli system go obsługuje)
pkill -SIGUSR1 -f "qemu-system-x86_64.*CoTanOS"
pkill -SIGUSR1 -f "qemu-system-riscv64.*CoTanOS"

# Odczekanie 3 sekund na graceful shutdown
sleep 3

# Wymuszone zakończenie procesów, które nadal działają
pkill -9 -f "qemu-system-x86_64.*CoTanOS"
pkill -9 -f "qemu-system-riscv64.*CoTanOS"

# Czyszczenie tymczasowych plików NVRAM
rm -f build/OVMF_VARS_COPY_UEFI.fd

echo "Wszystkie instancje zostały zatrzymane."
