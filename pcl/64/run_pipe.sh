#!/usr/bin/env bash
set -e
SRC="${1:?usage: $0 plik.S}"
BASE="${SRC%.S}"

if [ ! -x obj_dir/Vtb_rv64i_pipe ] || \
   [ rv64i_pipe.v -nt obj_dir/Vtb_rv64i_pipe ] || \
   [ rv64i_csr.v  -nt obj_dir/Vtb_rv64i_pipe ] || \
   [ rv64i_mem.v  -nt obj_dir/Vtb_rv64i_pipe ] || \
   [ tb_rv64i_pipe.v -nt obj_dir/Vtb_rv64i_pipe ]; then
    echo "[build] verilator pipeline..."
    make obj_dir/Vtb_rv64i_pipe >/dev/null
fi

riscv64-linux-gnu-gcc -march=rv64im_zicsr -mabi=lp64 -mcmodel=medany \
    -fno-pic -mno-relax -nostdlib -nostartfiles \
    -T link.ld "$SRC" -o "$BASE.elf"
riscv64-linux-gnu-objcopy -O binary "$BASE.elf" "$BASE.bin"
python3 - "$BASE.bin" > "$BASE.hex" <<'PY'
import sys
d = open(sys.argv[1],'rb').read()
while len(d) % 4: d += b'\x00'
for i in range(0, len(d), 4):
    print(f'{int.from_bytes(d[i:i+4], "little"):08x}')
PY
./obj_dir/Vtb_rv64i_pipe +HEX="$BASE.hex"
