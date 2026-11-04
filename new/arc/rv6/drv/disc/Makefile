# ============================================================================
#  CoTanOS Storage Driver Suite — Makefile
# ============================================================================
#  RISC-V RV32 / RV64 · C11 / C++17 · freestanding · bare-metal
#
#  Usage:
#    make                  Build build/libstorage.a
#    make objects          Build individual .o files (for direct kernel link)
#    make selftest         Build build/selftest.elf (requires selftest/platform)
#    make clean            Remove build artifacts
#    make distclean        Remove build + generated docs
#    make size             Print archive footprint
#    make info             Print resolved toolchain and flags
#    make help             This help
#
#  Variables (override on command line or via env):
#    CROSS_COMPILE   Toolchain prefix            (riscv64-unknown-elf-)
#    ARCH            rv32 | rv64                 (rv64)
#    DEBUG           Set to 1 for -O0 -g          (0)
#    TIMEBASE_HZ     time CSR frequency, Hz      (1000000)
#    BUILD           Output directory            (build)
#    DESTDIR         Install prefix              (empty = no install)
#    V               Set to 1 for verbose build  (0)
# ============================================================================

# ============================================================================
# 1. Toolchain
# ============================================================================

CROSS_COMPILE ?= riscv64-unknown-elf-

CC      := $(CROSS_COMPILE)gcc
CXX     := $(CROSS_COMPILE)g++
AR      := $(CROSS_COMPILE)ar
LD      := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
SIZE    := $(CROSS_COMPILE)size
NM      := $(CROSS_COMPILE)nm

# ============================================================================
# 2. Architecture
# ============================================================================

ARCH ?= rv64

ifeq ($(ARCH),rv64)
  ARCH_FLAGS  := -march=rv64gc -mabi=lp64d -mcmodel=medany
  ARCH_LD_EMUL := elf64lriscv
else ifeq ($(ARCH),rv32)
  ARCH_FLAGS  := -march=rv32imac -mabi=ilp32 -mcmodel=medlow
  ARCH_LD_EMUL := elf32lriscv
else
  $(error ARCH must be 'rv32' or 'rv64' (got '$(ARCH)'))
endif

# ============================================================================
# 3. Common flags
# ============================================================================

BUILD ?= build
V     ?= 0

ifeq ($(V),1)
  Q :=
else
  Q := @
endif

DEBUG       ?= 0
TIMEBASE_HZ ?= 1000000

WARN_FLAGS := \
  -Wall -Wextra \
  -Wno-unused-parameter \
  -Wno-missing-field-initializers \
  -Wshadow \
  -Wpointer-arith \
  -Wcast-align \
  -Wstrict-prototypes \
  -Wmissing-prototypes \
  -Wno-discarded-qualifiers

FREESTANDING_FLAGS := \
  -ffreestanding \
  -fno-builtin \
  -fno-common \
  -fno-stack-protector \
  -nostdlib \
  -nostdinc \
  -fno-pic -fno-pie

OPT_FLAGS := -O2
ifeq ($(DEBUG),1)
  OPT_FLAGS := -O0 -g -DDEBUG=1
endif

DEFINES := -DSTOR_TIMEBASE_HZ=$(TIMEBASE_HZ)UL

INCLUDES := -Iinclude

CFLAGS := \
  $(ARCH_FLAGS) \
  $(OPT_FLAGS) \
  $(FREESTANDING_FLAGS) \
  $(WARN_FLAGS) \
  $(DEFINES) \
  $(INCLUDES) \
  -std=c11 \
  -MMD -MP

CXXFLAGS := \
  $(ARCH_FLAGS) \
  $(OPT_FLAGS) \
  $(FREESTANDING_FLAGS) \
  -Wall -Wextra \
  -Wno-unused-parameter \
  -Wno-missing-field-initializers \
  -fno-exceptions \
  -fno-rtti \
  -fno-threadsafe-statics \
  -fno-use-cxa-atexit \
  -std=c++17 \
  $(DEFINES) \
  $(INCLUDES) \
  -MMD -MP

LDFLAGS := \
  $(ARCH_FLAGS) \
  -nostdlib \
  -nostartfiles \
  -Wl,--build-id=none \
  -Wl,--gc-sections

# ============================================================================
# 4. Sources
# ============================================================================
# Layout assumed:
#   include/stor_mmio.h
#   include/stor_mm.h
#   include/storage_core.h
#   include/storage_controller.hpp
#   src/stor_mm.c
#   src/stor_pci.c
#   src/storage_core.c
#   src/storage_controller.cpp   (optional)
#   selftest/                    (optional — for make selftest)

