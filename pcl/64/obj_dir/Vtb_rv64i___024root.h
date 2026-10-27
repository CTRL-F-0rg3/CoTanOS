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
    CData/*0:0*/ tb_rv64i__DOT__dmem_we;
    CData/*0:0*/ tb_rv64i__DOT__halted;
    CData/*0:0*/ tb_rv64i__DOT__tohost_we;
    CData/*0:0*/ tb_rv64i__DOT__pass;
    CData/*4:0*/ tb_rv64i__DOT__u_core__DOT__rd;
    CData/*1:0*/ tb_rv64i__DOT__u_core__DOT__mem_size;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__pc_sel_imm;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__pc_sel_alu;
    CData/*2:0*/ tb_rv64i__DOT__u_core__DOT__branch_op;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__is_ebreak;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__is_ecall;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__is_csr;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__is_mret;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__trap_take;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_we;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_1;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_5;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_6;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_18;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_34;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_35;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_40;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_41;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_60;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_63;
    CData/*0:0*/ __Vtrigprevexpr___TOP__tb_rv64i__DOT__clk__0;
    SData/*11:0*/ tb_rv64i__DOT__u_core__DOT__csr_addr;
    IData/*31:0*/ tb_rv64i__DOT__cycles;
    VlWide<32>/*1023:0*/ tb_rv64i__DOT__u_mem__DOT__hexpath;
    IData/*31:0*/ __Vi;
    QData/*63:0*/ tb_rv64i__DOT__dmem_rdata;
    QData/*63:0*/ tb_rv64i__DOT__tohost_data;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__pc;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__imm;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__rs1_val;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__rs2_val;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__csr_rdata;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__alu_y;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__pc_plus4;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__pc_plus_imm;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__misa_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mie_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtvec_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mscratch_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mepc_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mcause_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtval_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mip_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mhartid_reg;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val;
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
