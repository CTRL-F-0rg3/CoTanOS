# SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
# SPDX-License-Identifier: GPL-3.0-only
#
# CoTanSBI — M-mode firmware for RISC-V (QEMU 'virt').
# Output: fw.bin (raw image loaded by QEMU `-bios`).

CROSS   ?= riscv64-elf-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy

ARCH    := rv64ima_zicsr_zifencei
ABI     := lp64

CFLAGS  := -march=$(ARCH) -mabi=$(ABI) -mcmodel=medany \
           -ffreestanding -nostdlib -fno-builtin -fno-stack-protector \
           -msmall-data-limit=0 -fno-pic \
           -O2 -Wall -Wextra -I.

LDFLAGS := -nostdlib -static -T fw.ld

OBJS := entry.o trap.o uart.o sbi.o sbi_main.o

HDRS := sbi.h uart.h clint.h

all: fw.bin

fw.bin: fw.elf
	$(OBJCOPY) -O binary $< $@

fw.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o fw.elf fw.bin

.PHONY: all clean