C_SRCS := \
  src/stor_mm.c \
  src/stor_pci.c \
  src/storage_core.c

CXX_SRCS := src/storage_controller.cpp

# Filter out sources that don't exist (allows partial trees during bring-up).
C_SRCS   := $(wildcard $(C_SRCS))
CXX_SRCS := $(wildcard $(CXX_SRCS))

C_OBJS   := $(patsubst src/%.c,$(BUILD)/%.o,$(C_SRCS))
CXX_OBJS := $(patsubst src/%.cpp,$(BUILD)/%.o,$(CXX_SRCS))
OBJS     := $(C_OBJS) $(CXX_OBJS)
DEPS     := $(OBJS:.o=.d)

LIB := $(BUILD)/libstorage.a

HEADERS := \
  include/stor_mmio.h \
  include/stor_mm.h \
  include/storage_core.h \
  include/storage_controller.hpp

HEADERS := $(wildcard $(HEADERS))

# ============================================================================
# 5. Top-level targets
# ============================================================================

.PHONY: all lib objects selftest clean distclean size info help install
.PHONY: FORCE

all: $(LIB)

lib: $(LIB)

objects: $(OBJS)
	@echo "Built $(words $(OBJS)) object file(s) in $(BUILD)/"

$(LIB): $(OBJS)
	@mkdir -p $(dir $@)
	@echo "  AR   $@"
	$(Q)rm -f $@
	$(Q)$(AR) rcs $@ $^
	@echo "  OK   $(LIB)  ($(words $(OBJS)) objects)"

# ============================================================================
# 6. Compile rules
# ============================================================================

$(BUILD)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "  CC   $<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX  $<"
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================================================
# 7. Optional standalone selftest binary
# ============================================================================
# Requires:
#   selftest/entry.c        — _start() entry point
#   selftest/platform.c     — implements stor_mm_ops_t + stor_log_fn
#   selftest/linker.ld      — linker script (MEMORY / SECTIONS)
#
# The binary links libstorage.a and calls stor_selftest_run_all().
# Override SELFTEST_ENTRY / SELFTEST_LD / SELFTEST_PLATFORM if using
# a different layout.

SELFTEST_DIR      ?= selftest
SELFTEST_ENTRY    ?= $(SELFTEST_DIR)/entry.c
SELFTEST_PLATFORM ?= $(SELFTEST_DIR)/platform.c
SELFTEST_LD       ?= $(SELFTEST_DIR)/linker.ld
SELFTEST_ELF      := $(BUILD)/selftest.elf
SELFTEST_BIN      := $(BUILD)/selftest.bin
SELFTEST_MAP      := $(BUILD)/selftest.map

SELFTEST_CSRCS := $(wildcard $(SELFTEST_ENTRY) $(SELFTEST_PLATFORM))
SELFTEST_OBJS  := $(patsubst $(SELFTEST_DIR)/%.c,$(BUILD)/selftest/%.o,$(SELFTEST_CSRCS))

selftest: $(SELFTEST_ELF)

$(SELFTEST_ELF): $(LIB) $(SELFTEST_OBJS) $(SELFTEST_LD)
	@if [ ! -f "$(SELFTEST_LD)" ]; then \
		echo "ERROR: selftest linker script missing: $(SELFTEST_LD)"; \
		echo "       Provide one in selftest/ or override SELFTEST_LD=..."; \
		exit 1; \
	fi
	@if [ -z "$(SELFTEST_OBJS)" ]; then \
		echo "ERROR: selftest entry/platform missing in $(SELFTEST_DIR)/"; \
		exit 1; \
	fi
	@mkdir -p $(dir $@)
	@echo "  LD   $@"
	$(Q)$(CC) $(LDFLAGS) \
		-T $(SELFTEST_LD) \
		-Wl,-Map=$(SELFTEST_MAP) \
		-Wl,--print-memory-usage \
		-o $@ \
		$(SELFTEST_OBJS) \
		$(LIB)
	@$(SIZE) $@ || true
	@echo "  OK   $(SELFTEST_ELF)"

