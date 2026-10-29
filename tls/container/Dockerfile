FROM archlinux:latest

RUN pacman -Syu --noconfirm --needed \
    base-devel \
    rustup \
    riscv64-elf-gcc \
    riscv64-elf-binutils \
    qemu \
    git \
    make

RUN rustup default stable && rustup target add riscv64gc-unknown-none-elf

WORKDIR /workspace