VERILATOR ?= verilator
TEST      ?= tests/rv64_test1.hex

run: obj_dir/Vtb_rv64i
	./obj_dir/Vtb_rv64i +HEX=$(TEST)

run-pipe: obj_dir/Vtb_rv64i_pipe
	./obj_dir/Vtb_rv64i_pipe +HEX=$(TEST)

obj_dir/Vtb_rv64i: rv64i.v rv64i_csr.v rv64i_mem.v tb_rv64i.v
	$(VERILATOR) --binary --timing -Wno-fatal --timescale 1ns/1ps \
	    --top-module tb_rv64i \
	    rv64i.v rv64i_csr.v rv64i_mem.v tb_rv64i.v

obj_dir/Vtb_rv64i_pipe: rv64i_pipe.v rv64i_csr.v rv64i_mem.v tb_rv64i_pipe.v
	$(VERILATOR) --binary --timing -Wno-fatal --timescale 1ns/1ps \
	    --top-module tb_rv64i_pipe \
	    rv64i_pipe.v rv64i_csr.v rv64i_mem.v tb_rv64i_pipe.v

clean:
	rm -rf obj_dir dump.vcd