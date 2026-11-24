#!/bin/bash
set -e

echo "[*] Kopiowanie nagłówków EGL/GLES2..."
cp -r mesa-upstream/include/EGL deps-riscv/include/ 2>/dev/null || true
cp -r mesa-upstream/include/GLES2 deps-riscv/include/ 2>/dev/null || true
cp -r mesa-upstream/include/GLES deps-riscv/include/ 2>/dev/null || true
cp -r mesa-upstream/include/KHR deps-riscv/include/ 2>/dev/null || true

echo "[*] Kompilacja triangle.c dla RISC-V..."
riscv64-linux-gnu-gcc -o triangle triangle.c \
    -Ideps-riscv/include \
    -Ldeps-riscv/lib \
    -lEGL -lGLESv2 -lgallium -lgbm -ldrm_cotan -lm -lpthread \
    -Wl,-rpath,/home/ctrl/project/CoTanOS/deps-riscv/lib \
    -Wl,-rpath-link,/home/ctrl/project/CoTanOS/deps-riscv/lib

echo "[+] triangle skompilowany:"
file triangle

echo "[*] Przygotowanie disk.img..."
if [ ! -f disk.img ]; then
    echo "[-] Brak disk.img, tworzę 64MB obraz ext2..."
    qemu-img create -f raw disk.img 64M
    mkfs.ext2 disk.img
fi

MOUNT_DIR=$(mktemp -d)
echo "[*] Montowanie disk.img do $MOUNT_DIR..."
sudo mount -o loop disk.img $MOUNT_DIR

echo "[*] Kopiowanie plików do obrazu..."
sudo cp triangle $MOUNT_DIR/
sudo mkdir -p $MOUNT_DIR/lib
sudo cp deps-riscv/lib/*.so* $MOUNT_DIR/lib/ 2>/dev/null || true
sudo chmod -R 755 $MOUNT_DIR/lib

echo "[*] Odmonowywanie..."
sudo umount $MOUNT_DIR
rmdir $MOUNT_DIR

echo "[*] Zwiększanie RAM w run.sh do 512M..."
if grep -q '256M' run.sh 2>/dev/null; then
    sed -i 's/256M/512M/g' run.sh
fi

echo ""
echo "========================================="
echo "[+] GOTOWE! Uruchom CoTanOS:"
echo "    ./run.sh"
echo ""
echo "Wewnątrz CoTanOS (w userspace) wykonaj:"
echo "    export LD_LIBRARY_PATH=/lib"
echo "    ./triangle"
echo "========================================="