$(BUILD)/selftest/%.o: $(SELFTEST_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "  CC   $<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# 8. Size / diagnostics
# ============================================================================

size: $(LIB)
	@echo "--- Archive footprint ---"
	@$(SIZE) --total --format=berkeley $(OBJS) 2>/dev/null || $(SIZE) $(OBJS)
	@echo ""
	@echo "--- Symbol counts ---"
	@$(NM) --defined-only $(LIB) 2>/dev/null | grep -c ' T \| t ' | \
		awk '{print "  text symbols : " $$1}'
	@$(NM) --defined-only $(LIB) 2>/dev/null | grep -c ' D \| d ' | \
		awk '{print "  data symbols : " $$1}'
	@$(NM) --defined-only $(LIB) 2>/dev/null | grep -c ' B \| b ' | \
		awk '{print "  bss symbols  : " $$1}'

info:
	@echo "CoTanOS Storage Driver Suite"
	@echo "============================"
	@echo "  Toolchain prefix : $(CROSS_COMPILE)"
	@echo "  CC               : $(CC)"
	@echo "  CXX              : $(CXX)"
	@echo "  AR               : $(AR)"
	@echo "  ARCH             : $(ARCH)"
	@echo "  ARCH_FLAGS       : $(ARCH_FLAGS)"
	@echo "  DEBUG            : $(DEBUG)"
	@echo "  TIMEBASE_HZ      : $(TIMEBASE_HZ)"
	@echo "  BUILD            : $(BUILD)"
	@echo "  Verbose (V)      : $(V)"
	@echo ""
	@echo "  C sources        : $(words $(C_SRCS)) [$(C_SRCS)]"
	@echo "  C++ sources      : $(words $(CXX_SRCS)) [$(CXX_SRCS)]"
	@echo "  Objects          : $(words $(OBJS))"
	@echo "  Library          : $(LIB)"
	@echo "  Headers          : $(words $(HEADERS))"
	@echo ""
	@echo "  CFLAGS           : $(CFLAGS)"
	@echo "  CXXFLAGS         : $(CXXFLAGS)"
	@echo "  LDFLAGS          : $(LDFLAGS)"

help:
	@echo "CoTanOS Storage Driver Suite — Build targets"
	@echo ""
	@echo "  make                Build static archive (default)"
	@echo "  make lib            Same as 'make'"
	@echo "  make objects        Build individual .o files"
	@echo "  make selftest       Build standalone selftest ELF (needs selftest/)"
	@echo "  make size           Print archive footprint"
	@echo "  make info           Print resolved toolchain and flags"
	@echo "  make clean          Remove $(BUILD)/"
	@echo "  make distclean      Remove $(BUILD)/ and generated docs"
	@echo "  make install        Copy headers to DESTDIR (if set)"
	@echo ""
	@echo "Common overrides:"
	@echo "  ARCH=rv32           Build for RV32 instead of default rv64"
	@echo "  DEBUG=1             -O0 -g, adds -DDEBUG=1"
	@echo "  TIMEBASE_HZ=N       Override time CSR frequency"
	@echo "  V=1                 Verbose output"
	@echo "  BUILD=path          Custom output directory"
	@echo "  DESTDIR=path        Install prefix (make install)"

# ============================================================================
# 9. Install (headers only — library is linked statically into the kernel)
# ============================================================================

INSTALL_INCDIR ?= $(DESTDIR)/usr/include/cotanos/storage

install: $(HEADERS)
	@if [ -z "$(DESTDIR)" ]; then \
		echo "ERROR: set DESTDIR=/path/to/rootfs (e.g. DESTDIR=/tmp/root)"; \
		exit 1; \
	fi
	@echo "  INSTALL  headers -> $(INSTALL_INCDIR)"
	$(Q)mkdir -p $(INSTALL_INCDIR)
	$(Q)for h in $(HEADERS); do \
		install -m 0644 "$$h" "$(INSTALL_INCDIR)/$$(basename $$h)"; \
	done
	@echo "  INSTALL  $(LIB) -> $(DESTDIR)/usr/lib/libstorage.a"
	$(Q)mkdir -p $(DESTDIR)/usr/lib
	$(Q)install -m 0644 $(LIB) $(DESTDIR)/usr/lib/libstorage.a

# ============================================================================
# 10. Clean
# ============================================================================

clean:
	@echo "  CLEAN    $(BUILD)/"
	$(Q)rm -rf $(BUILD)

distclean: clean
	@echo "  CLEAN    docs artifacts"
	$(Q)rm -f docs/*.tmp docs/*.bak

# ============================================================================
# 11. Dependency files
# ============================================================================

-include $(DEPS)

# ============================================================================
# 12. Sanity checks
# ============================================================================
# Warn once at build time if the caller didn't override TIMEBASE_HZ.
# The default (1 MHz) works for QEMU virt and most real RISC-V SoCs,
# but kernels with a different time CSR will get incorrect delays.

ifeq ($(TIMEBASE_HZ),1000000)
$(info Note: TIMEBASE_HZ defaults to 1000000 Hz. Override with TIMEBASE_HZ=N)
endif
