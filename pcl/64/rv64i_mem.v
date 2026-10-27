module rv64i_mem #(
    parameter MEM_WORDS = 32768,
    parameter BASE      = 64'h0000_0000_8000_0000
)(
    input  wire        clk,
    input  wire [63:0] iaddr,
    output wire [31:0] idata,
    input  wire [63:0] daddr,
    input  wire [63:0] dwdata,
    input  wire        dwe,
    input  wire        dre,
    input  wire [1:0]  dsize,
    output wire [63:0] drdata,
    output reg         tohost_we,
    output reg  [63:0] tohost_data
);
    reg [31:0] imem [0:MEM_WORDS-1];
    reg [63:0] dmem [0:MEM_WORDS-1];
    reg [1023:0] hexpath;

    initial begin
        if ($value$plusargs("HEX=%s", hexpath)) begin
            $readmemh(hexpath, imem);
        end
    end

    wire [63:0] iw_full = (iaddr - BASE) >> 2;
    wire [63:0] dw_full = (daddr - BASE) >> 3;
    wire [31:0] iw      = iw_full[31:0];
    wire [31:0] dw      = dw_full[31:0];
    wire [2:0]  dbo     = daddr[2:0];
    wire [63:0] word    = dmem[dw];

    assign idata = imem[iw];

    assign drdata = (dsize == 2'b11) ? word :
                    (dsize == 2'b10) ? (dbo[2] ? {32'd0, word[63:32]} : {32'd0, word[31:0]}) :
                    (dsize == 2'b01) ? (dbo[2:1] == 2'd0 ? {48'd0, word[15:0]}  :
                                        dbo[2:1] == 2'd1 ? {48'd0, word[31:16]} :
                                        dbo[2:1] == 2'd2 ? {48'd0, word[47:32]} :
                                                           {48'd0, word[63:48]}) :
                                        (dbo == 3'd0 ? {56'd0, word[7:0]}   :
                                         dbo == 3'd1 ? {56'd0, word[15:8]}  :
                                         dbo == 3'd2 ? {56'd0, word[23:16]} :
                                         dbo == 3'd3 ? {56'd0, word[31:24]} :
                                         dbo == 3'd4 ? {56'd0, word[39:32]} :
                                         dbo == 3'd5 ? {56'd0, word[47:40]} :
                                         dbo == 3'd6 ? {56'd0, word[55:48]} :
                                                       {56'd0, word[63:56]});

    always @(posedge clk) begin
        tohost_we <= 1'b0;
        if (dwe) begin
            case (dsize)
                2'b00: case (dbo)
                    3'd0: dmem[dw][7:0]   <= dwdata[7:0];
                    3'd1: dmem[dw][15:8]  <= dwdata[7:0];
                    3'd2: dmem[dw][23:16] <= dwdata[7:0];
                    3'd3: dmem[dw][31:24] <= dwdata[7:0];
                    3'd4: dmem[dw][39:32] <= dwdata[7:0];
                    3'd5: dmem[dw][47:40] <= dwdata[7:0];
                    3'd6: dmem[dw][55:48] <= dwdata[7:0];
                    3'd7: dmem[dw][63:56] <= dwdata[7:0];
                endcase
                2'b01: case (dbo[2:1])
                    2'd0: dmem[dw][15:0]  <= dwdata[15:0];
                    2'd1: dmem[dw][31:16] <= dwdata[15:0];
                    2'd2: dmem[dw][47:32] <= dwdata[15:0];
                    2'd3: dmem[dw][63:48] <= dwdata[15:0];
                endcase
                2'b10: if (dbo[2] == 0) dmem[dw][31:0]  <= dwdata[31:0];
                       else             dmem[dw][63:32] <= dwdata[31:0];
                2'b11: dmem[dw] <= dwdata;
            endcase
            if (daddr == 64'h0000_0000_8000_1000) begin
                tohost_we   <= 1'b1;
                tohost_data <= dwdata;
            end
        end
    end
endmodule