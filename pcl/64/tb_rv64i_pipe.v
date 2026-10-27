`timescale 1ns/1ps
module tb_rv64i_pipe;
    reg clk=0, rst=1;

    wire [63:0] imem_addr, dmem_addr, dmem_wdata, dmem_rdata;
    wire [31:0] imem_data;
    wire        dmem_we, dmem_re;
    wire [1:0]  dmem_size;
    wire        halted;
    wire        tohost_we;
    wire [63:0] tohost_data;
    wire [63:0] dbg_pc;

    rv64i_pipe_core u_core (
        .clk(clk), .rst(rst),
        .imem_addr(imem_addr), .imem_data(imem_data),
        .dmem_addr(dmem_addr), .dmem_wdata(dmem_wdata),
        .dmem_we(dmem_we), .dmem_re(dmem_re), .dmem_size(dmem_size),
        .dmem_rdata(dmem_rdata), .halted(halted),
        .dbg_pc(dbg_pc)
    );

    rv64i_mem u_mem (
        .clk(clk),
        .iaddr(imem_addr), .idata(imem_data),
        .daddr(dmem_addr), .dwdata(dmem_wdata),
        .dwe(dmem_we), .dre(dmem_re), .dsize(dmem_size),
        .drdata(dmem_rdata),
        .tohost_we(tohost_we), .tohost_data(tohost_data)
    );

    always #5 clk = ~clk;
    integer cycles = 0;
    reg pass;

    initial begin
        #20 rst = 0;
        while (!halted && !tohost_we && cycles < 200000) begin
            @(posedge clk);
            cycles = cycles + 1;
        end
        if (tohost_we) begin
            pass = (tohost_data == 64'd1);
            $display("[tohost] value=0x%016x", tohost_data);
        end else if (halted) begin
            pass = 1'b0;
            $display("[halt-bez-tohost] cykli=%0d", cycles);
        end else begin
            pass = 1'b0;
            $display("[timeout] cykli=%0d", cycles);
        end
        $display("pc =0x%016x", dbg_pc);
        $display("x10=0x%016x  x11=0x%016x", u_core.u_rf.regs[10], u_core.u_rf.regs[11]);
        $display("x12=0x%016x  x13=0x%016x", u_core.u_rf.regs[12], u_core.u_rf.regs[13]);
        if (pass) $display("*** PASS ***");
        else      $display("*** FAIL ***");
        $finish;
    end
endmodule
