#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
# SPDX-License-Identifier: GPL-3.0-only
#
# Boot-chain test for CoTanOS on RISC-V (QEMU 'virt'):
#   CoTanSBI   (M-mode, own SBI,        -bios          @ 0x8000_0000)
#     -> ini/  bootloader (S-mode, "BIOS", -device loader @ 0x8020_0000)
#       -> 256xtr kernel (Rust, arc-rv6,    -kernel      @ 0x8040_0000)
#
# Default: QEMU GRAPHICAL window (GTK) + serial console in the terminal.
#   --nographic : serial console only (CI/SSH/no X11)
#
# Tooling:
#   * builds — on the host, or inside a container (podman/docker, image
#     cotanos-boot) when riscv64-elf-gcc/cargo are missing; a stale image
#     without the required tools is detected and rebuilt automatically,
#   * QEMU — host preferred (the window works natively),
#     fallback: container with the display forwarded (X11/Wayland).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
have() { command -v "$1" >/dev/null 2>&1; }

# ---------------------------------------------------------------------------
# Mode: graphical (default) or --nographic
# ---------------------------------------------------------------------------
GRAPHIC=1
if [ "${1:-}" = "--nographic" ]; then
    GRAPHIC=0
fi

pick_ct() {
    if have podman; then echo podman
    elif have docker; then echo docker
    else return 1; fi
}

echo "==> CoTanOS boot-chain test (RISC-V, QEMU virt)..."
if [ "$GRAPHIC" = 1 ]; then
    echo "    mode: QEMU graphical window + serial console in this terminal"
else
    echo "    mode: serial console only (-nographic)"
fi

# ---------------------------------------------------------------------------
# 1. Toolchain: host or container?
# ---------------------------------------------------------------------------
CT=""
if ! have riscv64-elf-gcc || ! have cargo; then
    CT="$(pick_ct || true)"
    if [ -z "$CT" ]; then
        echo "[!] Missing riscv64-elf-gcc/cargo on the host and no docker/podman."
        echo "    Install the toolchain or a container (tls/container/)."
        exit 1
    fi
fi

IMAGE="cotanos-boot"

# ---------------------------------------------------------------------------
# 2. Container: make sure the image has the required tools
#    (a missing or stale image without QEMU => rebuild).
# ---------------------------------------------------------------------------
if [ -n "$CT" ]; then
    NEED_IMAGE=0
    if ! $CT image inspect "$IMAGE" >/dev/null 2>&1; then
        NEED_IMAGE=1
    elif ! $CT run --rm "$IMAGE" sh -c \
        'command -v riscv64-elf-gcc >/dev/null 2>&1 &&
         command -v qemu-system-riscv64 >/dev/null 2>&1 &&
         command -v cargo >/dev/null 2>&1' >/dev/null 2>&1; then
        echo "[!] Image $IMAGE lacks riscv64-elf-gcc/qemu-system-riscv64/cargo"
        echo "    (stale?) — rebuilding."
        NEED_IMAGE=1
    fi
    if [ "$NEED_IMAGE" = 1 ]; then
        echo "==> Building container image $IMAGE ($CT)..."
        $CT build -t "$IMAGE" -f "$ROOT/tls/container/Dockerfile.boot" "$ROOT"
    fi
fi

# ---------------------------------------------------------------------------
# 3. Build the chain: userspace (host) + CoTanSBI + ini/ + 256xtr kernel
# ---------------------------------------------------------------------------
KERNEL_REL="new/target/riscv64gc-unknown-none-elf/debug/arc-rv6"

# The userspace programs (C/C++/Rust with full std) are built on the HOST:
# the packed initrd is embedded into the kernel image afterwards (also when
# the kernel itself is built inside a container).
build_userspace_host() {
    if have riscv64-linux-gnu-gcc && have rustc && have cpio \
       && rustup target list --installed 2>/dev/null | grep -q riscv64gc-unknown-linux-gnu; then
        echo "==> Building userspace (C/C++/Rust with full std)..."
        make -C "$ROOT/new/usp"
    else
        echo "[!] Userspace toolchain missing on the host:"
        echo "    - gcc-riscv64-linux-gnu / g++-riscv64-linux-gnu"
        echo "    - rustc + target riscv64gc-unknown-linux-gnu"
        echo "      (rustup target add riscv64gc-unknown-linux-gnu)"
        echo "    - cpio"
        echo "    Install them (or build new/usp manually) and re-run."
        exit 1
    fi
}

build_chain_host() {
    echo "==> Building CoTanSBI (M-mode, own SBI)..."
    make -C "$ROOT/ini/sbi"
    echo "==> Building ini/ bootloader (S-mode, BIOS role)..."
    make -C "$ROOT/ini"
    echo "==> Building 256xtr kernel (Rust, arc-rv6)..."
    cd "$ROOT/new"
    RUSTFLAGS="-C link-arg=-T$ROOT/new/arc/rv6/link.ld" \
        cargo +nightly build -p arc-rv6 --target riscv64gc-unknown-none-elf
}

