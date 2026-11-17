// rv64i_pipe.v - 5-stage pipeline RV64IM (no CSR yet)
// IF → ID → EX → MEM → WB
// Forwarding: EX/MEM→EX, MEM/WB→EX
// Stall: load-use (1 cycle)
// Flush: branch taken (2 instr), jal/jalr, halt

// ---------- ALU (identyczny jak single-cycle) ----------
module rv64i_pipe_alu (
    input  wire [63:0] a,
    input  wire [63:0] b,
    input  wire [4:0]  op,
    output reg  [63:0] y
);
    localparam ALU_ADD=5'd0, ALU_SUB=5'd1, ALU_SLL=5'd2, ALU_SLT=5'd3,
               ALU_SLTU=5'd4, ALU_XOR=5'd5, ALU_SRL=5'd6, ALU_SRA=5'd7,
               ALU_OR=5'd8, ALU_AND=5'd9, ALU_PASS=5'd10, ALU_ADDW=5'd11,
               ALU_SUBW=5'd12, ALU_SLLW=5'd13, ALU_SRLW=5'd14, ALU_SRAW=5'd15,
               ALU_MUL=5'd16, ALU_MULH=5'd17, ALU_MULHSU=5'd18, ALU_MULHU=5'd19,
               ALU_DIV=5'd20, ALU_DIVU=5'd21, ALU_REM=5'd22, ALU_REMU=5'd23,
               ALU_MULW=5'd24, ALU_DIVW=5'd25, ALU_DIVUW=5'd26,
               ALU_REMW=5'd27, ALU_REMUW=5'd28;

    wire [31:0] a_w = a[31:0], b_w = b[31:0];
    wire signed [31:0] a_ws = a[31:0], b_ws = b[31:0];
    wire signed [63:0] a_s = a, b_s = b;

    wire [31:0] addw_full = a_w + b_w;
    wire [31:0] subw_full = a_w - b_w;
    wire [31:0] sllw_full = a_w << b[4:0];
    wire [31:0] srlw_full = a_w >> b[4:0];
    wire [31:0] sraw_full = a_ws >>> b[4:0];

    wire signed [127:0] mul_ss = a_s * b_s;
    wire        [127:0] mul_uu = a * b;
    wire signed [127:0] mul_su = a_s * $signed({1'b0, b});
    wire [31:0] mulw_full = a_w * b_w;

    wire signed [63:0] div_q_s =
        (b_s == 64'sd0) ? -64'sd1 :
        (a_s == 64'sh8000000000000000 && b_s == -64'sd1) ? 64'sh8000000000000000 :
        a_s / b_s;
    wire [63:0] div_q_u = (b == 64'd0) ? 64'hFFFFFFFFFFFFFFFF : a / b;
    wire signed [63:0] rem_r_s =
        (b_s == 64'sd0) ? a_s :
        (a_s == 64'sh8000000000000000 && b_s == -64'sd1) ? 64'sd0 :
        a_s % b_s;
    wire [63:0] rem_r_u = (b == 64'd0) ? a : a % b;

    wire signed [31:0] divw_s =
        (b_ws == 32'sd0) ? -32'sd1 :
        (a_ws == -32'sd2147483648 && b_ws == -32'sd1) ? 32'sh80000000 :
        a_ws / b_ws;
    wire [31:0] divuw_r = (b_w == 32'd0) ? 32'hFFFFFFFF : a_w / b_w;
    wire signed [31:0] remw_s =
        (b_ws == 32'sd0) ? a_ws :
        (a_ws == -32'sd2147483648 && b_ws == -32'sd1) ? 32'sd0 :
        a_ws % b_ws;
    wire [31:0] remuw_r = (b_w == 32'd0) ? a_w : a_w % b_w;

    always @(*) begin
        case (op)
            ALU_ADD: y=a+b;
            ALU_SUB: y=a-b;
            ALU_SLL: y=a<<b[5:0];
            ALU_SLT: y=($signed(a)<$signed(b))?64'd1:64'd0;
            ALU_SLTU:y=(a<b)?64'd1:64'd0;
            ALU_XOR: y=a^b;
            ALU_SRL: y=a>>b[5:0];
            ALU_SRA: y=$signed(a)>>>b[5:0];
            ALU_OR:  y=a|b;
            ALU_AND: y=a&b;
            ALU_PASS:y=b;
            ALU_ADDW:y={{32{addw_full[31]}},addw_full};
            ALU_SUBW:y={{32{subw_full[31]}},subw_full};
            ALU_SLLW:y={{32{sllw_full[31]}},sllw_full};
            ALU_SRLW:y={32'd0,srlw_full};
            ALU_SRAW:y={{32{sraw_full[31]}},sraw_full};
            ALU_MUL: y=mul_ss[63:0];
            ALU_MULH:y=mul_ss[127:64];
            ALU_MULHSU:y=mul_su[127:64];
            ALU_MULHU:y=mul_uu[127:64];
            ALU_DIV: y=div_q_s;
            ALU_DIVU:y=div_q_u;
            ALU_REM: y=rem_r_s;
            ALU_REMU:y=rem_r_u;
            ALU_MULW:y={{32{mulw_full[31]}},mulw_full};
            ALU_DIVW:y={{32{divw_s[31]}},divw_s};
            ALU_DIVUW:y={{32{divuw_r[31]}},divuw_r};
            ALU_REMW:y={{32{remw_s[31]}},remw_s};
            ALU_REMUW:y={{32{remuw_r[31]}},remuw_r};
            default: y=64'd0;
        endcase
    end
endmodule

// ---------- Regfile ----------
module rv64i_pipe_regfile (
    input  wire clk, rst,
    input  wire [4:0]  rs1, rs2, rd,
    input  wire [63:0] wdata,
    input  wire        we,
    output wire [63:0] rdata1, rdata2
);
    reg [63:0] regs [0:31];
    integer i;
    always @(posedge clk) begin
        if (rst) for (i=0;i<32;i=i+1) regs[i] <= 64'd0;
        else if (we && rd != 5'd0) regs[rd] <= wdata;
    end
    // Write-first bypass: ID read in the same cycle as WB write
    // must see the new value. Covers 3-instruction gaps that 2-level
    // EX/MEM + MEM/WB forwarding cannot reach.
    assign rdata1 = (rs1 == 5'd0) ? 64'd0 :
                    (we && rd == rs1) ? wdata : regs[rs1];
    assign rdata2 = (rs2 == 5'd0) ? 64'd0 :
                    (we && rd == rs2) ? wdata : regs[rs2];
endmodule

// ---------- Decoder ----------
module rv64i_pipe_decoder (
    input  wire [31:0] instr,
    output wire [4:0]  rs1, rs2, rd,
    output reg  [63:0] imm,
    output reg  [4:0]  alu_op,
    output reg         alu_a_pc, alu_b_imm, reg_we,
    output reg         mem_re, mem_we,
    output reg  [1:0]  mem_size,
    output reg         pc_sel_imm, pc_sel_alu,
    output reg  [2:0]  branch_op,
    output reg         is_ecall, is_ebreak, is_mret,
    output reg         is_csr,
    output reg  [2:0]  csr_op,
    output reg  [11:0] csr_addr,
    output reg         is_jump, is_branch,
    output reg         mem_unsigned,
    output reg         illegal
);
    wire [6:0] opcode=instr[6:0];
    wire [2:0] funct3=instr[14:12];
    wire [6:0] funct7=instr[31:25];
    assign rs1=instr[19:15]; assign rs2=instr[24:20]; assign rd=instr[11:7];
    wire [63:0] imm_i={{52{instr[31]}},instr[31:20]};
    wire [63:0] imm_s={{52{instr[31]}},instr[31:25],instr[11:7]};
    wire [63:0] imm_b={{51{instr[31]}},instr[31],instr[7],instr[30:25],instr[11:8],1'b0};
    wire [63:0] imm_j={{43{instr[31]}},instr[31],instr[19:12],instr[20],instr[30:21],1'b0};
    wire [63:0] imm_u={{32{instr[31]}},instr[31:12],12'b0};
    always @(*) begin
        imm=64'd0; alu_op=5'd0; alu_a_pc=0; alu_b_imm=0;
        reg_we=0; mem_re=0; mem_we=0; mem_size=2'b11;
        pc_sel_imm=0; pc_sel_alu=0; branch_op=3'b000;
        is_ecall=0; is_ebreak=0; is_mret=0;
        is_csr=0; csr_op=3'b000; csr_addr=12'h000;
        is_jump=0; is_branch=0; mem_unsigned=0; illegal=0;
        case (opcode)
            7'b0110111: begin imm=imm_u; alu_b_imm=1; alu_op=5'd10; reg_we=1; end
            7'b0010111: begin imm=imm_u; alu_a_pc=1; alu_b_imm=1; alu_op=5'd0; reg_we=1; end
            7'b1101111: begin imm=imm_j; reg_we=1; pc_sel_imm=1; is_jump=1; end
            7'b1100111: begin
                imm=imm_i; alu_b_imm=1; alu_op=5'd0; reg_we=1; pc_sel_alu=1; is_jump=1;
                if (funct3!=3'b000) illegal=1;
            end
            7'b1100011: begin
                imm=imm_b; is_branch=1;
                case (funct3)
                    3'b000: branch_op=3'b001; 3'b001: branch_op=3'b010;
                    3'b100: branch_op=3'b011; 3'b101: branch_op=3'b100;
                    3'b110: branch_op=3'b101; 3'b111: branch_op=3'b110;
                    default: illegal=1;
                endcase
            end
            7'b0000011: begin
                imm=imm_i; alu_b_imm=1; alu_op=5'd0; reg_we=1; mem_re=1;
                mem_unsigned = funct3[2];
                case (funct3)
                    3'b000,3'b100: mem_size=2'b00;
                    3'b001,3'b101: mem_size=2'b01;
                    3'b010,3'b110: mem_size=2'b10;
                    3'b011:        mem_size=2'b11;
                    default: illegal=1;
                endcase
            end
            7'b0100011: begin
                imm=imm_s; alu_b_imm=1; alu_op=5'd0; mem_we=1;
                case (funct3)
                    3'b000: mem_size=2'b00; 3'b001: mem_size=2'b01;
                    3'b010: mem_size=2'b10; 3'b011: mem_size=2'b11;
                    default: illegal=1;
                endcase
            end
            7'b0010011: begin
                imm=imm_i; alu_b_imm=1; reg_we=1;
                case (funct3)
                    3'b000: alu_op=5'd0; 3'b010: alu_op=5'd3;
                    3'b011: alu_op=5'd4; 3'b100: alu_op=5'd5;
                    3'b110: alu_op=5'd8; 3'b111: alu_op=5'd9;
                    3'b001: begin
                        if (funct7[6:1]!=6'b000000) illegal=1;
                        alu_op=5'd2; imm={58'd0,instr[25:20]};
                    end
                    3'b101: begin
                        if (funct7[6:1]==6'b000000) alu_op=5'd6;
                        else if (funct7[6:1]==6'b010000) alu_op=5'd7;
                        else illegal=1;
                        imm={58'd0,instr[25:20]};
                    end
                endcase
            end
            7'b0011011: begin
                imm=imm_i; alu_b_imm=1; reg_we=1;
                case (funct3)
                    3'b000: alu_op=5'd11;
                    3'b001: begin
                        if (funct7!=7'b0000000) illegal=1;
                        alu_op=5'd13; imm={59'd0,instr[24:20]};
                    end
                    3'b101: begin
                        if (funct7==7'b0000000) alu_op=5'd14;
                        else if (funct7==7'b0100000) alu_op=5'd15;
                        else illegal=1;
                        imm={59'd0,instr[24:20]};
                    end
                    default: illegal=1;
                endcase
            end
            7'b0110011: begin
                reg_we=1;
                case ({funct7,funct3})
                    {7'b0000000,3'b000}: alu_op=5'd0; {7'b0100000,3'b000}: alu_op=5'd1;
                    {7'b0000000,3'b001}: alu_op=5'd2; {7'b0000000,3'b010}: alu_op=5'd3;
                    {7'b0000000,3'b011}: alu_op=5'd4; {7'b0000000,3'b100}: alu_op=5'd5;
                    {7'b0000000,3'b101}: alu_op=5'd6; {7'b0100000,3'b101}: alu_op=5'd7;
                    {7'b0000000,3'b110}: alu_op=5'd8; {7'b0000000,3'b111}: alu_op=5'd9;
                    {7'b0000001,3'b000}: alu_op=5'd16; {7'b0000001,3'b001}: alu_op=5'd17;
                    {7'b0000001,3'b010}: alu_op=5'd18; {7'b0000001,3'b011}: alu_op=5'd19;
                    {7'b0000001,3'b100}: alu_op=5'd20; {7'b0000001,3'b101}: alu_op=5'd21;
                    {7'b0000001,3'b110}: alu_op=5'd22; {7'b0000001,3'b111}: alu_op=5'd23;
                    default: illegal=1;
                endcase
            end
            7'b0111011: begin
                reg_we=1;
                case ({funct7,funct3})
                    {7'b0000000,3'b000}: alu_op=5'd11; {7'b0100000,3'b000}: alu_op=5'd12;
                    {7'b0000000,3'b001}: alu_op=5'd13; {7'b0000000,3'b101}: alu_op=5'd14;
                    {7'b0100000,3'b101}: alu_op=5'd15; {7'b0000001,3'b000}: alu_op=5'd24;
                    {7'b0000001,3'b100}: alu_op=5'd25; {7'b0000001,3'b101}: alu_op=5'd26;
                    {7'b0000001,3'b110}: alu_op=5'd27; {7'b0000001,3'b111}: alu_op=5'd28;
                    default: illegal=1;
                endcase
            end
            7'b0001111: begin
            end
            7'b1110011: begin
                if (funct3 == 3'b000) begin
                    if (instr[31:20] == 12'h000 && rs1==5'd0 && rd==5'd0) is_ecall = 1'b1;
                    else if (instr[31:20] == 12'h001 && rs1==5'd0 && rd==5'd0) is_ebreak = 1'b1;
                    else if (instr[31:20] == 12'h302 && rs1==5'd0 && rd==5'd0) is_mret = 1'b1;
                    else illegal = 1'b1;
                end else if (funct3 == 3'b001 || funct3 == 3'b010 || funct3 == 3'b011
                          || funct3 == 3'b101 || funct3 == 3'b110 || funct3 == 3'b111) begin
                    is_csr = 1'b1; csr_op = funct3; csr_addr = instr[31:20]; reg_we = 1'b1;
                end else
                    illegal = 1'b1;
            end
            default: illegal=1;
        endcase
    end
endmodule

module rv64i_pipe_core (
    input  wire        clk, rst,
    output wire [63:0] imem_addr,
    input  wire [31:0] imem_data,
    output wire [63:0] dmem_addr,
    output wire [63:0] dmem_wdata,
    output wire        dmem_we, dmem_re,
    output wire [1:0]  dmem_size,
    input  wire [63:0] dmem_rdata,
    output reg         halted,
    output wire [63:0] dbg_pc
);
    reg  [63:0] pc;
    wire [31:0] if_instr = imem_data;
    assign imem_addr = pc;
    assign dbg_pc = pc;

    reg [63:0] ifid_pc; reg [31:0] ifid_instr; reg ifid_valid;

    wire [4:0] id_rs1, id_rs2, id_rd;
    wire [63:0] id_imm;
    wire [4:0]  id_alu_op;
    wire        id_alu_a_pc, id_alu_b_imm, id_reg_we, id_mem_re, id_mem_we;
    wire [1:0]  id_mem_size;
    wire        id_pc_sel_imm, id_pc_sel_alu;
    wire [2:0]  id_branch_op;
    wire        id_is_ecall, id_is_ebreak, id_is_mret, id_is_csr;
    wire [2:0]  id_csr_op;
    wire [11:0] id_csr_addr;
    wire        id_is_jump, id_is_branch, id_illegal, id_mem_unsigned;

    rv64i_pipe_decoder u_dec (
        .instr(ifid_instr),
        .rs1(id_rs1), .rs2(id_rs2), .rd(id_rd),
        .imm(id_imm), .alu_op(id_alu_op),
        .alu_a_pc(id_alu_a_pc), .alu_b_imm(id_alu_b_imm),
        .reg_we(id_reg_we), .mem_re(id_mem_re), .mem_we(id_mem_we),
        .mem_size(id_mem_size),
        .pc_sel_imm(id_pc_sel_imm), .pc_sel_alu(id_pc_sel_alu),
        .branch_op(id_branch_op),
        .is_ecall(id_is_ecall), .is_ebreak(id_is_ebreak), .is_mret(id_is_mret),
        .is_csr(id_is_csr), .csr_op(id_csr_op), .csr_addr(id_csr_addr),
        .is_jump(id_is_jump), .is_branch(id_is_branch),
        .mem_unsigned(id_mem_unsigned), .illegal(id_illegal)
    );

    wire [63:0] id_rs1_val, id_rs2_val, wb_wdata;
    wire        wb_we;
    wire [4:0]  wb_rd;

    rv64i_pipe_regfile u_rf (
        .clk(clk), .rst(rst),
        .rs1(id_rs1), .rs2(id_rs2), .rd(wb_rd),
        .wdata(wb_wdata), .we(wb_we),
        .rdata1(id_rs1_val), .rdata2(id_rs2_val)
    );

    reg [63:0] idex_pc, idex_rs1_val, idex_rs2_val, idex_imm;
    reg [4:0]  idex_rs1, idex_rs2, idex_rd, idex_alu_op;
    reg        idex_alu_a_pc, idex_alu_b_imm, idex_reg_we, idex_mem_re, idex_mem_we;
    reg [1:0]  idex_mem_size;
    reg        idex_pc_sel_imm, idex_pc_sel_alu;
    reg [2:0]  idex_branch_op;
    reg        idex_is_jump, idex_is_branch, idex_mem_unsigned, idex_illegal;
    reg        idex_is_ecall, idex_is_ebreak, idex_is_mret, idex_is_csr;
    reg [2:0]  idex_csr_op;
    reg [11:0] idex_csr_addr;
    reg [4:0]  idex_csr_zimm;

    reg [4:0]  exmem_rd;
    reg        exmem_reg_we;
    reg [63:0] exmem_alu_y;
    reg [4:0]  memwb_rd;
    reg        memwb_reg_we;

    wire fwd_a_exmem = exmem_reg_we && (exmem_rd != 5'd0) && (exmem_rd == idex_rs1);
    wire fwd_b_exmem = exmem_reg_we && (exmem_rd != 5'd0) && (exmem_rd == idex_rs2);
    wire fwd_a_memwb = memwb_reg_we && (memwb_rd != 5'd0) && (memwb_rd == idex_rs1) && !fwd_a_exmem;
    wire fwd_b_memwb = memwb_reg_we && (memwb_rd != 5'd0) && (memwb_rd == idex_rs2) && !fwd_b_exmem;

    wire [63:0] ex_rs1 = fwd_a_exmem ? exmem_alu_y : fwd_a_memwb ? wb_wdata : idex_rs1_val;
    wire [63:0] ex_rs2 = fwd_b_exmem ? exmem_alu_y : fwd_b_memwb ? wb_wdata : idex_rs2_val;

    wire [63:0] ex_alu_a = idex_alu_a_pc ? idex_pc : ex_rs1;
    wire [63:0] ex_alu_b = idex_alu_b_imm ? idex_imm : ex_rs2;
    wire [63:0] ex_alu_y;
    rv64i_pipe_alu u_alu (.a(ex_alu_a), .b(ex_alu_b), .op(idex_alu_op), .y(ex_alu_y));

    reg ex_branch_taken;
    always @(*) begin
        case (idex_branch_op)
            3'b001: ex_branch_taken = (ex_rs1 == ex_rs2);
            3'b010: ex_branch_taken = (ex_rs1 != ex_rs2);
            3'b011: ex_branch_taken = ($signed(ex_rs1) <  $signed(ex_rs2));
            3'b100: ex_branch_taken = ($signed(ex_rs1) >= $signed(ex_rs2));
            3'b101: ex_branch_taken = (ex_rs1 <  ex_rs2);
            3'b110: ex_branch_taken = (ex_rs1 >= ex_rs2);
            default:ex_branch_taken = 1'b0;
        endcase
    end

    wire [63:0] ex_jalr_target = (ex_rs1 + idex_imm) & ~64'd1;
    wire [63:0] ex_branch_target = idex_pc + idex_imm;

    wire [63:0] ex_csr_src = idex_csr_op[2] ? {59'd0, idex_csr_zimm} : ex_rs1;
    wire [63:0] ex_csr_rdata;
    wire        ex_csr_illegal;
    wire [63:0] ex_mtvec, ex_mepc;

    wire ex_trap_illegal = idex_illegal || ex_csr_illegal;
    wire ex_trap_take    = ex_trap_illegal || idex_is_ecall || idex_is_ebreak;
    wire ex_mret_take    = idex_is_mret;

    reg [63:0] ex_trap_cause;
    always @(*) begin
        if (ex_trap_illegal)     ex_trap_cause = 64'd2;
        else if (idex_is_ebreak) ex_trap_cause = 64'd3;
        else if (idex_is_ecall)  ex_trap_cause = 64'd11;
        else                     ex_trap_cause = 64'd0;
    end

    rv64i_csr u_csr (
        .clk(clk), .rst(rst),
        .csr_active(idex_is_csr),
        .csr_addr(idex_csr_addr),
        .csr_op(idex_csr_op),
        .csr_src(ex_csr_src),
        .csr_rdata(ex_csr_rdata),
        .csr_instr_illegal(ex_csr_illegal),
        .trap_take(ex_trap_take),
        .trap_pc(idex_pc),
        .trap_cause(ex_trap_cause),
        .trap_tval(64'd0),
        .mret(ex_mret_take),
        .mtvec_out(ex_mtvec),
        .mepc_out(ex_mepc),
        .mstatus_mie()
    );

    wire [63:0] ex_mtvec_base = {ex_mtvec[63:2], 2'b00};
    wire ex_redirect = ex_trap_take || ex_mret_take
                       || idex_is_jump || (idex_is_branch && ex_branch_taken);
    wire [63:0] ex_redirect_target =
        ex_trap_take    ? ex_mtvec_base  :
        ex_mret_take    ? ex_mepc        :
        idex_pc_sel_alu ? ex_jalr_target :
                          ex_branch_target;

    reg [63:0] exmem_pc, exmem_rs2_val, exmem_imm;
    reg        exmem_mem_re, exmem_mem_we;
    reg [1:0]  exmem_mem_size;
    reg        exmem_is_jump, exmem_mem_unsigned, exmem_is_csr;
    reg [63:0] exmem_csr_rdata;

    assign dmem_addr  = exmem_alu_y;
    assign dmem_wdata = exmem_rs2_val;
    assign dmem_we    = exmem_mem_we;
    assign dmem_re    = exmem_mem_re;
    assign dmem_size  = exmem_mem_size;

    reg [63:0] memwb_alu_y, memwb_mem_data;
    reg        memwb_mem_re;
    reg [1:0]  memwb_mem_size;
    reg        memwb_is_jump, memwb_mem_unsigned, memwb_is_csr;
    reg [63:0] memwb_csr_rdata;

    reg [63:0] wb_load_ext;
    always @(*) begin
        case (memwb_mem_size)
            2'b00: wb_load_ext = memwb_mem_unsigned ? {56'd0, memwb_mem_data[7:0]}
                                                    : {{56{memwb_mem_data[7]}}, memwb_mem_data[7:0]};
            2'b01: wb_load_ext = memwb_mem_unsigned ? {48'd0, memwb_mem_data[15:0]}
                                                    : {{48{memwb_mem_data[15]}}, memwb_mem_data[15:0]};
            2'b10: wb_load_ext = memwb_mem_unsigned ? {32'd0, memwb_mem_data[31:0]}
                                                    : {{32{memwb_mem_data[31]}}, memwb_mem_data[31:0]};
            default: wb_load_ext = memwb_mem_data;
        endcase
    end

    assign wb_wdata = memwb_is_csr ? memwb_csr_rdata :
                      memwb_mem_re ? wb_load_ext : memwb_alu_y;
    assign wb_we = memwb_reg_we;
    assign wb_rd = memwb_rd;

    wire load_use_hazard =
        idex_mem_re && (idex_rd != 5'd0) && ifid_valid &&
        ((idex_rd == id_rs1) || (idex_rd == id_rs2));
    wire stall = load_use_hazard;
    wire flush = ex_redirect;

    always @(posedge clk) begin
        if (rst) begin
            pc <= 64'h0000_0000_8000_0000;
            ifid_pc <= 64'd0; ifid_instr <= 32'd0; ifid_valid <= 1'b0;
            idex_reg_we<=1'b0; idex_mem_re<=1'b0; idex_mem_we<=1'b0;
            idex_is_jump<=1'b0; idex_is_branch<=1'b0; idex_is_csr<=1'b0;
            idex_is_ecall<=1'b0; idex_is_ebreak<=1'b0; idex_is_mret<=1'b0;
            idex_illegal<=1'b0;
            exmem_reg_we<=1'b0; exmem_mem_we<=1'b0; exmem_mem_re<=1'b0;
            exmem_is_csr<=1'b0; exmem_is_jump<=1'b0;
            memwb_reg_we<=1'b0; memwb_mem_re<=1'b0;
            memwb_is_csr<=1'b0; memwb_is_jump<=1'b0;
            halted <= 1'b0;
        end else begin
            memwb_reg_we <= exmem_reg_we; memwb_rd <= exmem_rd;
            memwb_alu_y <= exmem_alu_y; memwb_mem_data <= dmem_rdata;
            memwb_mem_re <= exmem_mem_re; memwb_mem_size <= exmem_mem_size;
            memwb_is_jump <= exmem_is_jump; memwb_mem_unsigned <= exmem_mem_unsigned;
            memwb_is_csr <= exmem_is_csr; memwb_csr_rdata <= exmem_csr_rdata;

            exmem_reg_we <= idex_reg_we && !ex_trap_take;
            exmem_rd <= idex_rd;
            exmem_alu_y <= idex_is_jump ? (idex_pc + 64'd4) :
                           idex_is_csr  ? ex_csr_rdata :
                                          ex_alu_y;
            exmem_rs2_val <= ex_rs2; exmem_imm <= idex_imm;
            exmem_mem_re <= idex_mem_re && !ex_trap_take;
            exmem_mem_we <= idex_mem_we && !ex_trap_take;
            exmem_mem_size <= idex_mem_size;
            exmem_is_jump <= idex_is_jump;
            exmem_mem_unsigned <= idex_mem_unsigned;
            exmem_is_csr <= idex_is_csr && !ex_trap_take;
            exmem_csr_rdata <= ex_csr_rdata;

            if (flush || stall) begin
                idex_reg_we<=1'b0; idex_mem_re<=1'b0; idex_mem_we<=1'b0;
                idex_is_jump<=1'b0; idex_is_branch<=1'b0; idex_is_csr<=1'b0;
                idex_is_ecall<=1'b0; idex_is_ebreak<=1'b0; idex_is_mret<=1'b0;
                idex_illegal<=1'b0;
            end else begin
                idex_pc <= ifid_pc; idex_rs1_val <= id_rs1_val; idex_rs2_val <= id_rs2_val;
                idex_imm <= id_imm; idex_rs1 <= id_rs1; idex_rs2 <= id_rs2; idex_rd <= id_rd;
                idex_alu_op <= id_alu_op; idex_alu_a_pc <= id_alu_a_pc; idex_alu_b_imm <= id_alu_b_imm;
                idex_reg_we <= id_reg_we && ifid_valid;
                idex_mem_re <= id_mem_re && ifid_valid;
                idex_mem_we <= id_mem_we && ifid_valid;
                idex_mem_size <= id_mem_size;
                idex_pc_sel_imm <= id_pc_sel_imm; idex_pc_sel_alu <= id_pc_sel_alu;
                idex_branch_op <= id_branch_op;
                idex_is_jump <= id_is_jump && ifid_valid;
                idex_is_branch <= id_is_branch && ifid_valid;
                idex_mem_unsigned <= id_mem_unsigned;
                idex_illegal <= id_illegal && ifid_valid;
                idex_is_ecall <= id_is_ecall && ifid_valid;
                idex_is_ebreak <= id_is_ebreak && ifid_valid;
                idex_is_mret <= id_is_mret && ifid_valid;
                idex_is_csr <= id_is_csr && ifid_valid;
                idex_csr_op <= id_csr_op;
                idex_csr_addr <= id_csr_addr;
                idex_csr_zimm <= ifid_instr[19:15];
            end

            if (stall) begin
            end else if (flush) begin
                ifid_valid <= 1'b0;
            end else begin
                ifid_pc <= pc; ifid_instr <= if_instr; ifid_valid <= 1'b1;
            end

            if (ex_redirect) begin
                pc <= ex_redirect_target;
            end else if (!stall) begin
                pc <= pc + 64'd4;
            end
        end
    end
endmodule

