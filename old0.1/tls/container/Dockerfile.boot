FROM archlinux:latest

# Boot-chain test image for RISC-V: C toolchain, QEMU with GTK UI,
# and the Rust toolchain (nightly) for the 256xtr kernel (new/ workspace).
RUN pacman -Syu --noconfirm --needed \
    base-devel \
    riscv64-elf-gcc \
    riscv64-elf-binutils \
    qemu-system-riscv \
    qemu-ui-gtk \
    rustup \
    make

# Nightly is required by the new/ workspace (uses unstable features).
# The riscv64gc-unknown-none-elf target builds the arc-rv6 kernel.
RUN rustup default stable \
    && rustup toolchain install nightly --profile minimal \
    && rustup target add --toolchain nightly riscv64gc-unknown-none-elf

WORKDIR /workspace
