CC ?= gcc
CFLAGS = -Wall -Wextra -O2 -fPIC -I/home/ctrl/project/CoTanOS/ZFC-kernel-base/include/sys

SRCS = libdrm_cotan.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: libdrm_cotan.so

libdrm_cotan.so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f libdrm_cotan.so $(OBJS)