build_chain_ct() {
    $CT run --rm -v "$ROOT:/workspace:z" -w /workspace "$IMAGE" sh -c '
        set -e
        echo "==> Building CoTanSBI (M-mode, own SBI)..."
        make -C ini/sbi
        echo "==> Building ini/ bootloader (S-mode, BIOS role)..."
        make -C ini
        echo "==> Building 256xtr kernel (Rust, arc-rv6)..."
        cd new
        RUSTFLAGS="-C link-arg=-T/workspace/new/arc/rv6/link.ld" \
            cargo +nightly build -p arc-rv6 --target riscv64gc-unknown-none-elf'
}

if [ -z "$CT" ]; then
    build_userspace_host
    build_chain_host
else
    build_userspace_host
    build_chain_ct
fi

# ---------------------------------------------------------------------------
# 4. QEMU: host (native graphical window) or container (forwarded display)
# ---------------------------------------------------------------------------
FW="$ROOT/ini/sbi/fw.bin"
BOOT="$ROOT/ini/boot.bin"
KERNEL="$ROOT/$KERNEL_REL"


if have qemu-system-riscv64; then
    # --- QEMU on the host ---
    if [ "$GRAPHIC" = 1 ]; then
        # pick an available graphical backend
        if qemu-system-riscv64 -display help 2>/dev/null | grep -qw gtk; then
            DISPLAY_BE=gtk
        elif qemu-system-riscv64 -display help 2>/dev/null | grep -qw sdl; then
            DISPLAY_BE=sdl
        else
            DISPLAY_BE=default
        fi
        echo "==> Starting QEMU on the host (graphical window $DISPLAY_BE + console)..."
        echo "    (quit QEMU: Ctrl+A, then X)"
        exec qemu-system-riscv64 \
            -machine virt -cpu rv64 -smp 1 -m 128M \
            -bios "$FW" \
            -device "loader,file=$BOOT,addr=0x80200000" \
            -device "loader,file=$ROOT/ini/font_atlas.bin,addr=0x81000000" \
            -kernel "$KERNEL" \
            -device ramfb \
            -display "$DISPLAY_BE" \
            -serial mon:stdio
    else
        echo "==> Starting QEMU on the host (console, -nographic)..."
        echo "    (quit QEMU: Ctrl+A, then X)"
        exec qemu-system-riscv64 \
            -machine virt -cpu rv64 -smp 1 -m 128M \
            -bios "$FW" \
            -device "loader,file=$BOOT,addr=0x80200000" \
            -device "loader,file=$ROOT/ini/font_atlas.bin,addr=0x81000000" \
            -kernel "$KERNEL" \
            -nographic
    fi
else
    # --- QEMU inside the container ---
    if [ -z "$CT" ]; then
        CT="$(pick_ct || true)"
        if [ -z "$CT" ]; then
            echo "[!] No qemu-system-riscv64 on the host and no docker/podman."
            exit 1
        fi
    fi
    EXTRA=()
    QEMU_IN_CT="-machine virt -cpu rv64 -smp 1 -m 128M \
        -bios /workspace/ini/sbi/fw.bin \
        -device loader,file=/workspace/ini/boot.bin,addr=0x80200000 \
        -device loader,file=/workspace/ini/font_atlas.bin,addr=0x81000000 \
        -kernel /workspace/$KERNEL_REL"
    if [ "$GRAPHIC" = 1 ]; then
        if [ -z "${DISPLAY:-}" ] && [ -z "${WAYLAND_DISPLAY:-}" ]; then
            echo "[!] No DISPLAY/WAYLAND_DISPLAY — a graphical window inside the"
            echo "    container will not work. Use --nographic or install QEMU on the host."
            exit 1
        fi
        if [ -n "${DISPLAY:-}" ]; then
            EXTRA+=(-e "DISPLAY=$DISPLAY")
            if [ -d /tmp/.X11-unix ]; then
                EXTRA+=(-v /tmp/.X11-unix:/tmp/.X11-unix:ro)
            fi
        fi
        if [ -n "${XDG_RUNTIME_DIR:-}" ] && [ -n "${WAYLAND_DISPLAY:-}" ] \
           && [ -S "${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}" ]; then
            EXTRA+=( -v "${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}:${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}"
                     -e "XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR"
                     -e "WAYLAND_DISPLAY=$WAYLAND_DISPLAY" )
        fi
        QEMU_IN_CT="$QEMU_IN_CT -device ramfb -display gtk -serial mon:stdio"
        echo "==> Starting QEMU in the container (graphical via ${DISPLAY:-$WAYLAND_DISPLAY})..."
    else
        QEMU_IN_CT="$QEMU_IN_CT -nographic"
        echo "==> Starting QEMU in the container (console, -nographic)..."
    fi
    echo "    (quit QEMU: Ctrl+A, then X)"
    exec $CT run --rm -it \
        "${EXTRA[@]}" \
        -v "$ROOT:/workspace:z" \
        -w /workspace \
        "$IMAGE" sh -c "exec $QEMU_IN_CT"
fi
