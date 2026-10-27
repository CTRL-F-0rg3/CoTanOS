// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_rv64i.h for the primary calling header

#include "Vtb_rv64i__pch.h"

VlCoroutine Vtb_rv64i___024root___eval_initial__TOP__Vtiming__0(Vtb_rv64i___024root* vlSelf);
VlCoroutine Vtb_rv64i___024root___eval_initial__TOP__Vtiming__1(Vtb_rv64i___024root* vlSelf);

void Vtb_rv64i___024root___eval_initial(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_initial\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_initial__TOP
        IData/*31:0*/ __Vinline_0__eval_initial__TOP_tb_rv64i__DOT__u_mem__DOT____VlemExpr_0;
        __Vinline_0__eval_initial__TOP_tb_rv64i__DOT__u_mem__DOT____VlemExpr_0 
            = VL_VALUEPLUSARGS_INW(1024, "HEX=%s"s, 
                                   vlSelfRef.tb_rv64i__DOT__u_mem__DOT__hexpath);
        if (VL_UNLIKELY((__Vinline_0__eval_initial__TOP_tb_rv64i__DOT__u_mem__DOT____VlemExpr_0))) {
            VL_READMEM_N(true, 32, 32768, 0, VL_CVT_PACK_STR_NW(32, vlSelfRef.tb_rv64i__DOT__u_mem__DOT__hexpath)
                         ,  &(vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem)
                         , 0, ~0ULL);
        }
    }
    Vtb_rv64i___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtb_rv64i___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtb_rv64i___024root___eval_sample(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_sample\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtb_rv64i___024root___eval_ico(Vtb_rv64i___024root* vlSelf, CData/*0:0*/ firstIteration) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_ico\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VicoTriggered[0U]) 
                                     | (IData)((IData)(firstIteration)));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_rv64i___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    return (0U);
}

void Vtb_rv64i___024root___timing_ready(Vtb_rv64i___024root* vlSelf);
void Vtb_rv64i___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
bool Vtb_rv64i___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);
void Vtb_rv64i___024root___timing_resume(Vtb_rv64i___024root* vlSelf);

bool Vtb_rv64i___024root___eval_act(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_act\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__act
        vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                        ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                          << 1U) 
                                                         | ((IData)(vlSelfRef.tb_rv64i__DOT__clk) 
                                                            & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i__DOT__clk__0))))));
        vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i__DOT__clk__0 
            = vlSelfRef.tb_rv64i__DOT__clk;
    }
    Vtb_rv64i___024root___timing_ready(vlSelf);
    Vtb_rv64i___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_rv64i___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtb_rv64i___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtb_rv64i___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtb_rv64i___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

bool Vtb_rv64i___024root___eval_inact(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_inact\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("tb_rv64i.v", 2, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtb_rv64i___024root___nba_sequent__TOP__0(Vtb_rv64i___024root* vlSelf);
void Vtb_rv64i___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out);

bool Vtb_rv64i___024root___eval_nba(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_nba\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtb_rv64i___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        {
            // Inlined CFunc: _eval_body__nba
            if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
                Vtb_rv64i___024root___nba_sequent__TOP__0(vlSelf);
            }
        }
        Vtb_rv64i___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

bool Vtb_rv64i___024root___eval_obs(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_obs\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    return (0U);
}

bool Vtb_rv64i___024root___eval_react(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_react\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    return (0U);
}

void Vtb_rv64i___024root___eval_postponed(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_postponed\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vtb_rv64i___024root____VbeforeTrig_h0c2173a0__0(Vtb_rv64i___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtb_rv64i___024root___eval_initial__TOP__Vtiming__0(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "tb_rv64i.v", 
                                         39);
    vlSelfRef.tb_rv64i__DOT__rst = 0U;
    while ((((~ (IData)(vlSelfRef.tb_rv64i__DOT__halted)) 
             & (~ (IData)(vlSelfRef.tb_rv64i__DOT__tohost_we))) 
            & VL_GTS_III(32, 0x00030d40U, vlSelfRef.tb_rv64i__DOT__cycles))) {
        Vtb_rv64i___024root____VbeforeTrig_h0c2173a0__0(vlSelf, 
                                                        "@(posedge tb_rv64i.clk)");
        co_await vlSelfRef.__VtrigSched_h0c2173a0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge tb_rv64i.clk)", 
                                                             "tb_rv64i.v", 
                                                             42);
        vlSelfRef.tb_rv64i__DOT__cycles = ((IData)(1U) 
                                           + vlSelfRef.tb_rv64i__DOT__cycles);
    }
    if (VL_UNLIKELY((vlSelfRef.tb_rv64i__DOT__tohost_we))) {
        vlSelfRef.tb_rv64i__DOT__pass = ((1ULL == vlSelfRef.tb_rv64i__DOT__tohost_data) 
                                         | (0ULL == vlSelfRef.tb_rv64i__DOT__tohost_data));
        VL_WRITEF_NX("[tohost] value=0x%016x\n",1, '#',64,vlSelfRef.tb_rv64i__DOT__tohost_data);
    } else if (vlSelfRef.tb_rv64i__DOT__halted) {
        vlSelfRef.tb_rv64i__DOT__pass = 1U;
        VL_WRITEF_NX("[halt] cykli=%0d\n",1, '~',32,vlSelfRef.tb_rv64i__DOT__cycles);
    } else {
        vlSelfRef.tb_rv64i__DOT__pass = 0U;
        VL_WRITEF_NX("[timeout] cykli=%0d\n",1, '~',32,vlSelfRef.tb_rv64i__DOT__cycles);
    }
    VL_WRITEF_NX("x1 =0x%016x\nx2 =0x%016x\nx3 =0x%016x\nx4 =0x%016x\n",4
                 , '#',64,vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[1U]
                 , '#',64,vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[2U]
                 , '#',64,vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[3U]
                 , '#',64,vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[4U]);
    if (vlSelfRef.tb_rv64i__DOT__pass) {
        VL_WRITEF_NX("*** PASS ***\n",0);
    } else {
        VL_WRITEF_NX("*** FAIL ***\n",0);
    }
    VL_FINISH_MT("tb_rv64i.v", 64, "");
    co_return;
}

