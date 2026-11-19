# --- DP/HDMI driver sources -------------------------------------------------
DP_SRC := \
    src/core_io.c \
    src/dp_core.c \
    src/dp_selftest.c

DP_CXX_SRC := \
    src/dp_controller.cpp

DP_INC := -Iinclude -I.

DP_CFLAGS   := $(CFLAGS)   $(DP_INC) -std=c11   -Wall -Wextra -Wno-unused-parameter
DP_CXXFLAGS := $(CXXFLAGS) $(DP_INC) -std=c++17 -Wall -Wextra -Wno-unused-parameter

DP_OBJ_C   := $(DP_SRC:.c=.o)
DP_OBJ_CXX := $(DP_CXX_SRC:.cpp=.o)
DP_OBJ     := $(DP_OBJ_C) $(DP_OBJ_CXX)

# Library target — link this into the kernel image
drv_dp.a: $(DP_OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC)  $(DP_CFLAGS)   -c $< -o $@

%.o: %.cpp
	$(CXX) $(DP_CXXFLAGS) -c $< -o $@

# Standalone selftest binary (runs on QEMU or real HW)
selftest_dp.elf: $(DP_OBJ) src/selftest_entry.o
	$(CXX) $(DP_CXXFLAGS) -o $@ $^ -T $(LINKER_SCRIPT)

# Optional: build the driver as part of the full kernel
kernel: drv_dp.a
	# ... existing kernel link step ...
	$(CXX) ... -Wl,--start-group drv_dp.a $(OTHER_DRV_LIBS) -Wl,--end-group ...

.PHONY: selftest-driver
selftest-driver: selftest_dp.elf
	@echo "Running DP selftest..."
	qemu-system-riscv64 -machine virt -bios none -kernel selftest_dp.elf -nographic
