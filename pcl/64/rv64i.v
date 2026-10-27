// rv64i.v - RV64I single-cycle core

// ---------- ALU ----------
module rv64i_alu (
    input  wire [63:0] a,
    input  wire [63:0] b,
    input  wire [4:0]  op,
    output reg  [63:0] y
);
    localparam ALU_ADD  = 5'd0;
    localparam ALU_SUB  = 5'd1;
    localparam ALU_SLL  = 5'd2;
    localparam ALU_SLT  = 5'd3;
    localparam ALU_SLTU = 5'd4;
    localparam ALU_XOR  = 5'd5;
    localparam ALU_SRL  = 5'd6;
    localparam ALU_SRA  = 5'd7;
    localparam ALU_OR   = 5'd8;
    localparam ALU_AND  = 5'd9;
    localparam ALU_PASS = 5'd10;
    localparam ALU_ADDW = 5'd11;
    localparam ALU_SUBW = 5'd12;
    localparam ALU_SLLW = 5'd13;
    localparam ALU_SRLW = 5'd14;
    localparam ALU_SRAW = 5'd15;

    // word ops intermediates (no bit-select on parenthesized expr)
    wire [31:0] a_w = a[31:0];
    wire [31:0] b_w = b[31:0];

    wire [31:0] addw_full = a_w + b_w;
    wire [31:0] subw_full = a_w - b_w;
    wire [31:0] sllw_full = a_w << b[4:0];
    wire [31:0] srlw_full = a_w >> b[4:0];
    wire [31:0] sraw_full = $signed(a_w) >>> b[4:0];

    always @(*) begin
        case (op)
            ALU_ADD:  y = a + b;
            ALU_SUB:  y = a - b;
            ALU_SLL:  y = a << b[5:0];
            ALU_SLT:  y = ($signed(a) < $signed(b)) ? 64'd1 : 64'd0;
            ALU_SLTU: y = (a < b) ? 64'd1 : 64'd0;
            ALU_XOR:  y = a ^ b;
            ALU_SRL:  y = a >> b[5:0];
            ALU_SRA:  y = $signed(a) >>> b[5:0];
            ALU_OR:   y = a | b;
            ALU_AND:  y = a & b;
            ALU_PASS: y = b;
            ALU_ADDW: y = {{32{addw_full[31]}}, addw_full};
            ALU_SUBW: y = {{32{subw_full[31]}}, subw_full};
            ALU_SLLW: y = {{32{sllw_full[31]}}, sllw_full};
            ALU_SRLW: y = {32'd0, srlw_full};
            ALU_SRAW: y = {{32{sraw_full[31]}}, sraw_full};
            default:  y = 64'd0;
        endcase
    end
endmodule

// ---------- Register file ----------
module rv64i_regfile (
    input  wire        clk,
    input  wire        rst,
    input  wire [4:0]  rs1,
    input  wire [4:0]  rs2,
    input  wire [4:0]  rd,
    input  wire [63:0] wdata,
    input  wire        we,
    output wire [63:0] rdata1,
    output wire [63:0] rdata2
);
    reg [63:0] regs [0:31];
    integer i;
    always @(posedge clk) begin
        if (rst) begin
            for (i = 0; i < 32; i = i + 1) regs[i] <= 64'd0;
        end else if (we && rd != 5'd0) begin
            regs[rd] <= wdata;
        end
    end
    assign rdata1 = (rs1 == 5'd0) ? 64'd0 : regs[rs1];
    assign rdata2 = (rs2 == 5'd0) ? 64'd0 : regs[rs2];
endmodule

// ---------- Decoder ----------
module rv64i_decoder (
    input  wire [31:0] instr,
    output wire [4:0]  rs1,
    output wire [4:0]  rs2,
    output wire [4:0]  rd,
    output reg  [63:0] imm,
    output reg  [4:0]  alu_op,
    output reg         alu_a_pc,
    output reg         alu_b_imm,
    output reg         reg_we,
    output reg         mem_re,
    output reg         mem_we,
    output reg  [1:0]  mem_size,
    output reg         pc_sel_imm,
    output reg         pc_sel_alu,
    output reg  [2:0]  branch_op,
    output reg         is_ebreak,
    output reg         is_ecall,
    output reg         illegal
);
    wire [6:0] opcode = instr[6:0];
    wire [2:0] funct3 = instr[14:12];
    wire [6:0] funct7 = instr[31:25];

    assign rs1 = instr[19:15];
    assign rs2 = instr[24:20];
    assign rd  = instr[11:7];

    wire [63:0] imm_i = {{52{instr[31]}}, instr[31:20]};
    wire [63:0] imm_s = {{52{instr[31]}}, instr[31:25], instr[11:7]};
    wire [63:0] imm_b = {{51{instr[31]}}, instr[31], instr[7],
                         instr[30:25], instr[11:8], 1'b0};
    wire [63:0] imm_j = {{43{instr[31]}}, instr[31], instr[19:12],
                         instr[20], instr[30:21], 1'b0};
    wire [63:0] imm_u = {{32{instr[31]}}, instr[31:12], 12'b0};

    always @(*) begin
        imm        = 64'd0;
        alu_op     = 5'd0;
        alu_a_pc   = 1'b0;
        alu_b_imm  = 1'b0;
        reg_we     = 1'b0;
        mem_re     = 1'b0;
        mem_we     = 1'b0;
        mem_size   = 2'b11;
        pc_sel_imm = 1'b0;
        pc_sel_alu = 1'b0;
        branch_op  = 3'b000;
        is_ebreak  = 1'b0;
        is_ecall   = 1'b0;
        illegal    = 1'b0;

        case (opcode)
            7'b0110111: begin // LUI
                imm      = imm_u;
                alu_b_imm= 1'b1;
                alu_op   = 5'd10;
                reg_we   = 1'b1;
            end
            7'b0010111: begin // AUIPC
                imm      = imm_u;
                alu_a_pc = 1'b1;
                alu_b_imm= 1'b1;
                alu_op   = 5'd0;
                reg_we   = 1'b1;
            end
            7'b1101111: begin // JAL
                imm       = imm_j;
                reg_we    = 1'b1;
                pc_sel_imm= 1'b1;
            end
            7'b1100111: begin // JALR
                imm       = imm_i;
                alu_b_imm = 1'b1;
                alu_op    = 5'd0;
                reg_we    = 1'b1;
                pc_sel_alu= 1'b1;
                if (funct3 != 3'b000) illegal = 1'b1;
            end
            7'b1100011: begin // BRANCH
                imm = imm_b;
                case (funct3)
                    3'b000: branch_op = 3'b001;
                    3'b001: branch_op = 3'b010;
                    3'b100: branch_op = 3'b011;
                    3'b101: branch_op = 3'b100;
                    3'b110: branch_op = 3'b101;
                    3'b111: branch_op = 3'b110;
                    default: illegal = 1'b1;
                endcase
            end
            7'b0000011: begin // LOAD
                imm      = imm_i;
                alu_b_imm= 1'b1;
                alu_op   = 5'd0;
                reg_we   = 1'b1;
                mem_re   = 1'b1;
                case (funct3)
                    3'b000, 3'b100: mem_size = 2'b00; // LB/LBU
                    3'b001, 3'b101: mem_size = 2'b01; // LH/LHU
                    3'b010, 3'b110: mem_size = 2'b10; // LW/LWU
                    3'b011:         mem_size = 2'b11; // LD
                    default: illegal = 1'b1;
                endcase
            end
            7'b0100011: begin // STORE
                imm      = imm_s;
                alu_b_imm= 1'b1;
                alu_op   = 5'd0;
                mem_we   = 1'b1;
                case (funct3)
                    3'b000: mem_size = 2'b00; // SB
                    3'b001: mem_size = 2'b01; // SH
                    3'b010: mem_size = 2'b10; // SW
                    3'b011: mem_size = 2'b11; // SD
                    default: illegal = 1'b1;
                endcase
            end
            7'b0010011: begin // OP-IMM
                imm      = imm_i;
                alu_b_imm= 1'b1;
                reg_we   = 1'b1;
                case (funct3)
                    3'b000: alu_op = 5'd0;  // ADDI
                    3'b010: alu_op = 5'd3;  // SLTI
                    3'b011: alu_op = 5'd4;  // SLTIU
                    3'b100: alu_op = 5'd5;  // XORI
                    3'b110: alu_op = 5'd8;  // ORI
                    3'b111: alu_op = 5'd9;  // ANDI
                    3'b001: begin           // SLLI
                        if (funct7[6:1] != 6'b000000) illegal = 1'b1;
                        alu_op = 5'd2;
                        imm    = {58'd0, instr[25:20]};
                    end
                    3'b101: begin           // SRLI / SRAI
                        if (funct7[6:1] == 6'b000000)      alu_op = 5'd6;
                        else if (funct7[6:1] == 6'b010000) alu_op = 5'd7;
                        else illegal = 1'b1;
                        imm = {58'd0, instr[25:20]};
                    end
                endcase
            end
            7'b0011011: begin // OP-IMM-32
                imm      = imm_i;
                alu_b_imm= 1'b1;
                reg_we   = 1'b1;
                case (funct3)
                    3'b000: alu_op = 5'd11; // ADDIW
                    3'b001: begin           // SLLIW
                        if (funct7 != 7'b0000000) illegal = 1'b1;
                        alu_op = 5'd13;
                        imm    = {59'd0, instr[24:20]};
                    end
                    3'b101: begin           // SRLIW / SRAIW
                        if (funct7 == 7'b0000000)      alu_op = 5'd14;
                        else if (funct7 == 7'b0100000) alu_op = 5'd15;
                        else illegal = 1'b1;
                        imm = {59'd0, instr[24:20]};
                    end
                    default: illegal = 1'b1;
                endcase
            end
            7'b0110011: begin // OP
                reg_we = 1'b1;
                case ({funct7, funct3})
                    {7'b0000000, 3'b000}: alu_op = 5'd0;
                    {7'b0100000, 3'b000}: alu_op = 5'd1;
                    {7'b0000000, 3'b001}: alu_op = 5'd2;
                    {7'b0000000, 3'b010}: alu_op = 5'd3;
                    {7'b0000000, 3'b011}: alu_op = 5'd4;
                    {7'b0000000, 3'b100}: alu_op = 5'd5;
                    {7'b0000000, 3'b101}: alu_op = 5'd6;
                    {7'b0100000, 3'b101}: alu_op = 5'd7;
                    {7'b0000000, 3'b110}: alu_op = 5'd8;
                    {7'b0000000, 3'b111}: alu_op = 5'd9;
                    default: illegal = 1'b1;
                endcase
            end
            7'b0111011: begin // OP-32
                reg_we = 1'b1;
                case ({funct7, funct3})
                    {7'b0000000, 3'b000}: alu_op = 5'd11;
                    {7'b0100000, 3'b000}: alu_op = 5'd12;
                    {7'b0000000, 3'b001}: alu_op = 5'd13;
                    {7'b0000000, 3'b101}: alu_op = 5'd14;
                    {7'b0100000, 3'b101}: alu_op = 5'd15;
                    default: illegal = 1'b1;
                endcase
            end
            7'b0001111: begin // FENCE
            end
            7'b1110011: begin // SYSTEM
                if (instr[31:20] == 12'h001)      is_ebreak = 1'b1;
                else if (instr[31:20] == 12'h000) is_ecall  = 1'b1;
                else illegal = 1'b1;
            end
            default: illegal = 1'b1;
        endcase
    end
endmodule

// ---------- Core ----------
module rv64i_core (
    input  wire        clk,
    input  wire        rst,
    output wire [63:0] imem_addr,
    input  wire [31:0] imem_data,
    output wire [63:0] dmem_addr,
    output wire [63:0] dmem_wdata,
    output wire        dmem_we,
    output wire        dmem_re,
    output wire [1:0]  dmem_size,
    input  wire [63:0] dmem_rdata,
    output reg         halted
);
    reg  [63:0] pc;
    wire [31:0] instr = imem_data;   // kombinacyjnie, nie rejestr

    assign imem_addr = pc;

    wire [4:0]  rs1, rs2, rd;
    wire [63:0] imm;
    wire [4:0]  alu_op;
    wire        alu_a_pc, alu_b_imm, reg_we, mem_re, mem_we;
    wire [1:0]  mem_size;
    wire        pc_sel_imm, pc_sel_alu;
    wire [2:0]  branch_op;
    wire        is_ebreak, is_ecall, illegal;

    rv64i_decoder u_dec (
        .instr(instr),
        .rs1(rs1), .rs2(rs2), .rd(rd),
        .imm(imm), .alu_op(alu_op),
        .alu_a_pc(alu_a_pc), .alu_b_imm(alu_b_imm),
        .reg_we(reg_we), .mem_re(mem_re), .mem_we(mem_we),
        .mem_size(mem_size),
        .pc_sel_imm(pc_sel_imm), .pc_sel_alu(pc_sel_alu),
        .branch_op(branch_op),
        .is_ebreak(is_ebreak), .is_ecall(is_ecall),
        .illegal(illegal)
    );

    wire [63:0] rs1_val, rs2_val, wb_data;
    reg  [63:0] alu_y;

    rv64i_regfile u_rf (
        .clk(clk), .rst(rst),
        .rs1(rs1), .rs2(rs2), .rd(rd),
        .wdata(wb_data), .we(reg_we),
        .rdata1(rs1_val), .rdata2(rs2_val)
    );

    wire [63:0] alu_a = alu_a_pc  ? pc  : rs1_val;
    wire [63:0] alu_b = alu_b_imm ? imm : rs2_val;
    rv64i_alu u_alu (.a(alu_a), .b(alu_b), .op(alu_op), .y(alu_y));

    reg branch_taken;
    always @(*) begin
        case (branch_op)
            3'b001: branch_taken = (rs1_val == rs2_val);
            3'b010: branch_taken = (rs1_val != rs2_val);
            3'b011: branch_taken = ($signed(rs1_val) <  $signed(rs2_val));
            3'b100: branch_taken = ($signed(rs1_val) >= $signed(rs2_val));
            3'b101: branch_taken = (rs1_val <  rs2_val);
            3'b110: branch_taken = (rs1_val >= rs2_val);
            default: branch_taken = 1'b0;
        endcase
    end

    wire [63:0] pc_plus4    = pc + 64'd4;
    wire [63:0] pc_plus_imm = pc + imm;
    wire [63:0] jalr_target = (rs1_val + imm) & ~64'd1;
    wire [63:0] next_pc     = pc_sel_alu  ? jalr_target :
                              pc_sel_imm  ? pc_plus_imm :
                              branch_taken? pc_plus_imm :
                                            pc_plus4;

    reg [63:0] load_ext;
    always @(*) begin
        case (mem_size)
            2'b00: load_ext = instr[14] ? {56'd0, dmem_rdata[7:0]}
                                        : {{56{dmem_rdata[7]}}, dmem_rdata[7:0]};
            2'b01: load_ext = instr[14] ? {48'd0, dmem_rdata[15:0]}
                                        : {{48{dmem_rdata[15]}}, dmem_rdata[15:0]};
            2'b10: load_ext = instr[14] ? {32'd0, dmem_rdata[31:0]}
                                        : {{32{dmem_rdata[31]}}, dmem_rdata[31:0]};
            default: load_ext = dmem_rdata;
        endcase
    end

    assign wb_data = (pc_sel_imm || pc_sel_alu) ? pc_plus4 :
                     mem_re ? load_ext : alu_y;

    assign dmem_addr  = alu_y;
    assign dmem_wdata = rs2_val;
    assign dmem_we    = mem_we;
    assign dmem_re    = mem_re;
    assign dmem_size  = mem_size;

    always @(posedge clk) begin
        if (rst) begin
            pc     <= 64'h0000_0000_8000_0000;
            halted <= 1'b0;
        end else if (!halted) begin
            if (is_ebreak || is_ecall || illegal) begin
                halted <= 1'b1;
            end else begin
                pc <= next_pc;
            end
        end
    end
endmodule
