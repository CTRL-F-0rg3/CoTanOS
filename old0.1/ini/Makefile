# SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
# SPDX-License-Identifier: GPL-3.0-only
#
# ini/ — CoTanOS Hardware Bootloader (S-mode, BIOS role).
# Output: boot.bin (raw image loaded by QEMU `-device loader`
# at 0x8020_0000).

CROSS   ?= riscv64-elf-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy

ARCH    := rv64ima_zicsr_zifencei
ABI     := lp64

CFLAGS  := -march=$(ARCH) -mabi=$(ABI) -mcmodel=medany \
           -ffreestanding -nostdlib -fno-builtin -fno-stack-protector \
           -msmall-data-limit=0 -fno-pic \
           -O2 -Wall -Wextra -I. -Isbi

LDFLAGS := -nostdlib -static -T link.ld

OBJS := boot.o trap.o main.o fdt.o gfx.o

HDRS := bootinfo.h fdt.h gfx.h font_atlas.h sbi/sbi.h

all: boot.bin

boot.bin: boot.elf
	$(OBJCOPY) -O binary $< $@

boot.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o boot.elf boot.bin

.PHONY: all clean

