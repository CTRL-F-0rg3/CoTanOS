CROSS   ?= riscv64-elf-
CFLAGS  := -Os -ffreestanding -fno-pic -fno-builtin -nostdlib \
           -march=rv64imac -mabi=lp64 -Wall -Wextra
BUILD   := build

all: $(BUILD)/hello.bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/hello.elf: hello/main.c lib/crt0.S lib/syscall.h link.ld | $(BUILD)
	$(CROSS)gcc $(CFLAGS) -T link.ld -Wl,--build-id=none \
	    lib/crt0.S hello/main.c -o $@

$(BUILD)/%.bin: $(BUILD)/%.elf
	$(CROSS)objcopy -O binary $< $@
	@ls -l $@
	@if [ $$(stat -c%s $@) -gt 4096 ]; then \
	    echo "ERROR: $@ exceeds one page (kernel maps 1 page)"; exit 1; fi

clean:
	rm -rf $(BUILD)
.PHONY: all clean

$(BUILD)/daemon_mgr.bin: daemon_mgr/main.c lib/syscall.h link.ld | $(BUILD)
	$(CROSS)gcc $(CFLAGS) -T link.ld -Wl,--build-id=none \
	    daemon_mgr/main.c -o $@
	$(CROSS)objcopy -O binary $@ $(BUILD)/daemon_mgr.bin
	@ls -l $(BUILD)/daemon_mgr.bin
