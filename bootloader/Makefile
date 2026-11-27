NASM     := nasm
CC       := gcc
LD       := ld
OBJCOPY  := objcopy

ROOT     := ..
BUILD    := $(ROOT)/build
ESP_DIR  := $(BUILD)/esp
DISK_BIOS := $(ROOT)/disk_x86_bios.img
DISK_SIZE_BIOS := 5G

BIOS_S1  := boot_x86_64_bios.s
BIOS_S2  := boot_x86_64_bios_stage_2.s
S1_BIN   := stage1.bin
S2_BIN   := stage2.bin

UEFI_C   := boot_x86_64_uefi.c
UEFI_S   := boot_x86_64_uefi.s
UEFI_SO  := boot.so
UEFI_EFI := BOOTX64.EFI

RISCV_S  := boot_riscv.s
RISCV_BIN := $(BUILD)/boot_riscv.bin

# --- gnu-efi (Arch/CachyOS: /usr/include/efi, /usr/lib/*) ---
GNUEfi_PREFIX ?= /usr
EFI_INC := $(GNUEfi_PREFIX)/include/efi
CRT0    := $(GNUEfi_PREFIX)/lib/crt0-efi-x86_64.o
LDS     := $(GNUEfi_PREFIX)/lib/elf_x86_64_efi.lds

.PHONY: all bios uefi riscv install-bios install-uefi check-gnuefi clean clean-all

all: bios uefi

# ============ BIOS ============
bios: $(S1_BIN) $(S2_BIN)
	@echo "[BIOS] stage1=$(shell stat -c%s $(S1_BIN))B stage2=$(shell stat -c%s $(S2_BIN))B"

$(S1_BIN): $(BIOS_S1) kernel_abi.inc
	$(NASM) -DSTAGE1 -f bin $< -o $@

$(S2_BIN): $(BIOS_S2) kernel_abi.inc
	$(NASM) -f bin $< -o $@

install-bios: bios
	rm -f $(DISK_BIOS)
	truncate -s $(DISK_SIZE_BIOS) $(DISK_BIOS)
	dd if=$(S1_BIN) of=$(DISK_BIOS) conv=notrunc bs=512 seek=0 status=none
	dd if=$(S2_BIN) of=$(DISK_BIOS) conv=notrunc bs=512 seek=1 status=none
	@echo "[BIOS] zainstalowano w $(DISK_BIOS)"

# ============ UEFI ============
uefi: check-gnuefi $(UEFI_EFI)

check-gnuefi:
	@test -f $(CRT0)        || { echo "[-] brak $(CRT0) — sudo pacman -S gnu-efi"; exit 1; }
	@test -f $(LDS)         || { echo "[-] brak $(LDS)"; exit 1; }
	@test -f $(EFI_INC)/efi.h || { echo "[-] brak $(EFI_INC)/efi.h"; exit 1; }

boot_c.o: $(UEFI_C) kernel_abi.inc
	$(CC) -fpic -ffreestanding -fno-stack-protector -mno-red-zone -fcf-protection=none \
	      -fshort-wchar -DGNU_EFI_USE_MS_ABI \
	      -I$(EFI_INC) -I$(EFI_INC)/x86_64 \
	      -c $< -o $@

boot_s.o: $(UEFI_S)
	$(CC) -fpic -ffreestanding -fno-stack-protector -mno-red-zone -fcf-protection=none \
	      -c $< -o $@

$(UEFI_SO): boot_c.o boot_s.o $(CRT0)
	@echo "Linking $(UEFI_SO)"
	$(LD) -nostdlib -znocombreloc -T $(LDS) -shared -Bsymbolic \
	      $(CRT0) boot_c.o boot_s.o \
	      -L$(GNUEfi_PREFIX)/lib -lefi -lgnuefi -o $@

$(UEFI_EFI): $(UEFI_SO)
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic \
	           -j .dynsym -j .rel -j .rela -j .reloc \
	           --target=efi-app-x86_64 $< $@

install-uefi: uefi
	mkdir -p $(ESP_DIR)/EFI/BOOT
	cp $(UEFI_EFI) $(ESP_DIR)/EFI/BOOT/BOOTX64.EFI
	@echo "[UEFI] $(ESP_DIR)/EFI/BOOT/BOOTX64.EFI"

# ============ RISC-V ============
riscv: $(RISCV_BIN)

$(RISCV_BIN): $(RISCV_S)
	@mkdir -p $(BUILD)
	$(NASM) -f bin $< -o $@

# ============ clean ============
clean:
	rm -f *.o *.bin *.so *.EFI
	rm -rf $(ESP_DIR)

clean-all: clean
	rm -f $(DISK_BIOS)
