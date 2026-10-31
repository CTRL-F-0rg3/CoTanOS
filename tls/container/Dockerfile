FROM archlinux:latest

# Instalacja narzędzi deweloperskich, kompilatorów dla RISC-V oraz QEMU
# (precyzyjne pakiety zamiast ogromnej grupy `qemu` — szybszy i mniejszy obraz).
RUN pacman -Syu --noconfirm --needed \
    base-devel \
    rustup \
    riscv64-elf-gcc \
    riscv64-elf-binutils \
    qemu-system-riscv \
    qemu-ui-gtk \
    git \
    make

# Konfiguracja domyślnego targetu Rust wewnątrz kontenera
RUN rustup default stable && rustup target add riscv64gc-unknown-none-elf

WORKDIR /workspace