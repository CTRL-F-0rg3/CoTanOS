CC = riscv64-elf-gcc
AR = riscv64-elf-ar
CFLAGS = -Wall -Wextra -O2 -Iinclude -march=rv64imafd -mabi=lp64d -mcmodel=medany -ffreestanding

SRCS = src/uart.c src/display/edid.c src/display/drm_modes_adapter.c src/display/cea_parser.c src/display/qemu_vga.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: libcotanbase.a

libcotanbase.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f libcotanbase.a $(OBJS)
