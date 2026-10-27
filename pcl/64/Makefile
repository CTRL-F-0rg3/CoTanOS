VERILATOR ?= verilator
TEST      ?= tests/rv64_test1.hex

run: obj_dir/Vtb_rv64i
	./obj_dir/Vtb_rv64i +HEX=$(TEST)

obj_dir/Vtb_rv64i: rv64i.v rv64i_mem.v tb_rv64i.v
	$(VERILATOR) --binary --timing -Wno-fatal \
	    --timescale 1ns/1ps \
	    --top-module tb_rv64i \
	    rv64i.v rv64i_mem.v tb_rv64i.v

clean:
	rm -rf obj_dir dump.vcd