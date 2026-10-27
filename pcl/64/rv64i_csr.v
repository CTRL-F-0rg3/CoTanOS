module rv64i_csr (
    input  wire        clk,
    input  wire        rst,
    input  wire        csr_active,
    input  wire [11:0] csr_addr,
    input  wire [2:0]  csr_op,
    input  wire [63:0] csr_src,
    output wire [63:0] csr_rdata,
    output wire        csr_instr_illegal,
    input  wire        trap_take,
    input  wire [63:0] trap_pc,
    input  wire [63:0] trap_cause,
    input  wire [63:0] trap_tval,
    input  wire        mret,
    output wire [63:0] mtvec_out,
    output wire [63:0] mepc_out,
    output wire        mstatus_mie
);
    reg [63:0] mstatus_reg, misa_reg, mie_reg, mtvec_reg, mscratch_reg;
    reg [63:0] mepc_reg, mcause_reg, mtval_reg, mip_reg, mhartid_reg;

    reg csr_exists;
    always @(*) begin
        case (csr_addr)
            12'h300, 12'h301, 12'h304, 12'h305,
            12'h340, 12'h341, 12'h342, 12'h343, 12'h344,
            12'hF14: csr_exists = 1'b1;
            default: csr_exists = 1'b0;
        endcase
    end

    wire csr_ro = (csr_addr[11:10] == 2'b11);

    reg csr_should_write;
    always @(*) begin
        case (csr_op)
            3'b001, 3'b101: csr_should_write = 1'b1;
            3'b010, 3'b011, 3'b110, 3'b111: csr_should_write = (csr_src != 64'd0);
            default: csr_should_write = 1'b0;
        endcase
    end

    assign csr_instr_illegal = csr_active && (!csr_exists || (csr_ro && csr_should_write));

    reg [63:0] csr_rdata_reg;
    always @(*) begin
        case (csr_addr)
            12'h300: csr_rdata_reg = mstatus_reg | 64'h1800;
            12'h301: csr_rdata_reg = misa_reg;
            12'h304: csr_rdata_reg = mie_reg;
            12'h305: csr_rdata_reg = mtvec_reg;
            12'h340: csr_rdata_reg = mscratch_reg;
            12'h341: csr_rdata_reg = mepc_reg;
            12'h342: csr_rdata_reg = mcause_reg;
            12'h343: csr_rdata_reg = mtval_reg;
            12'h344: csr_rdata_reg = mip_reg;
            12'hF14: csr_rdata_reg = mhartid_reg;
            default: csr_rdata_reg = 64'd0;
        endcase
    end
    assign csr_rdata = csr_rdata_reg;

    reg [63:0] csr_new_val;
    always @(*) begin
        case (csr_op)
            3'b001, 3'b101: csr_new_val = csr_src;
            3'b010, 3'b110: csr_new_val = csr_rdata | csr_src;
            3'b011, 3'b111: csr_new_val = csr_rdata & ~csr_src;
            default:        csr_new_val = csr_rdata;
        endcase
    end

    wire csr_we = csr_active && csr_should_write && !csr_instr_illegal
                  && !trap_take && !mret;

    always @(posedge clk) begin
        if (rst) begin
            mstatus_reg  <= 64'h0000_0000_0000_1800;
            misa_reg     <= 64'h8000_0000_0000_0100;
            mie_reg      <= 64'd0;
            mtvec_reg    <= 64'd0;
            mscratch_reg <= 64'd0;
            mepc_reg     <= 64'd0;
            mcause_reg   <= 64'd0;
            mtval_reg    <= 64'd0;
            mip_reg      <= 64'd0;
            mhartid_reg  <= 64'd0;
        end else if (trap_take) begin
            mepc_reg    <= trap_pc & ~64'd3;
            mcause_reg  <= trap_cause;
            mtval_reg   <= trap_tval;
            mstatus_reg[7]     <= mstatus_reg[3];
            mstatus_reg[3]     <= 1'b0;
            mstatus_reg[12:11] <= 2'b11;
        end else if (mret) begin
            mstatus_reg[3]     <= mstatus_reg[7];
            mstatus_reg[7]     <= 1'b1;
            mstatus_reg[12:11] <= 2'b11;
        end else if (csr_we) begin
            case (csr_addr)
                12'h300: begin
                    mstatus_reg[3]     <= csr_new_val[3];
                    mstatus_reg[7]     <= csr_new_val[7];
                    mstatus_reg[12:11] <= 2'b11;
                end
                12'h304: mie_reg      <= csr_new_val;
                12'h305: mtvec_reg    <= csr_new_val & ~64'd3;
                12'h340: mscratch_reg <= csr_new_val;
                12'h341: mepc_reg     <= csr_new_val & ~64'd3;
                12'h342: mcause_reg   <= csr_new_val;
                12'h343: mtval_reg    <= csr_new_val;
                default: ;
            endcase
        end
    end

    assign mtvec_out   = mtvec_reg;
    assign mepc_out    = mepc_reg;
    assign mstatus_mie = mstatus_reg[3];
endmodule
