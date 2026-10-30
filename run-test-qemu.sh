#!/usr/bin/env bash
set -e

GREEN='\033[0;32m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

# Automatyczne wykrycie czy potrzebne jest sudo do Dockera
CONTAINER_CMD="docker"
if ! docker ps &> /dev/null; then
    CONTAINER_CMD="sudo docker"
fi

IMAGE_NAME="cotanos-dev"
BUILD_DIR="ini/target"
ELF_PATH="$BUILD_DIR/bootloader.elf"

# Ucisz ostrzeżenie asemblera o braku newline na końcu boot.s
if [ -f ini/boot.s ] && [ -n "$(tail -c 1 ini/boot.s)" ]; then
    echo >> ini/boot.s
fi

echo -e "${CYAN}==> [1/3] Sprawdzanie i budowanie obrazu kontenera...${NC}"
$CONTAINER_CMD build -t $IMAGE_NAME -f tls/container/Dockerfile .

echo -e "${GREEN}==> [2/3] Kompilacja CoTanOS (ini) wewnątrz kontenera...${NC}"
mkdir -p "$BUILD_DIR"

$CONTAINER_CMD run --rm -it \
    --volume "$(pwd):/workspace:z" \
    $IMAGE_NAME \
    bash -c "riscv64-elf-gcc -march=rv64gc -mabi=lp64d -mcmodel=medany -c ini/boot.s -o /workspace/$BUILD_DIR/boot.o && \
             riscv64-elf-gcc -march=rv64gc -mabi=lp64d -mcmodel=medany -ffreestanding -nostdlib -nodefaultlibs -g -c ini/main.c -o /workspace/$BUILD_DIR/main.o && \
             riscv64-elf-ld -T ini/link.ld /workspace/$BUILD_DIR/boot.o /workspace/$BUILD_DIR/main.o -o /workspace/$BUILD_DIR/bootloader.elf"

sudo chown -R $USER:$USER "$BUILD_DIR"

if [ ! -f "$ELF_PATH" ]; then
    echo -e "${RED}[!] Błąd: Plik wynikowy $ELF_PATH nie powstał!${NC}"
    exit 1
fi

echo -e "${GREEN}==> [3/3] Uruchamianie QEMU z oknem graficznym (ramfb)...${NC}"

QEMU_CMD="qemu-system-riscv64 \
    -machine virt \
    -cpu rv64 \
    -m 128M \
    -bios none \
    -kernel $ELF_PATH \
    -device ramfb \
    -display gtk,zoom-to-fit=on \
    -serial stdio"

echo -e "${CYAN}Wykonuję:${NC} $QEMU_CMD"
echo "--------------------------------------------------------"

$QEMU_CMD