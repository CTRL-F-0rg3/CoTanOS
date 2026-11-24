CC = riscv64-linux-gnu-gcc
CFLAGS = -Wall -Wextra -O2 -fPIC -I./include

SRCS = src/libdrm_cotan.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: libdrm_cotan.so

libdrm_cotan.so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f libdrm_cotan.so $(OBJS)