VlCoroutine Vtb_rv64i___024root___eval_initial__TOP__Vtiming__1(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "tb_rv64i.v", 
                                             33);
        vlSelfRef.tb_rv64i__DOT__clk = (1U & (~ (IData)(vlSelfRef.tb_rv64i__DOT__clk)));
    }
    co_return;
}

bool Vtb_rv64i___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___trigger_anySet__ico\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

bool Vtb_rv64i___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vtb_rv64i___024root___nba_sequent__TOP__0(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___nba_sequent__TOP__0\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ tb_rv64i__DOT__u_core__DOT__rs1;
    tb_rv64i__DOT__u_core__DOT__rs1 = 0;
    CData/*4:0*/ tb_rv64i__DOT__u_core__DOT__rs2;
    tb_rv64i__DOT__u_core__DOT__rs2 = 0;
    CData/*4:0*/ tb_rv64i__DOT__u_core__DOT__alu_op;
    tb_rv64i__DOT__u_core__DOT__alu_op = 0;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__alu_a;
    tb_rv64i__DOT__u_core__DOT__alu_a = 0;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__alu_b;
    tb_rv64i__DOT__u_core__DOT__alu_b = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__addw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__addw_full = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__subw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__subw_full = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sllw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sllw_full = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sraw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sraw_full = 0;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i;
    tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_10;
    __VdfgRegularize_h6e95ff9d_0_10 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_19;
    __VdfgRegularize_h6e95ff9d_0_19 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_27;
    __VdfgRegularize_h6e95ff9d_0_27 = 0;
    CData/*1:0*/ __VdfgRegularize_h6e95ff9d_0_30;
    __VdfgRegularize_h6e95ff9d_0_30 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_39;
    __VdfgRegularize_h6e95ff9d_0_39 = 0;
    QData/*63:0*/ __VdfgRegularize_h6e95ff9d_0_40;
    __VdfgRegularize_h6e95ff9d_0_40 = 0;
    QData/*63:0*/ __VdfgRegularize_h6e95ff9d_0_41;
    __VdfgRegularize_h6e95ff9d_0_41 = 0;
    QData/*63:0*/ __Vdly__tb_rv64i__DOT__u_core__DOT__pc;
    __Vdly__tb_rv64i__DOT__u_core__DOT__pc = 0;
    CData/*0:0*/ __Vdly__tb_rv64i__DOT__halted;
    __Vdly__tb_rv64i__DOT__halted = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v0;
    __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v0 = 0;
    QData/*63:0*/ __VdlyVal__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32;
    __VdlyVal__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 0;
    CData/*4:0*/ __VdlyDim0__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32;
    __VdlyDim0__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32;
    __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 0;
    QData/*63:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v0;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v0 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v0;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v0;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v0 = 0;
    IData/*31:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v1;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v1 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v1;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v1 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v1;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v1 = 0;
    IData/*31:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v2;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v2 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v2;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v2 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v2;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v2 = 0;
    SData/*15:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v3;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v3 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v3;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v3 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v3;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v3 = 0;
    SData/*15:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v4;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v4 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v4;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v4 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v4;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v4 = 0;
    SData/*15:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v5;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v5 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v5;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v5 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v5;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v5 = 0;
    SData/*15:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v6;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v6 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v6;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v6 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v6;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v6 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v7;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v7 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v7;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v7 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v7;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v7 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v8;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v8 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v8;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v8 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v8;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v8 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v9;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v9 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v9;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v9 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v9;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v9 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v10;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v10 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v10;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v10 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v10;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v10 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v11;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v11 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v11;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v11 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v11;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v11 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v12;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v12 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v12;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v12 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v12;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v12 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v13;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v13 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v13;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v13 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v13;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v13 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v14;
    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v14 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v14;
    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v14 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v14;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v14 = 0;
    // Body
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v0 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v1 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v2 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v3 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v4 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v5 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v6 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v7 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v8 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v9 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v10 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v11 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v12 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v13 = 0U;
    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v14 = 0U;
    __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v0 = 0U;
    __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 0U;
    __Vdly__tb_rv64i__DOT__halted = vlSelfRef.tb_rv64i__DOT__halted;
    __Vdly__tb_rv64i__DOT__u_core__DOT__pc = vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc;
    if (vlSelfRef.tb_rv64i__DOT__rst) {
        __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v0 = 1U;
        __Vdly__tb_rv64i__DOT__u_core__DOT__pc = 0x0000000080000000ULL;
        __Vdly__tb_rv64i__DOT__halted = 0U;
    } else {
        if ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13)
               ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4) 
                  & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                     & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28)))
               : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)
                   ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_31) 
                      & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                   : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_32))) 
             & (0U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd)))) {
            __VdlyVal__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 
                = (((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_imm) 
                    | (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_alu))
                    ? (4ULL + vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc)
                    : (((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)) 
                        & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_37))
                        ? ((((0U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))
                              ? ((- (QData)((IData)(
                                                    (1U 
                                                     & (IData)(
                                                               (vlSelfRef.tb_rv64i__DOT__dmem_rdata 
                                                                >> 7U)))))) 
                                 & (- (QData)((IData)(
                                                      (1U 
                                                       & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9)))))))
                              : ((0x00ffffffffffff00ULL 
                                  & (((1U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))
                                       ? ((- (QData)((IData)(
                                                             (1U 
                                                              & (IData)(
                                                                        (vlSelfRef.tb_rv64i__DOT__dmem_rdata 
                                                                         >> 0x0000000fU)))))) 
                                          & (- (QData)((IData)(
                                                               (1U 
                                                                & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9)))))))
                                       : (((QData)((IData)(
                                                           ((2U 
                                                             == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))
                                                             ? 
                                                            ((- (IData)(
                                                                        (1U 
                                                                         & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__dmem_rdata 
                                                                                >> 0x0000001fU))))) 
                                                             & (- (IData)(
                                                                          (1U 
                                                                           & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9))))))
                                                             : (IData)(
                                                                       (vlSelfRef.tb_rv64i__DOT__dmem_rdata 
                                                                        >> 0x00000020U))))) 
                                           << 0x00000010U) 
                                          | (QData)((IData)(
                                                            (0x0000ffffU 
                                                             & (IData)(
                                                                       (vlSelfRef.tb_rv64i__DOT__dmem_rdata 
                                                                        >> 0x00000010U))))))) 
                                     << 8U)) | (QData)((IData)(
                                                               (0x000000ffU 
                                                                & (IData)(
                                                                          (vlSelfRef.tb_rv64i__DOT__dmem_rdata 
                                                                           >> 8U))))))) 
                            << 8U) | (QData)((IData)(
                                                     (0x000000ffU 
                                                      & (IData)(vlSelfRef.tb_rv64i__DOT__dmem_rdata)))))
                        : vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y));
            __VdlyDim0__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 
                = vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd;
            __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 1U;
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_rv64i__DOT__halted)))) {
            if ((1U & (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                        & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12) 
                           & (((1U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21)) 
                               & (3U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38))) 
                              | ((3U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38)) 
                                 & ((1U != (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21)) 
                                    & (0U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21))))))) 
                       | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13)
                           ? ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)) 
                              | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)
                                  ? ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_45)) 
                                     | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25) 
                                        | ((~ ((1U 
                                                == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21)) 
                                               | (0U 
                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21)))) 
                                           | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_33))))
                                  : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                      ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34)
                                      : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)
                                          ? (~ ((0U 
                                                 == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16)) 
                                                & (3U 
                                                   == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35))))
                                          : (((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9)) 
                                              & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)) 
                                             | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8))))))
                           : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)
                               ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)
                                   ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                       ? ((~ ((3U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)) 
                                              & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_43) 
                                                 | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_36)))) 
                                          | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))
                                       : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)
                                           ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8)
                                           : (~ ((3U 
                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)) 
                                                 & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_42) 
                                                    | ((6U 
                                                        == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                                                       | (7U 
                                                          == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))))))))
                                   : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8) 
                                      | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9) 
                                         | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_33))))
                               : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)
                                   ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                       ? ((~ ((3U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)) 
                                              & ((0U 
                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16)) 
                                                 | ((1U 
                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16))
                                                     ? 
                                                    (0U 
                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17))
                                                     : 
                                                    ((5U 
                                                      == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16)) 
                                                     & ((0U 
                                                         == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17)) 
                                                        | (0x20U 
                                                           == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17)))))))) 
                                          | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))
                                       : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)
                                           ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8)
                                           : (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9)
                                                ? (
                                                   (~ 
                                                    ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6) 
                                                     | ((0U 
                                                         == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_44)) 
                                                        | (0x10U 
                                                           == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_44))))) 
                                                   & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))
                                                : (
                                                   (~ 
                                                    ((0U 
                                                      == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_44)) 
                                                     | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6))) 
                                                   & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))) 
                                              | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8))))
                                   : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                       ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34)
                                       : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2) 
                                          | (((3U == 
                                               (3U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 0x0000000dU))) 
                                              & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)) 
                                             | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8)))))))))) {
                __Vdly__tb_rv64i__DOT__halted = 1U;
            } else {
                __Vdly__tb_rv64i__DOT__u_core__DOT__pc 
                    = ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_alu)
                        ? (0xfffffffffffffffeULL & 
                           (vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val 
                            + vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm))
                        : ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_imm)
                            ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_plus_imm
                            : (((4U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op))
                                 ? ((2U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op))
                                     ? ((~ (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op)) 
                                        & (vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val 
                                           >= vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val))
                                     : ((1U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op))
                                         ? (vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val 
                                            < vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val)
                                         : VL_GTES_IQQ(64, vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val, vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val)))
                                 : ((2U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op))
                                     ? ((1U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op))
                                         ? VL_LTS_IQQ(64, vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val, vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val)
                                         : (vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val 
                                            != vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val))
                                     : ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op) 
                                        & (vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val 
                                           == vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val))))
                                ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_plus_imm
                                : vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_plus4)));
            }
        }
    }
    vlSelfRef.tb_rv64i__DOT__tohost_we = 0U;
    if (vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_we) {
        if ((2U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))) {
            if ((1U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))) {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v0 
                    = vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val;
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v0 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v0 = 1U;
            } else if ((1U & (IData)((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                      >> 2U)))) {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v1 
                    = (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val);
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v1 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v1 = 1U;
            } else {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v2 
                    = (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val);
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v2 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v2 = 1U;
            }
        } else if ((1U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))) {
            if ((1U & (IData)((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                               >> 2U)))) {
                if ((1U & (IData)((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                   >> 1U)))) {
                    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v3 
                        = (0x0000ffffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v3 
                        = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                   - 0x0000000080000000ULL) 
                                                  >> 3U)));
                    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v3 = 1U;
                } else {
                    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v4 
                        = (0x0000ffffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v4 
                        = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                   - 0x0000000080000000ULL) 
                                                  >> 3U)));
                    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v4 = 1U;
                }
            } else if ((1U & (IData)((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                      >> 1U)))) {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v5 
                    = (0x0000ffffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v5 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v5 = 1U;
            } else {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v6 
                    = (0x0000ffffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v6 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v6 = 1U;
            }
        } else if ((1U & (IData)((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                  >> 2U)))) {
            if ((1U & (IData)((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                               >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y))) {
                    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v7 
                        = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v7 
                        = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                   - 0x0000000080000000ULL) 
                                                  >> 3U)));
                    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v7 = 1U;
                } else {
                    __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v8 
                        = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                    __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v8 
                        = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                   - 0x0000000080000000ULL) 
                                                  >> 3U)));
                    __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v8 = 1U;
                }
            } else if ((1U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y))) {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v9 
                    = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v9 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v9 = 1U;
            } else {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v10 
                    = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v10 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v10 = 1U;
            }
        } else if ((1U & (IData)((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                  >> 1U)))) {
            if ((1U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y))) {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v11 
                    = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v11 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v11 = 1U;
            } else {
                __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v12 
                    = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
                __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v12 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v12 = 1U;
            }
        } else if ((1U & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y))) {
            __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v13 
                = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
            __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v13 
                = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                           - 0x0000000080000000ULL) 
                                          >> 3U)));
            __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v13 = 1U;
        } else {
            __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v14 
                = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val));
            __VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v14 
                = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                           - 0x0000000080000000ULL) 
                                          >> 3U)));
            __VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v14 = 1U;
        }
        if ((0x0000000080001000ULL == vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)) {
            vlSelfRef.tb_rv64i__DOT__tohost_we = 1U;
            vlSelfRef.tb_rv64i__DOT__tohost_data = vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val;
        }
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v0) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v0] 
            = __VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v0;
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v1) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v1] 
            = ((0x00000000ffffffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v1]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v1)) 
                  << 0x00000020U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v2) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v2] 
            = ((0xffffffff00000000ULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v2]) 
               | (IData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v2)));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v3) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v3] 
            = ((0x0000ffffffffffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v3]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v3)) 
                  << 0x00000030U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v4) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v4] 
            = ((0xffff0000ffffffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v4]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v4)) 
                  << 0x00000020U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v5) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v5] 
            = ((0xffffffff0000ffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v5]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v5)) 
                  << 0x00000010U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v6) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v6] 
            = ((0xffffffffffff0000ULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v6]) 
               | (IData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v6)));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v7) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v7] 
            = ((0x00ffffffffffffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v7]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v7)) 
                  << 0x00000038U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v8) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v8] 
            = ((0xff00ffffffffffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v8]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v8)) 
                  << 0x00000030U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v9) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v9] 
            = ((0xffff00ffffffffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v9]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v9)) 
                  << 0x00000028U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v10) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v10] 
            = ((0xffffff00ffffffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v10]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v10)) 
                  << 0x00000020U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v11) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v11] 
            = ((0xffffffff00ffffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v11]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v11)) 
                  << 0x00000018U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v12) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v12] 
            = ((0xffffffffff00ffffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v12]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v12)) 
                  << 0x00000010U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v13) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v13] 
            = ((0xffffffffffff00ffULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v13]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v13)) 
                  << 8U));
    }
    if (__VdlySet__tb_rv64i__DOT__u_mem__DOT__dmem__v14) {
        vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v14] 
            = ((0xffffffffffffff00ULL & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i__DOT__u_mem__DOT__dmem__v14]) 
               | (IData)((IData)(__VdlyVal__tb_rv64i__DOT__u_mem__DOT__dmem__v14)));
    }
    if (__VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v0) {
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[0U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[1U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[2U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[3U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[4U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[5U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[6U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[7U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[8U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[9U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[10U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[11U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[12U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[13U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[14U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[15U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[16U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[17U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[18U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[19U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[20U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[21U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[22U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[23U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[24U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[25U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[26U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[27U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[28U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[29U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[30U] = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[31U] = 0ULL;
    }
    if (__VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32) {
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[__VdlyDim0__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32] 
            = __VdlyVal__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32;
    }
    vlSelfRef.tb_rv64i__DOT__halted = __Vdly__tb_rv64i__DOT__halted;
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc = __Vdly__tb_rv64i__DOT__u_core__DOT__pc;
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_plus4 
        = (4ULL + vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_33 = (0U 
                                                 != 
                                                 (3U 
                                                  & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                     [
                                                     (0x00007fffU 
                                                      & (IData)(
                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                  - 0x0000000080000000ULL) 
                                                                 >> 2U)))] 
                                                     >> 2U)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38 = (3U 
                                                 & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                    [
                                                    (0x00007fffU 
                                                     & (IData)(
                                                               ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                 - 0x0000000080000000ULL) 
                                                                >> 2U)))] 
                                                    >> 5U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25 = (1U 
                                                 & (~ 
                                                    (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                     [
                                                     (0x00007fffU 
                                                      & (IData)(
                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                  - 0x0000000080000000ULL) 
                                                                 >> 2U)))] 
                                                     >> 1U)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_44 = (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                 [(0x00007fffU 
                                                   & (IData)(
                                                             ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                               - 0x0000000080000000ULL) 
                                                              >> 2U)))] 
                                                 >> 0x0000001aU);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 0x0000000eU));
    tb_rv64i__DOT__u_core__DOT__rs1 = (0x0000001fU 
                                       & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                          [(0x00007fffU 
                                            & (IData)(
                                                      ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                        - 0x0000000080000000ULL) 
                                                       >> 2U)))] 
                                          >> 0x0000000fU));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd = (0x0000001fU 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 7U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 0x0000000cU));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 0x0000000dU));
    tb_rv64i__DOT__u_core__DOT__rs2 = (0x0000001fU 
                                       & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                          [(0x00007fffU 
                                            & (IData)(
                                                      ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                        - 0x0000000080000000ULL) 
                                                       >> 2U)))] 
                                          >> 0x00000014U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21 = (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                 [(0x00007fffU 
                                                   & (IData)(
                                                             ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                               - 0x0000000080000000ULL) 
                                                              >> 2U)))] 
                                                 >> 0x00000014U);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16 = (7U 
                                                 & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                    [
                                                    (0x00007fffU 
                                                     & (IData)(
                                                               ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                 - 0x0000000080000000ULL) 
                                                                >> 2U)))] 
                                                    >> 0x0000000cU));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17 = (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                 [(0x00007fffU 
                                                   & (IData)(
                                                             ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                               - 0x0000000080000000ULL) 
                                                              >> 2U)))] 
                                                 >> 0x00000019U);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_45 = (1U 
                                                 & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                 [(0x00007fffU 
                                                   & (IData)(
                                                             ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                               - 0x0000000080000000ULL) 
                                                              >> 2U)))]);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 5U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13 = (1U 
                                                 & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                    [
                                                    (0x00007fffU 
                                                     & (IData)(
                                                               ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                 - 0x0000000080000000ULL) 
                                                                >> 2U)))] 
                                                    >> 6U));
    __VdfgRegularize_h6e95ff9d_0_10 = (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                       [(0x00007fffU 
                                         & (IData)(
                                                   ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                     - 0x0000000080000000ULL) 
                                                    >> 2U)))] 
                                       >> 0x0000001fU);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 4U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 2U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 3U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35 = (3U 
                                                 & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                 [(0x00007fffU 
                                                   & (IData)(
                                                             ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                               - 0x0000000080000000ULL) 
                                                              >> 2U)))]);
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val = 
        ((0U == (IData)(tb_rv64i__DOT__u_core__DOT__rs1))
          ? 0ULL : vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs
         [tb_rv64i__DOT__u_core__DOT__rs1]);
    __VdfgRegularize_h6e95ff9d_0_30 = (3U & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                                              ? (2U 
                                                 | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))))
                                              : (1U 
                                                 & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))))));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val = 
        ((0U == (IData)(tb_rv64i__DOT__u_core__DOT__rs2))
          ? 0ULL : vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs
         [tb_rv64i__DOT__u_core__DOT__rs2]);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5 = (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17) 
                                                 << 3U) 
                                                | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8 = (1U 
                                                & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25) 
                                                   | (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_45))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28 = ((3U 
                                                  == 
                                                  (3U 
                                                   & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                      [
                                                      (0x00007fffU 
                                                       & (IData)(
                                                                 ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                   - 0x0000000080000000ULL) 
                                                                  >> 2U)))] 
                                                      >> 1U))) 
                                                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_45));
    tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i = 
        (((- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_10))) 
          << 0x0000000cU) | (QData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21)));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op 
        = (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9)
             ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                 ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                     ? 6U : 5U) : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                    ? 4U : 3U)) : (
                                                   ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                     ? 2U
                                                     : 1U) 
                                                   & (- (IData)(
                                                                (1U 
                                                                 & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6))))))) 
           & (- (IData)(((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                         & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)) 
                            & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)) 
                               & (3U == ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35) 
                                         & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38)))))))));
    __VdfgRegularize_h6e95ff9d_0_40 = ((((QData)((IData)(
                                                         (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_10))))) 
                                         << 0x00000020U) 
                                        | (QData)((IData)(
                                                          (0xfffff000U 
                                                           & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                           [
                                                           (0x00007fffU 
                                                            & (IData)(
                                                                      ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                        - 0x0000000080000000ULL) 
                                                                       >> 2U)))])))) 
                                       & (- (QData)((IData)(
                                                            (3U 
                                                             == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35))))));
    __VdfgRegularize_h6e95ff9d_0_19 = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)) 
                                       & (3U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size 
        = (3U & (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)
                   ? ((IData)(__VdfgRegularize_h6e95ff9d_0_30) 
                      | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))) 
                         | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))) 
                            | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25))) 
                               | ((- (IData)((1U & 
                                              (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_45))))) 
                                  | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))) 
                                     | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9)))))))))
                   : ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))) 
                      | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))) 
                         | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25))) 
                            | ((- (IData)((1U & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_45))))) 
                               | ((IData)(__VdfgRegularize_h6e95ff9d_0_30) 
                                  | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))))))))) 
                 | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13)))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_36 = ((5U 
                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                                                 | (0x0105U 
                                                    == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_43 = ((0U 
                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                                                 | ((0x0100U 
                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                                                    | (1U 
                                                       == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34 = (1U 
                                                 & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)) 
                                                    | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8)));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_imm 
        = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
           & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3) 
              & ((3U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38)) 
                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28))));
    __VdfgRegularize_h6e95ff9d_0_27 = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)) 
                                       & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28));
    __VdfgRegularize_h6e95ff9d_0_41 = (tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i 
                                       & (- (QData)((IData)(
                                                            (3U 
                                                             == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_31 = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                                  ? (IData)(__VdfgRegularize_h6e95ff9d_0_19)
                                                  : 
                                                 (3U 
                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12 = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)) 
                                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_19));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_42 = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_43) 
                                                 | ((2U 
                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                                                    | ((3U 
                                                        == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                                                       | ((4U 
                                                           == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                                                          | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_36)))));
    tb_rv64i__DOT__u_core__DOT__alu_a = (((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13)) 
                                          & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)) 
                                             & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                                & (IData)(__VdfgRegularize_h6e95ff9d_0_27))))
                                          ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc
                                          : vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val);
    __VdfgRegularize_h6e95ff9d_0_39 = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                       & (IData)(__VdfgRegularize_h6e95ff9d_0_27));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_37 = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                                 & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13)) 
                                                    & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_32 = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)
                                                  ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_31)
                                                  : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13)
                                                  ? 
                                                 ((- (QData)((IData)(
                                                                     (1U 
                                                                      & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)))))) 
                                                  & (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                                       ? 
                                                      ((((- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_10))) 
                                                         << 0x00000015U) 
                                                        | (QData)((IData)(
                                                                          ((((IData)(__VdfgRegularize_h6e95ff9d_0_10) 
                                                                             << 0x00000014U) 
                                                                            | (((0x000001feU 
                                                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                                >> 0x0000000bU)) 
                                                                                | (1U 
                                                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                                >> 0x00000014U))) 
                                                                               << 0x0000000bU)) 
                                                                           | (0x000007feU 
                                                                              & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                                >> 0x00000014U)))))) 
                                                       & (- (QData)((IData)(
                                                                            (7U 
                                                                             == 
                                                                             (7U 
                                                                              & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                              [
                                                                              (0x00007fffU 
                                                                               & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))]))))))
                                                       : 
                                                      ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)
                                                        ? __VdfgRegularize_h6e95ff9d_0_41
                                                        : 
                                                       ((((- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_10))) 
                                                          << 0x0000000dU) 
                                                         | (QData)((IData)(
                                                                           ((((IData)(__VdfgRegularize_h6e95ff9d_0_10) 
                                                                              << 0x0000000cU) 
                                                                             | (0x00000800U 
                                                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                                << 4U))) 
                                                                            | ((0x000007e0U 
                                                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                                >> 0x00000014U)) 
                                                                               | (0x0000001eU 
                                                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                                >> 7U))))))) 
                                                        & (- (QData)((IData)(
                                                                             (3U 
                                                                              == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)))))))) 
                                                     & (- (QData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)))))
                                                  : 
                                                 ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)
                                                   ? 
                                                  ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)
                                                    ? 
                                                   ((- (QData)((IData)(
                                                                       ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)) 
                                                                        & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))))) 
                                                    & __VdfgRegularize_h6e95ff9d_0_40)
                                                    : 
                                                   ((((- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_10))) 
                                                      << 0x0000000cU) 
                                                     | (QData)((IData)(
                                                                       (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17) 
                                                                         << 5U) 
                                                                        | (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd))))) 
                                                    & (- (QData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12)))))
                                                   : 
                                                  ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)
                                                    ? 
                                                   ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                                     ? 
                                                    (((0U 
                                                       == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16))
                                                       ? tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i
                                                       : 
                                                      ((1U 
                                                        == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16))
                                                        ? (QData)((IData)(tb_rv64i__DOT__u_core__DOT__rs2))
                                                        : 
                                                       ((5U 
                                                         == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16))
                                                         ? (QData)((IData)(tb_rv64i__DOT__u_core__DOT__rs2))
                                                         : tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i))) 
                                                     & (- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_19))))
                                                     : 
                                                    ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)
                                                      ? __VdfgRegularize_h6e95ff9d_0_40
                                                      : 
                                                     (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                                                        ? tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i
                                                        : 
                                                       ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                         ? (QData)((IData)(
                                                                           (0x0000003fU 
                                                                            & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                               [
                                                                               (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                               >> 0x00000014U))))
                                                         : tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i)) 
                                                      & (- (QData)((IData)(
                                                                           (3U 
                                                                            == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35))))))))
                                                    : 
                                                   (__VdfgRegularize_h6e95ff9d_0_41 
                                                    & ((- (QData)((IData)(
                                                                          (1U 
                                                                           & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)))))) 
                                                       & (- (QData)((IData)(
                                                                            (1U 
                                                                             & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)))))))))));
    tb_rv64i__DOT__u_core__DOT__alu_op = (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)
                                            ? (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                                 ? 
                                                (((0U 
                                                   == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                   ? 0x0bU
                                                   : 
                                                  ((0x0100U 
                                                    == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                    ? 0x0cU
                                                    : 
                                                   ((1U 
                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                     ? 0x0dU
                                                     : 
                                                    ((5U 
                                                      == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                      ? 0x0eU
                                                      : 
                                                     (0x0fU 
                                                      & (- (IData)(
                                                                   (0x0105U 
                                                                    == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))))))))) 
                                                 & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_19))))
                                                 : 
                                                ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)
                                                  ? 
                                                 (0x0aU 
                                                  & (- (IData)(
                                                               (3U 
                                                                == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)))))
                                                  : 
                                                 (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_42)
                                                    ? 
                                                   (((0x0100U 
                                                      == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                      ? 1U
                                                      : 
                                                     ((1U 
                                                       == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                       ? 2U
                                                       : 
                                                      ((2U 
                                                        == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                        ? 3U
                                                        : 
                                                       ((3U 
                                                         == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                         ? 4U
                                                         : 
                                                        ((4U 
                                                          == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                          ? 5U
                                                          : 
                                                         ((5U 
                                                           == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                           ? 6U
                                                           : 7U)))))) 
                                                    & (- (IData)(
                                                                 (0U 
                                                                  != (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)))))
                                                    : 
                                                   ((6U 
                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))
                                                     ? 8U
                                                     : 
                                                    (9U 
                                                     & (- (IData)(
                                                                  (7U 
                                                                   == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))))))) 
                                                  & (- (IData)(
                                                               (3U 
                                                                == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35))))))) 
                                               & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))))
                                            : (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                                                 ? 
                                                (((0U 
                                                   == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16))
                                                   ? 0x0bU
                                                   : 
                                                  ((1U 
                                                    == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16))
                                                    ? 0x0dU
                                                    : 
                                                   (((0U 
                                                      == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17))
                                                      ? 0x0eU
                                                      : 
                                                     (0x0fU 
                                                      & (- (IData)(
                                                                   (0x20U 
                                                                    == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17)))))) 
                                                    & (- (IData)(
                                                                 (5U 
                                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16))))))) 
                                                 & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_19))))
                                                 : 
                                                ((- (IData)(
                                                            (1U 
                                                             & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))))) 
                                                 & (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_9)
                                                      ? 
                                                     ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                                                       ? 
                                                      ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                        ? 9U
                                                        : 8U)
                                                       : 
                                                      ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                        ? 
                                                       ((0U 
                                                         == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_44))
                                                         ? 6U
                                                         : 
                                                        (7U 
                                                         & (- (IData)(
                                                                      (0x10U 
                                                                       == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_44))))))
                                                        : 5U))
                                                      : 
                                                     ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                                                       ? 
                                                      ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                        ? 4U
                                                        : 3U)
                                                       : 
                                                      (2U 
                                                       & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)))))) 
                                                    & (- (IData)(
                                                                 (3U 
                                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35))))))) 
                                               & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))))) 
                                          & (- (IData)(
                                                       (1U 
                                                        & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13))))));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_alu 
        = ((3U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38)) 
           & (IData)(__VdfgRegularize_h6e95ff9d_0_39));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_we = 
        ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4) 
         & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_37));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_plus_imm 
        = (vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
           + vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm);
    tb_rv64i__DOT__u_core__DOT__alu_b = (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_13)
                                           ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4) 
                                              & (IData)(__VdfgRegularize_h6e95ff9d_0_39))
                                           : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4)
                                               ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)
                                                   ? (IData)(__VdfgRegularize_h6e95ff9d_0_27)
                                                   : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12))
                                               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_32)))
                                          ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm
                                          : vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val);
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__addw_full 
        = ((IData)(tb_rv64i__DOT__u_core__DOT__alu_a) 
           + (IData)(tb_rv64i__DOT__u_core__DOT__alu_b));
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__subw_full 
        = ((IData)(tb_rv64i__DOT__u_core__DOT__alu_a) 
           - (IData)(tb_rv64i__DOT__u_core__DOT__alu_b));
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sllw_full 
        = ((IData)(tb_rv64i__DOT__u_core__DOT__alu_a) 
           << (0x0000001fU & (IData)(tb_rv64i__DOT__u_core__DOT__alu_b)));
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sraw_full 
        = VL_SHIFTRS_III(32,32,5, (IData)(tb_rv64i__DOT__u_core__DOT__alu_a), 
                         (0x0000001fU & (IData)(tb_rv64i__DOT__u_core__DOT__alu_b)));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y = (
                                                   ((8U 
                                                     & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                     ? 
                                                    ((4U 
                                                      & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                      ? 
                                                     ((2U 
                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (((QData)((IData)(
                                                                         (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sraw_full 
                                                                                >> 0x0000001fU))))) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sraw_full)))
                                                        : (QData)((IData)(
                                                                          ((IData)(tb_rv64i__DOT__u_core__DOT__alu_a) 
                                                                           >> 
                                                                           (0x0000001fU 
                                                                            & (IData)(tb_rv64i__DOT__u_core__DOT__alu_b))))))
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (((QData)((IData)(
                                                                         (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sllw_full 
                                                                                >> 0x0000001fU))))) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sllw_full)))
                                                        : 
                                                       (((QData)((IData)(
                                                                         (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__subw_full 
                                                                                >> 0x0000001fU))))) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__subw_full)))))
                                                      : 
                                                     ((2U 
                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (((QData)((IData)(
                                                                         (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__addw_full 
                                                                                >> 0x0000001fU))))) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__addw_full)))
                                                        : tb_rv64i__DOT__u_core__DOT__alu_b)
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (tb_rv64i__DOT__u_core__DOT__alu_a 
                                                        & tb_rv64i__DOT__u_core__DOT__alu_b)
                                                        : 
                                                       (tb_rv64i__DOT__u_core__DOT__alu_a 
                                                        | tb_rv64i__DOT__u_core__DOT__alu_b))))
                                                     : 
                                                    ((4U 
                                                      & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                      ? 
                                                     ((2U 
                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       VL_SHIFTRS_QQI(64,64,6, tb_rv64i__DOT__u_core__DOT__alu_a, 
                                                                      (0x0000003fU 
                                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__alu_b)))
                                                        : 
                                                       (tb_rv64i__DOT__u_core__DOT__alu_a 
                                                        >> 
                                                        (0x0000003fU 
                                                         & (IData)(tb_rv64i__DOT__u_core__DOT__alu_b))))
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (tb_rv64i__DOT__u_core__DOT__alu_a 
                                                        ^ tb_rv64i__DOT__u_core__DOT__alu_b)
                                                        : 
                                                       (1ULL 
                                                        & (- (QData)((IData)(
                                                                             (tb_rv64i__DOT__u_core__DOT__alu_a 
                                                                              < tb_rv64i__DOT__u_core__DOT__alu_b)))))))
                                                      : 
                                                     ((2U 
                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (1ULL 
                                                        & (- (QData)((IData)(
                                                                             VL_LTS_IQQ(64, tb_rv64i__DOT__u_core__DOT__alu_a, tb_rv64i__DOT__u_core__DOT__alu_b)))))
                                                        : 
                                                       (tb_rv64i__DOT__u_core__DOT__alu_a 
                                                        << 
                                                        (0x0000003fU 
                                                         & (IData)(tb_rv64i__DOT__u_core__DOT__alu_b))))
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (tb_rv64i__DOT__u_core__DOT__alu_a 
                                                        - tb_rv64i__DOT__u_core__DOT__alu_b)
                                                        : 
                                                       (tb_rv64i__DOT__u_core__DOT__alu_a 
                                                        + tb_rv64i__DOT__u_core__DOT__alu_b))))) 
                                                   & (- (QData)((IData)(
                                                                        (1U 
                                                                         & (~ 
                                                                            ((IData)(tb_rv64i__DOT__u_core__DOT__alu_op) 
                                                                             >> 4U)))))));
    vlSelfRef.tb_rv64i__DOT__dmem_rdata = ((3U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))
                                            ? vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                           [(0x00007fffU 
                                             & (IData)(
                                                       ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                         - 0x0000000080000000ULL) 
                                                        >> 3U)))]
                                            : ((2U 
                                                == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))
                                                ? (QData)((IData)(
                                                                  ((1U 
                                                                    & (IData)(
                                                                              (vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                               >> 2U)))
                                                                    ? (IData)(
                                                                              (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                               [
                                                                               (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                               >> 0x00000020U))
                                                                    : (IData)(vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                              [
                                                                              (0x00007fffU 
                                                                               & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))]))))
                                                : (
                                                   (1U 
                                                    == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))
                                                    ? 
                                                   ((0U 
                                                     == 
                                                     (3U 
                                                      & (IData)(
                                                                (vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                 >> 1U))))
                                                     ? (QData)((IData)(
                                                                       (0x0000ffffU 
                                                                        & (IData)(vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))]))))
                                                     : 
                                                    ((1U 
                                                      == 
                                                      (3U 
                                                       & (IData)(
                                                                 (vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                  >> 1U))))
                                                      ? (QData)((IData)(
                                                                        (0x0000ffffU 
                                                                         & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                                >> 0x00000010U)))))
                                                      : 
                                                     ((2U 
                                                       == 
                                                       (3U 
                                                        & (IData)(
                                                                  (vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                   >> 1U))))
                                                       ? (QData)((IData)(
                                                                         (0x0000ffffU 
                                                                          & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                                >> 0x00000020U)))))
                                                       : 
                                                      (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                       [
                                                       (0x00007fffU 
                                                        & (IData)(
                                                                  ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                    - 0x0000000080000000ULL) 
                                                                   >> 3U)))] 
                                                       >> 0x00000030U))))
                                                    : 
                                                   ((0U 
                                                     == 
                                                     (7U 
                                                      & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)))
                                                     ? (QData)((IData)(
                                                                       (0x000000ffU 
                                                                        & (IData)(vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))]))))
                                                     : 
                                                    ((1U 
                                                      == 
                                                      (7U 
                                                       & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)))
                                                      ? (QData)((IData)(
                                                                        (0x000000ffU 
                                                                         & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                                >> 8U)))))
                                                      : 
                                                     ((2U 
                                                       == 
                                                       (7U 
                                                        & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)))
                                                       ? (QData)((IData)(
                                                                         (0x000000ffU 
                                                                          & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                                >> 0x00000010U)))))
                                                       : 
                                                      ((3U 
                                                        == 
                                                        (7U 
                                                         & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)))
                                                        ? (QData)((IData)(
                                                                          (0x000000ffU 
                                                                           & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                                >> 0x00000018U)))))
                                                        : 
                                                       ((4U 
                                                         == 
                                                         (7U 
                                                          & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)))
                                                         ? (QData)((IData)(
                                                                           (0x000000ffU 
                                                                            & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                                >> 0x00000020U)))))
                                                         : 
                                                        ((5U 
                                                          == 
                                                          (7U 
                                                           & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)))
                                                          ? (QData)((IData)(
                                                                            (0x000000ffU 
                                                                             & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                                >> 0x00000028U)))))
                                                          : 
                                                         ((6U 
                                                           == 
                                                           (7U 
                                                            & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)))
                                                           ? (QData)((IData)(
                                                                             (0x000000ffU 
                                                                              & (IData)(
                                                                                (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                                                [
                                                                                (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                                >> 0x00000030U)))))
                                                           : 
                                                          (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__dmem
                                                           [
                                                           (0x00007fffU 
                                                            & (IData)(
                                                                      ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y 
                                                                        - 0x0000000080000000ULL) 
                                                                       >> 3U)))] 
                                                           >> 0x00000038U)))))))))));
}

void Vtb_rv64i___024root___timing_ready(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___timing_ready\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h0c2173a0__0.ready("@(posedge tb_rv64i.clk)");
    }
}

void Vtb_rv64i___024root___timing_resume(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___timing_resume\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h0c2173a0__0.moveToResumeQueue(
                                                          "@(posedge tb_rv64i.clk)");
    vlSelfRef.__VtrigSched_h0c2173a0__0.resume("@(posedge tb_rv64i.clk)");
    if ((2ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtb_rv64i___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

void Vtb_rv64i___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

void Vtb_rv64i___024root____VbeforeTrig_h0c2173a0__0(Vtb_rv64i___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root____VbeforeTrig_h0c2173a0__0\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((IData)(vlSelfRef.tb_rv64i__DOT__clk) 
                                  & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i__DOT__clk__0)))));
    vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i__DOT__clk__0 
        = vlSelfRef.tb_rv64i__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h0c2173a0__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtb_rv64i___024root___eval_debug_assertions(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_debug_assertions\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
