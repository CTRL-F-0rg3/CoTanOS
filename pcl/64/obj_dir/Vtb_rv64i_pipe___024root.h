// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtb_rv64i_pipe.h for the primary calling header

#ifndef VERILATED_VTB_RV64I_PIPE___024ROOT_H_
#define VERILATED_VTB_RV64I_PIPE___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtb_rv64i_pipe__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtb_rv64i_pipe___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ tb_rv64i_pipe__DOT__clk;
        CData/*0:0*/ tb_rv64i_pipe__DOT__rst;
        CData/*0:0*/ tb_rv64i_pipe__DOT__halted;
        CData/*0:0*/ tb_rv64i_pipe__DOT__tohost_we;
        CData/*0:0*/ tb_rv64i_pipe__DOT__pass;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid;
        CData/*4:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1;
        CData/*4:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2;
        CData/*4:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_rd;
        CData/*4:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_a_pc;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_b_imm;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_reg_we;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_re;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_we;
        CData/*1:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_size;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_pc_sel_alu;
        CData/*2:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_is_halt;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_is_jump;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_is_branch;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_unsigned;
        CData/*4:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_reg_we;
        CData/*4:0*/ tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__memwb_reg_we;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__ex_redirect;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_re;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_we;
        CData/*1:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_is_jump;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_unsigned;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_re;
        CData/*1:0*/ tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_size;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__memwb_is_jump;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_unsigned;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard;
        CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__flush;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_1;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_6;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_8;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_11;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_12;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_17;
        CData/*1:0*/ __VdfgRegularize_h6e95ff9d_0_20;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_21;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_22;
        CData/*4:0*/ __VdfgRegularize_h6e95ff9d_0_23;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_31;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_34;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_35;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_36;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_37;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_38;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tb_rv64i_pipe__DOT__clk__0;
        SData/*9:0*/ __VdfgRegularize_h6e95ff9d_0_0;
        IData/*31:0*/ tb_rv64i_pipe__DOT__cycles;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__addw_full;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__subw_full;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sllw_full;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sraw_full;
        VlWide<4>/*127:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mul_ss;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mulw_full;
    };
    struct {
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divw_s;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divuw_r;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remw_s;
        IData/*31:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remuw_r;
        VlWide<32>/*1023:0*/ tb_rv64i_pipe__DOT__u_mem__DOT__hexpath;
        VlWide<4>/*127:0*/ __VdfgRegularize_h6e95ff9d_0_16;
        IData/*31:0*/ __Vi;
        QData/*63:0*/ tb_rv64i_pipe__DOT__tohost_data;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__pc;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__wb_wdata;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_pc;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1_val;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2_val;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__idex_imm;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__memwb_alu_y;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s;
        QData/*63:0*/ tb_rv64i_pipe__DOT__u_core__DOT__u_dec__DOT__imm_i;
        QData/*63:0*/ __VdfgRegularize_h6e95ff9d_0_25;
        QData/*63:0*/ __VdfgRegularize_h6e95ff9d_0_26;
        VlUnpacked<QData/*63:0*/, 32> tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs;
        VlUnpacked<IData/*31:0*/, 32768> tb_rv64i_pipe__DOT__u_mem__DOT__imem;
        VlUnpacked<QData/*63:0*/, 32768> tb_rv64i_pipe__DOT__u_mem__DOT__dmem;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VicoTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h3f920df9__0;

    // INTERNAL VARIABLES
    Vtb_rv64i_pipe__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtb_rv64i_pipe___024root(Vtb_rv64i_pipe__Syms* symsp, const char* namep);
    ~Vtb_rv64i_pipe___024root();
    VL_UNCOPYABLE(Vtb_rv64i_pipe___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
