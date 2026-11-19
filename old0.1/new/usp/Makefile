# SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
# SPDX-License-Identifier: GPL-3.0-only
#
# usp/ — CoTanOS userspace demo programs (C, C++, Rust — all with the
# full standard libraries).
#
# Everything is an ordinary STATIC riscv64-linux binary: the 256xtr kernel
# implements the Linux syscall ABI, so:
#   * C    -> glibc (riscv64-linux-gnu-gcc)
#   * C++  -> libstdc++ (riscv64-linux-gnu-g++)
#   * Rust -> the real, precompiled std (target riscv64gc-unknown-linux-gnu)
#
# Output: initrd.cpio (newc) placed into ../arc/rv6/initrd.cpio, embedded
# into the kernel image at compile time.

CROSS ?= riscv64-linux-gnu-
CC     := $(CROSS)gcc
CXX    := $(CROSS)g++

# The workspace pins nightly (new/rust-toolchain.toml), but the Linux-gnu
# std is installed on the stable toolchain — pin it explicitly here so the
# build does not depend on which toolchain the caller has active.
RUSTC       ?= rustup run stable rustc
RUST_TARGET := riscv64gc-unknown-linux-gnu

ROOTFS := rootfs
BIN    := $(ROOTFS)/bin

CFLAGS  := -static -O2 -no-pie -Wall
CXXFLAGS := $(CFLAGS) -fno-exceptions -fno-rtti
RUSTFLAGS := --target $(RUST_TARGET) -C target-feature=+crt-static \
             -C linker=$(CROSS)gcc -C link-arg=-static -O

CPIO := $(abspath ../arc/rv6/initrd.cpio)

all: $(CPIO)

$(BIN)/init: src/init.c | $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/hello-cpp: src/hello.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BIN)/hello-rs: src/hello.rs | $(BIN)
	$(RUSTC) $(RUSTFLAGS) $< -o $@

$(BIN):
	mkdir -p $(BIN)

$(CPIO): $(BIN)/init $(BIN)/hello-cpp $(BIN)/hello-rs
	cd $(ROOTFS) && find . -print | cpio -o -H newc > $(CPIO)

clean:
	rm -f $(CPIO)
	rm -rf $(ROOTFS)/bin

.PHONY: all clean
