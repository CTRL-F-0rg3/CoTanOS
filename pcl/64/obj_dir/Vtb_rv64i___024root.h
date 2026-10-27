// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtb_rv64i.h for the primary calling header

#ifndef VERILATED_VTB_RV64I___024ROOT_H_
#define VERILATED_VTB_RV64I___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtb_rv64i__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtb_rv64i___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ tb_rv64i__DOT__clk;
    CData/*0:0*/ tb_rv64i__DOT__rst;
    CData/*0:0*/ tb_rv64i__DOT__halted;
    CData/*0:0*/ tb_rv64i__DOT__tohost_we;
    CData/*0:0*/ tb_rv64i__DOT__pass;
    CData/*4:0*/ tb_rv64i__DOT__u_core__DOT__rd;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__mem_we;
    CData/*1:0*/ tb_rv64i__DOT__u_core__DOT__mem_size;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__pc_sel_imm;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__pc_sel_alu;
    CData/*2:0*/ tb_rv64i__DOT__u_core__DOT__branch_op;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_1;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_2;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_3;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_4;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_6;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_8;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_9;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_12;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_13;
    CData/*2:0*/ __VdfgRegularize_h6e95ff9d_0_16;
    CData/*6:0*/ __VdfgRegularize_h6e95ff9d_0_17;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_25;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_28;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_31;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_32;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_33;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_34;
    CData/*1:0*/ __VdfgRegularize_h6e95ff9d_0_35;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_36;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_37;
    CData/*1:0*/ __VdfgRegularize_h6e95ff9d_0_38;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_42;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_43;
    CData/*5:0*/ __VdfgRegularize_h6e95ff9d_0_44;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_45;
    CData/*0:0*/ __Vtrigprevexpr___TOP__tb_rv64i__DOT__clk__0;
    SData/*9:0*/ __VdfgRegularize_h6e95ff9d_0_5;
    SData/*11:0*/ __VdfgRegularize_h6e95ff9d_0_21;
    IData/*31:0*/ tb_rv64i__DOT__cycles;
    VlWide<32>/*1023:0*/ tb_rv64i__DOT__u_mem__DOT__hexpath;
    IData/*31:0*/ __Vi;
    QData/*63:0*/ tb_rv64i__DOT__dmem_rdata;
    QData/*63:0*/ tb_rv64i__DOT__tohost_data;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__pc;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__imm;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__rs1_val;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__rs2_val;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__alu_y;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__pc_plus4;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__pc_plus_imm;
    VlUnpacked<QData/*63:0*/, 32> tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs;
    VlUnpacked<IData/*31:0*/, 32768> tb_rv64i__DOT__u_mem__DOT__imem;
    VlUnpacked<QData/*63:0*/, 32768> tb_rv64i__DOT__u_mem__DOT__dmem;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VicoTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h0c2173a0__0;

    // INTERNAL VARIABLES
    Vtb_rv64i__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtb_rv64i___024root(Vtb_rv64i__Syms* symsp, const char* namep);
    ~Vtb_rv64i___024root();
    VL_UNCOPYABLE(Vtb_rv64i___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
