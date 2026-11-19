#!/usr/bin/env bash
set -e
SRC="${1:?usage: $0 plik.S}"
BASE="${SRC%.S}"

# auto-build jeśli brak binarki lub źródła nowsze
if [ ! -x obj_dir/Vtb_rv64i ] \
   || [ rv64i.v -nt obj_dir/Vtb_rv64i ] \
   || [ rv64i_csr.v -nt obj_dir/Vtb_rv64i ] \
   || [ rv64i_mem.v -nt obj_dir/Vtb_rv64i ] \
   || [ tb_rv64i.v -nt obj_dir/Vtb_rv64i ]; then
    echo "[build] verilator..."
    make obj_dir/Vtb_rv64i >/dev/null
fi

echo "[1/4] kompilacja $SRC → $BASE.elf"
riscv64-linux-gnu-gcc -march=rv64im_zicsr -mabi=lp64 -mcmodel=medany \
    -fno-pic -mno-relax -nostdlib -nostartfiles \
    -T link.ld "$SRC" -o "$BASE.elf"

echo "[2/4] objcopy → $BASE.bin"
riscv64-linux-gnu-objcopy -O binary "$BASE.elf" "$BASE.bin"

echo "[3/4] bin → hex (byte-swap little-endian → słowa 32-bit)"
python3 - "$BASE.bin" > "$BASE.hex" <<'PY'
import sys
d = open(sys.argv[1],'rb').read()
while len(d) % 4: d += b'\x00'
for i in range(0, len(d), 4):
    print(f'{int.from_bytes(d[i:i+4], "little"):08x}')
PY

echo "[4/4] symulacja"
./obj_dir/Vtb_rv64i +HEX="$BASE.hex"
