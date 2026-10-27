// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_rv64i_pipe.h for the primary calling header

#include "Vtb_rv64i_pipe__pch.h"

VlCoroutine Vtb_rv64i_pipe___024root___eval_initial__TOP__Vtiming__0(Vtb_rv64i_pipe___024root* vlSelf);
VlCoroutine Vtb_rv64i_pipe___024root___eval_initial__TOP__Vtiming__1(Vtb_rv64i_pipe___024root* vlSelf);

void Vtb_rv64i_pipe___024root___eval_initial(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_initial\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_initial__TOP
        IData/*31:0*/ __Vinline_0__eval_initial__TOP_tb_rv64i_pipe__DOT__u_mem__DOT____VlemExpr_0;
        __Vinline_0__eval_initial__TOP_tb_rv64i_pipe__DOT__u_mem__DOT____VlemExpr_0 
            = VL_VALUEPLUSARGS_INW(1024, "HEX=%s"s, 
                                   vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__hexpath);
        if (VL_UNLIKELY((__Vinline_0__eval_initial__TOP_tb_rv64i_pipe__DOT__u_mem__DOT____VlemExpr_0))) {
            VL_READMEM_N(true, 32, 32768, 0, VL_CVT_PACK_STR_NW(32, vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__hexpath)
                         ,  &(vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__imem)
                         , 0, ~0ULL);
        }
    }
    Vtb_rv64i_pipe___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtb_rv64i_pipe___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtb_rv64i_pipe___024root___eval_sample(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_sample\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtb_rv64i_pipe___024root___eval_ico(Vtb_rv64i_pipe___024root* vlSelf, CData/*0:0*/ firstIteration) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_ico\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VicoTriggered[0U]) 
                                     | (IData)((IData)(firstIteration)));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_rv64i_pipe___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    return (0U);
}

void Vtb_rv64i_pipe___024root___timing_ready(Vtb_rv64i_pipe___024root* vlSelf);
void Vtb_rv64i_pipe___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
bool Vtb_rv64i_pipe___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);
void Vtb_rv64i_pipe___024root___timing_resume(Vtb_rv64i_pipe___024root* vlSelf);

bool Vtb_rv64i_pipe___024root___eval_act(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_act\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__act
        vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                        ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                          << 1U) 
                                                         | ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__clk) 
                                                            & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i_pipe__DOT__clk__0))))));
        vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i_pipe__DOT__clk__0 
            = vlSelfRef.tb_rv64i_pipe__DOT__clk;
    }
    Vtb_rv64i_pipe___024root___timing_ready(vlSelf);
    Vtb_rv64i_pipe___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_rv64i_pipe___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtb_rv64i_pipe___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtb_rv64i_pipe___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtb_rv64i_pipe___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

bool Vtb_rv64i_pipe___024root___eval_inact(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_inact\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("tb_rv64i_pipe.v", 2, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtb_rv64i_pipe___024root___nba_sequent__TOP__0(Vtb_rv64i_pipe___024root* vlSelf);
void Vtb_rv64i_pipe___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out);

bool Vtb_rv64i_pipe___024root___eval_nba(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_nba\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtb_rv64i_pipe___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        {
            // Inlined CFunc: _eval_body__nba
            if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
                Vtb_rv64i_pipe___024root___nba_sequent__TOP__0(vlSelf);
            }
        }
        Vtb_rv64i_pipe___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

bool Vtb_rv64i_pipe___024root___eval_obs(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_obs\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    return (0U);
}

bool Vtb_rv64i_pipe___024root___eval_react(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_react\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    return (0U);
}

void Vtb_rv64i_pipe___024root___eval_postponed(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_postponed\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vtb_rv64i_pipe___024root____VbeforeTrig_h3f920df9__0(Vtb_rv64i_pipe___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtb_rv64i_pipe___024root___eval_initial__TOP__Vtiming__0(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "tb_rv64i_pipe.v", 
                                         37);
    vlSelfRef.tb_rv64i_pipe__DOT__rst = 0U;
    while ((((~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__halted)) 
             & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__tohost_we))) 
            & VL_GTS_III(32, 0x00030d40U, vlSelfRef.tb_rv64i_pipe__DOT__cycles))) {
        Vtb_rv64i_pipe___024root____VbeforeTrig_h3f920df9__0(vlSelf, 
                                                             "@(posedge tb_rv64i_pipe.clk)");
        co_await vlSelfRef.__VtrigSched_h3f920df9__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge tb_rv64i_pipe.clk)", 
                                                             "tb_rv64i_pipe.v", 
                                                             39);
        vlSelfRef.tb_rv64i_pipe__DOT__cycles = ((IData)(1U) 
                                                + vlSelfRef.tb_rv64i_pipe__DOT__cycles);
    }
    if (VL_UNLIKELY((vlSelfRef.tb_rv64i_pipe__DOT__tohost_we))) {
        vlSelfRef.tb_rv64i_pipe__DOT__pass = (1ULL 
                                              == vlSelfRef.tb_rv64i_pipe__DOT__tohost_data);
        VL_WRITEF_NX("[tohost] value=0x%016x\n",1, '#',64,vlSelfRef.tb_rv64i_pipe__DOT__tohost_data);
    } else if (vlSelfRef.tb_rv64i_pipe__DOT__halted) {
        vlSelfRef.tb_rv64i_pipe__DOT__pass = 0U;
        VL_WRITEF_NX("[halt-bez-tohost] cykli=%0d\n",1
                     , '~',32,vlSelfRef.tb_rv64i_pipe__DOT__cycles);
    } else {
        vlSelfRef.tb_rv64i_pipe__DOT__pass = 0U;
        VL_WRITEF_NX("[timeout] cykli=%0d\n",1, '~',32,vlSelfRef.tb_rv64i_pipe__DOT__cycles);
    }
    VL_WRITEF_NX("pc =0x%016x\nx10=0x%016x  x11=0x%016x\nx12=0x%016x  x13=0x%016x\n",5
                 , '#',64,vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__pc
                 , '#',64,vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[10U]
                 , '#',64,vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[11U]
                 , '#',64,vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[12U]
                 , '#',64,vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[13U]);
    if (vlSelfRef.tb_rv64i_pipe__DOT__pass) {
        VL_WRITEF_NX("*** PASS ***\n",0);
    } else {
        VL_WRITEF_NX("*** FAIL ***\n",0);
    }
    VL_FINISH_MT("tb_rv64i_pipe.v", 57, "");
    co_return;
}

VlCoroutine Vtb_rv64i_pipe___024root___eval_initial__TOP__Vtiming__1(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "tb_rv64i_pipe.v", 
                                             32);
        vlSelfRef.tb_rv64i_pipe__DOT__clk = (1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__clk)));
    }
    co_return;
}

bool Vtb_rv64i_pipe___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___trigger_anySet__ico\n"); );
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

bool Vtb_rv64i_pipe___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___trigger_anySet__act\n"); );
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

void Vtb_rv64i_pipe___024root___nba_sequent__TOP__0(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___nba_sequent__TOP__0\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__fwd_a_exmem;
    tb_rv64i_pipe__DOT__u_core__DOT__fwd_a_exmem = 0;
    CData/*0:0*/ tb_rv64i_pipe__DOT__u_core__DOT__fwd_b_exmem;
    tb_rv64i_pipe__DOT__u_core__DOT__fwd_b_exmem = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_13;
    __VdfgRegularize_h6e95ff9d_0_13 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_14;
    __VdfgRegularize_h6e95ff9d_0_14 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_15;
    __VdfgRegularize_h6e95ff9d_0_15 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_18;
    __VdfgRegularize_h6e95ff9d_0_18 = 0;
    QData/*63:0*/ __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__pc;
    __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__pc = 0;
    QData/*63:0*/ __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc;
    __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc = 0;
    CData/*0:0*/ __Vdly__tb_rv64i_pipe__DOT__halted;
    __Vdly__tb_rv64i_pipe__DOT__halted = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v0;
    __VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v0 = 0;
    QData/*63:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32;
    __VdlyVal__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 0;
    CData/*4:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32;
    __VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 0;
    QData/*63:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0 = 0;
    IData/*31:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1 = 0;
    IData/*31:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2 = 0;
    SData/*15:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3 = 0;
    SData/*15:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4 = 0;
    SData/*15:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5 = 0;
    SData/*15:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13 = 0;
    CData/*7:0*/ __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14;
    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14 = 0;
    SData/*14:0*/ __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14;
    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14 = 0;
    CData/*0:0*/ __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14 = 0;
    VlWide<4>/*127:0*/ __Vtemp_1;
    VlWide<4>/*127:0*/ __Vtemp_2;
    VlWide<4>/*127:0*/ __Vtemp_3;
    VlWide<3>/*95:0*/ __Vtemp_4;
    VlWide<4>/*127:0*/ __Vtemp_5;
    VlWide<4>/*127:0*/ __Vtemp_6;
    VlWide<4>/*127:0*/ __Vtemp_7;
    // Body
    __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__pc = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__pc;
    __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc 
        = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc;
    __Vdly__tb_rv64i_pipe__DOT__halted = vlSelfRef.tb_rv64i_pipe__DOT__halted;
    __VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v0 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13 = 0U;
    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14 = 0U;
    vlSelfRef.tb_rv64i_pipe__DOT__tohost_we = 0U;
    if (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_we) {
        if ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size))) {
            if ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size))) {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0 
                    = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val;
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0 = 1U;
            } else if ((1U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                      >> 2U)))) {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1 
                    = (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val);
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1 = 1U;
            } else {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2 
                    = (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val);
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2 = 1U;
            }
        } else if ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size))) {
            if ((1U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                               >> 2U)))) {
                if ((1U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                   >> 1U)))) {
                    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3 
                        = (0x0000ffffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3 
                        = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                   - 0x0000000080000000ULL) 
                                                  >> 3U)));
                    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3 = 1U;
                } else {
                    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4 
                        = (0x0000ffffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4 
                        = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                   - 0x0000000080000000ULL) 
                                                  >> 3U)));
                    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4 = 1U;
                }
            } else if ((1U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                      >> 1U)))) {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5 
                    = (0x0000ffffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5 = 1U;
            } else {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6 
                    = (0x0000ffffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6 = 1U;
            }
        } else if ((1U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                  >> 2U)))) {
            if ((1U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                               >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y))) {
                    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7 
                        = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7 
                        = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                   - 0x0000000080000000ULL) 
                                                  >> 3U)));
                    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7 = 1U;
                } else {
                    __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8 
                        = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                    __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8 
                        = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                   - 0x0000000080000000ULL) 
                                                  >> 3U)));
                    __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8 = 1U;
                }
            } else if ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y))) {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9 
                    = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9 = 1U;
            } else {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10 
                    = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10 = 1U;
            }
        } else if ((1U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                  >> 1U)))) {
            if ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y))) {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11 
                    = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11 = 1U;
            } else {
                __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12 
                    = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
                __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12 
                    = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                               - 0x0000000080000000ULL) 
                                              >> 3U)));
                __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12 = 1U;
            }
        } else if ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y))) {
            __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13 
                = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
            __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13 
                = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                           - 0x0000000080000000ULL) 
                                          >> 3U)));
            __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13 = 1U;
        } else {
            __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14 
                = (0x000000ffU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val));
            __VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14 
                = (0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                           - 0x0000000080000000ULL) 
                                          >> 3U)));
            __VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14 = 1U;
        }
        if ((0x0000000080001000ULL == vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y)) {
            vlSelfRef.tb_rv64i_pipe__DOT__tohost_we = 1U;
            vlSelfRef.tb_rv64i_pipe__DOT__tohost_data 
                = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val;
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__rst)))) {
        if ((1U & (~ ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush) 
                      | (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard))))) {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op 
                = (((0x00004000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                     ? ((0x00002000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                         ? ((0x00001000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                             ? 6U : 5U) : ((0x00001000U 
                                            & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                            ? 4U : 3U))
                     : (((0x00001000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                          ? 2U : 1U) & (- (IData)((1U 
                                                   & (~ 
                                                      (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                       >> 0x0000000dU))))))) 
                   & (- (IData)((IData)(((0U == (0x0000001cU 
                                                 & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                                         & (3U == (3U 
                                                   & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                      & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                         >> 5U)))))))));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_a_pc 
                = (IData)(((0x00000010U == (0x00000070U 
                                            & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                           & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11)));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_b_imm 
                = ((0x00000040U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                    ? ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                        >> 5U) & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34))
                    : ((0x00000020U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                        ? ((0x00000010U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                            ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11)
                            : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6))
                        : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_22)));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2_val 
                = ((0U == (0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                          >> 0x00000014U)))
                    ? 0ULL : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs
                   [(0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                    >> 0x00000014U))]);
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1_val 
                = ((0U == (0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                          >> 0x0000000fU)))
                    ? 0ULL : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs
                   [(0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                    >> 0x0000000fU))]);
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2 
                = (0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                  >> 0x00000014U));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1 
                = (0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                  >> 0x0000000fU));
        }
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_unsigned 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_unsigned;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_size 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_re 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_re;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_is_jump 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_is_jump;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_alu_y 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data 
            = ((3U == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size))
                ? vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
               [(0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                         - 0x0000000080000000ULL) 
                                        >> 3U)))] : 
               ((2U == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size))
                 ? (QData)((IData)(((1U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                   >> 2U)))
                                     ? (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                [(0x00007fffU 
                                                  & (IData)(
                                                            ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                              - 0x0000000080000000ULL) 
                                                             >> 3U)))] 
                                                >> 0x00000020U))
                                     : (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                               [(0x00007fffU 
                                                 & (IData)(
                                                           ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                             - 0x0000000080000000ULL) 
                                                            >> 3U)))]))))
                 : ((1U == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size))
                     ? ((0U == (3U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                              >> 1U))))
                         ? (QData)((IData)((0x0000ffffU 
                                            & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                      [
                                                      (0x00007fffU 
                                                       & (IData)(
                                                                 ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                   - 0x0000000080000000ULL) 
                                                                  >> 3U)))]))))
                         : ((1U == (3U & (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                  >> 1U))))
                             ? (QData)((IData)((0x0000ffffU 
                                                & (IData)(
                                                          (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                           [
                                                           (0x00007fffU 
                                                            & (IData)(
                                                                      ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                        - 0x0000000080000000ULL) 
                                                                       >> 3U)))] 
                                                           >> 0x00000010U)))))
                             : ((2U == (3U & (IData)(
                                                     (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                      >> 1U))))
                                 ? (QData)((IData)(
                                                   (0x0000ffffU 
                                                    & (IData)(
                                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                               [
                                                               (0x00007fffU 
                                                                & (IData)(
                                                                          ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                            - 0x0000000080000000ULL) 
                                                                           >> 3U)))] 
                                                               >> 0x00000020U)))))
                                 : (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                    [(0x00007fffU & (IData)(
                                                            ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                              - 0x0000000080000000ULL) 
                                                             >> 3U)))] 
                                    >> 0x00000030U))))
                     : ((0U == (7U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y)))
                         ? (QData)((IData)((0x000000ffU 
                                            & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                      [
                                                      (0x00007fffU 
                                                       & (IData)(
                                                                 ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                   - 0x0000000080000000ULL) 
                                                                  >> 3U)))]))))
                         : ((1U == (7U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y)))
                             ? (QData)((IData)((0x000000ffU 
                                                & (IData)(
                                                          (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                           [
                                                           (0x00007fffU 
                                                            & (IData)(
                                                                      ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                        - 0x0000000080000000ULL) 
                                                                       >> 3U)))] 
                                                           >> 8U)))))
                             : ((2U == (7U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y)))
                                 ? (QData)((IData)(
                                                   (0x000000ffU 
                                                    & (IData)(
                                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                               [
                                                               (0x00007fffU 
                                                                & (IData)(
                                                                          ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                            - 0x0000000080000000ULL) 
                                                                           >> 3U)))] 
                                                               >> 0x00000010U)))))
                                 : ((3U == (7U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y)))
                                     ? (QData)((IData)(
                                                       (0x000000ffU 
                                                        & (IData)(
                                                                  (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                                   [
                                                                   (0x00007fffU 
                                                                    & (IData)(
                                                                              ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                               >> 3U)))] 
                                                                   >> 0x00000018U)))))
                                     : ((4U == (7U 
                                                & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y)))
                                         ? (QData)((IData)(
                                                           (0x000000ffU 
                                                            & (IData)(
                                                                      (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                                       [
                                                                       (0x00007fffU 
                                                                        & (IData)(
                                                                                ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                       >> 0x00000020U)))))
                                         : ((5U == 
                                             (7U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y)))
                                             ? (QData)((IData)(
                                                               (0x000000ffU 
                                                                & (IData)(
                                                                          (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                                           [
                                                                           (0x00007fffU 
                                                                            & (IData)(
                                                                                ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                           >> 0x00000028U)))))
                                             : ((6U 
                                                 == 
                                                 (7U 
                                                  & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y)))
                                                 ? (QData)((IData)(
                                                                   (0x000000ffU 
                                                                    & (IData)(
                                                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                                               [
                                                                               (0x00007fffU 
                                                                                & (IData)(
                                                                                ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 3U)))] 
                                                                               >> 0x00000030U)))))
                                                 : 
                                                (vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                                                 [(0x00007fffU 
                                                   & (IData)(
                                                             ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                                                               - 0x0000000080000000ULL) 
                                                              >> 3U)))] 
                                                 >> 0x00000038U)))))))))));
    }
    if (vlSelfRef.tb_rv64i_pipe__DOT__rst) {
        __VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v0 = 1U;
    } else if (((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_reg_we) 
                & (0U != (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd)))) {
        __VdlyVal__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__wb_wdata;
        __VdlyDim0__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd;
        __VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 1U;
    }
    if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__rst)))) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_unsigned 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_unsigned;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_re 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_re;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_size;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2;
        __Vtemp_1[0U] = (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s);
        __Vtemp_1[1U] = (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s 
                                 >> 0x00000020U));
        __Vtemp_1[2U] = 0U;
        __Vtemp_1[3U] = 0U;
        __Vtemp_2[0U] = (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s);
        __Vtemp_2[1U] = (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                 >> 0x00000020U));
        __Vtemp_2[2U] = 0U;
        __Vtemp_2[3U] = 0U;
        VL_MUL_W(4, __Vtemp_3, __Vtemp_1, __Vtemp_2);
        __Vtemp_4[0U] = (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s);
        __Vtemp_4[1U] = (IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s 
                                 >> 0x00000020U));
        __Vtemp_4[2U] = 0U;
        VL_EXTENDS_WW(128,65, __Vtemp_5, __Vtemp_4);
        VL_MULS_WWW(128, __Vtemp_6, vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16, __Vtemp_5);
        if (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_jump) {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                = (4ULL + vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_pc);
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_is_jump = 1U;
        } else {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y 
                = ((0x00000010U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                    ? ((8U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                        ? ((4U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                            ? ((- (QData)((IData)((1U 
                                                   & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op)))))) 
                               & ((((QData)((IData)(
                                                    (- (IData)(
                                                               (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remuw_r 
                                                                >> 0x0000001fU))))) 
                                    << 0x00000020U) 
                                   | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remuw_r))) 
                                  & (- (QData)((IData)(
                                                       (1U 
                                                        & (~ 
                                                           ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op) 
                                                            >> 1U))))))))
                            : ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                ? ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (((QData)((IData)(
                                                        (- (IData)(
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remw_s 
                                                                    >> 0x0000001fU))))) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remw_s)))
                                    : (((QData)((IData)(
                                                        (- (IData)(
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divuw_r 
                                                                    >> 0x0000001fU))))) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divuw_r))))
                                : ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (((QData)((IData)(
                                                        (- (IData)(
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divw_s 
                                                                    >> 0x0000001fU))))) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divw_s)))
                                    : (((QData)((IData)(
                                                        (- (IData)(
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mulw_full 
                                                                    >> 0x0000001fU))))) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mulw_full))))))
                        : ((4U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                            ? ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                ? ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? ((0ULL == vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)
                                        ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s
                                        : VL_MODDIV_QQQ(64, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))
                                    : ((0ULL == vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)
                                        ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s
                                        : (VL_MODDIVS_QQQ(64, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s) 
                                           & (- (QData)((IData)(
                                                                (1U 
                                                                 & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17)))))))))
                                : ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (VL_DIV_QQQ(64, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s) 
                                       | (- (QData)((IData)(
                                                            (0ULL 
                                                             == vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)))))
                                    : (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17)
                                         ? 0x8000000000000000ULL
                                         : VL_DIVS_QQQ(64, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)) 
                                       | (- (QData)((IData)(
                                                            (0ULL 
                                                             == vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)))))))
                            : ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                ? ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (((QData)((IData)(__Vtemp_3[3U])) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(__Vtemp_3[2U])))
                                    : (((QData)((IData)(__Vtemp_6[3U])) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(__Vtemp_6[2U]))))
                                : ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (((QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mul_ss[3U])) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mul_ss[2U])))
                                    : (((QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mul_ss[1U])) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mul_ss[0U])))))))
                    : ((8U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                        ? ((4U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                            ? ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                ? ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (((QData)((IData)(
                                                        (- (IData)(
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sraw_full 
                                                                    >> 0x0000001fU))))) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sraw_full)))
                                    : (QData)((IData)(
                                                      ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s) 
                                                       >> 
                                                       (0x0000001fU 
                                                        & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))))))
                                : ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (((QData)((IData)(
                                                        (- (IData)(
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sllw_full 
                                                                    >> 0x0000001fU))))) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sllw_full)))
                                    : (((QData)((IData)(
                                                        (- (IData)(
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__subw_full 
                                                                    >> 0x0000001fU))))) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__subw_full)))))
                            : ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                ? ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (((QData)((IData)(
                                                        (- (IData)(
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__addw_full 
                                                                    >> 0x0000001fU))))) 
                                        << 0x00000020U) 
                                       | (QData)((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__addw_full)))
                                    : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)
                                : ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                       & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)
                                    : (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                       | vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))))
                        : ((4U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                            ? ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                ? ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? VL_SHIFTRS_QQI(64,64,6, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s, 
                                                     (0x0000003fU 
                                                      & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)))
                                    : (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                       >> (0x0000003fU 
                                           & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))))
                                : ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                       ^ vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)
                                    : (1ULL & (- (QData)((IData)(
                                                                 (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                                  < vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)))))))
                            : ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                ? ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (1ULL & (- (QData)((IData)(
                                                                 VL_LTS_IQQ(64, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)))))
                                    : (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                       << (0x0000003fU 
                                           & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))))
                                : ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op))
                                    ? (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                       - vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)
                                    : (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
                                       + vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))))));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_is_jump = 0U;
        }
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd 
            = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rd;
        if ((1U & (~ ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush) 
                      | (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard))))) {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_unsigned 
                = (IData)((0x00004003U == (0x0000407fU 
                                           & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_size 
                = (3U & (((0x00000020U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                           ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_20) 
                              | ((- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 3U)))) 
                                 | ((- (IData)((1U 
                                                & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 2U)))) 
                                    | ((- (IData)((1U 
                                                   & (~ 
                                                      (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                       >> 1U))))) 
                                       | ((- (IData)(
                                                     (1U 
                                                      & (~ vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)))) 
                                          | ((- (IData)(
                                                        (1U 
                                                         & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                            >> 4U)))) 
                                             | (- (IData)(
                                                          (1U 
                                                           & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                              >> 0x0000000eU))))))))))
                           : ((- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                >> 3U)))) 
                              | ((- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 2U)))) 
                                 | ((- (IData)((1U 
                                                & (~ 
                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                    >> 1U))))) 
                                    | ((- (IData)((1U 
                                                   & (~ vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)))) 
                                       | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_20) 
                                          | (- (IData)(
                                                       (1U 
                                                        & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                           >> 4U)))))))))) 
                         | (- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                             >> 6U))))));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op 
                = (((0x00000020U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                     ? (((8U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                          ? ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8))) 
                             & ((0U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                 ? 0x0bU : ((0x0100U 
                                             == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                             ? 0x0cU
                                             : ((1U 
                                                 == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                                 ? 0x0dU
                                                 : 
                                                ((5U 
                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                                  ? 0x0eU
                                                  : 
                                                 ((0x0105U 
                                                   == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                                   ? 0x0fU
                                                   : 
                                                  ((8U 
                                                    == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                                    ? 0x18U
                                                    : 
                                                   ((0x000cU 
                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                                     ? 0x19U
                                                     : 
                                                    ((0x000dU 
                                                      == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                                      ? 0x1aU
                                                      : 
                                                     ((0x000eU 
                                                       == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))
                                                       ? 0x1bU
                                                       : 
                                                      (0x1cU 
                                                       & (- (IData)(
                                                                    (0x000fU 
                                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)))))))))))))))
                          : ((4U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                              ? (0x0aU & (- (IData)(
                                                    (3U 
                                                     == 
                                                     (3U 
                                                      & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)))))
                              : ((- (IData)((1U & (~ 
                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                    >> 0x0000001fU))))) 
                                 & (((0x40000000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                      ? ((- (IData)(
                                                    (1U 
                                                     & (~ 
                                                        (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                         >> 0x0000001cU))))) 
                                         & ((- (IData)(
                                                       (1U 
                                                        & (~ 
                                                           (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                            >> 0x0000001bU))))) 
                                            & ((- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                               >> 0x0000001aU))))) 
                                               & ((- (IData)(
                                                             (1U 
                                                              & (~ 
                                                                 (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                  >> 0x00000019U))))) 
                                                  & (((0x00004000U 
                                                       & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                       ? 
                                                      (7U 
                                                       & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_31))))
                                                       : 
                                                      (1U 
                                                       & ((- (IData)(
                                                                     (1U 
                                                                      & (~ 
                                                                         (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                          >> 0x0000000cU))))) 
                                                          & (- (IData)(
                                                                       (1U 
                                                                        & (~ 
                                                                           (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                            >> 0x0000000dU)))))))) 
                                                     & (- (IData)(
                                                                  (1U 
                                                                   & (~ 
                                                                      (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                       >> 0x0000001dU))))))))))
                                      : ((- (IData)(
                                                    (1U 
                                                     & (~ 
                                                        (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                         >> 0x0000001cU))))) 
                                         & ((- (IData)(
                                                       (1U 
                                                        & (~ 
                                                           (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                            >> 0x0000001bU))))) 
                                            & ((- (IData)(
                                                          (1U 
                                                           & (~ 
                                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                               >> 0x0000001aU))))) 
                                               & (((0x02000000U 
                                                    & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                    ? 
                                                   ((0x00004000U 
                                                     & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                     ? 
                                                    ((0x00002000U 
                                                      & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                      ? 
                                                     ((0x00001000U 
                                                       & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                       ? 0x17U
                                                       : 0x16U)
                                                      : 
                                                     ((0x00001000U 
                                                       & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                       ? 0x15U
                                                       : 0x14U))
                                                     : 
                                                    ((0x00002000U 
                                                      & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                      ? 
                                                     ((0x00001000U 
                                                       & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                       ? 0x13U
                                                       : 0x12U)
                                                      : 
                                                     ((0x00001000U 
                                                       & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                       ? 0x11U
                                                       : 0x10U)))
                                                    : 
                                                   ((0x00004000U 
                                                     & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                     ? 
                                                    ((0x00002000U 
                                                      & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                      ? 
                                                     ((0x00001000U 
                                                       & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                       ? 9U
                                                       : 8U)
                                                      : 
                                                     ((0x00001000U 
                                                       & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                       ? 6U
                                                       : 5U))
                                                     : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_23))) 
                                                  & (- (IData)(
                                                               (1U 
                                                                & (~ 
                                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                    >> 0x0000001dU)))))))))) 
                                    & (- (IData)((3U 
                                                  == 
                                                  (3U 
                                                   & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)))))))) 
                        & (- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                            >> 4U)))))
                     : (((8U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                          ? (((0U == (7U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                            >> 0x0000000cU)))
                               ? 0x0bU : ((1U == (7U 
                                                  & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                     >> 0x0000000cU)))
                                           ? 0x0dU : 
                                          (((0U == 
                                             (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                              >> 0x00000019U))
                                             ? 0x0eU
                                             : (0x0fU 
                                                & (- (IData)(
                                                             (0x20U 
                                                              == 
                                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                               >> 0x00000019U)))))) 
                                           & (- (IData)(
                                                        (5U 
                                                         == 
                                                         (7U 
                                                          & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                             >> 0x0000000cU)))))))) 
                             & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8))))
                          : ((- (IData)((1U & (~ (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                  >> 2U))))) 
                             & (((0x00004000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                  ? ((0x00002000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                      ? ((0x00001000U 
                                          & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                          ? 9U : 8U)
                                      : ((0x00001000U 
                                          & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                          ? ((0U == 
                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                               >> 0x0000001aU))
                                              ? 6U : 
                                             (7U & 
                                              (- (IData)(
                                                         (0x10U 
                                                          == 
                                                          (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                           >> 0x0000001aU))))))
                                          : 5U)) : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_23)) 
                                & (- (IData)((3U == 
                                              (3U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))))))) 
                        & (- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                            >> 4U)))))) 
                   & (- (IData)((1U & (~ (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                          >> 6U))))));
        }
    }
    if (vlSelfRef.tb_rv64i_pipe__DOT__rst) {
        __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__pc = 0x0000000080000000ULL;
        __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc = 0ULL;
        __Vdly__tb_rv64i_pipe__DOT__halted = 0U;
    } else {
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_we 
            = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_we) 
               & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__halted)));
        if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard)))) {
            if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush)))) {
                __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc 
                    = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__pc;
            }
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__halted)))) {
            if (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_redirect) {
                __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__pc 
                    = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_pc_sel_alu)
                        ? (0xfffffffffffffffeULL & 
                           (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_imm 
                            + vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1))
                        : (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_pc 
                           + vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_imm));
            } else if (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_halt) {
                __Vdly__tb_rv64i_pipe__DOT__halted = 1U;
            } else if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard)))) {
                __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__pc 
                    = (4ULL + vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__pc);
            }
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__rst)))) {
        if ((1U & (~ ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush) 
                      | (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard))))) {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_pc_sel_alu 
                = (IData)(((0x00000060U == (0x00000060U 
                                            & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                           & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34)));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_imm 
                = ((0x00000040U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                    ? ((- (QData)((IData)((1U & (~ 
                                                 (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                  >> 4U)))))) 
                       & (((8U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                            ? ((((- (QData)((IData)(
                                                    (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                     >> 0x0000001fU)))) 
                                 << 0x00000015U) | (QData)((IData)(
                                                                   ((((0x00000200U 
                                                                       & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                          >> 0x00000016U)) 
                                                                      | ((0x000001feU 
                                                                          & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                             >> 0x0000000bU)) 
                                                                         | (1U 
                                                                            & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                               >> 0x00000014U)))) 
                                                                     << 0x0000000bU) 
                                                                    | (0x000007feU 
                                                                       & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                          >> 0x00000014U)))))) 
                               & (- (QData)((IData)(
                                                    (7U 
                                                     == 
                                                     (7U 
                                                      & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))))))
                            : ((4U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                ? vlSelfRef.__VdfgRegularize_h6e95ff9d_0_26
                                : ((((- (QData)((IData)(
                                                        (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                         >> 0x0000001fU)))) 
                                     << 0x0000000dU) 
                                    | (QData)((IData)(
                                                      ((((2U 
                                                          & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                             >> 0x0000001eU)) 
                                                         | (1U 
                                                            & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                               >> 7U))) 
                                                        << 0x0000000bU) 
                                                       | ((0x000007e0U 
                                                           & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                              >> 0x00000014U)) 
                                                          | (0x0000001eU 
                                                             & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                >> 7U))))))) 
                                   & (- (QData)((IData)(
                                                        (3U 
                                                         == 
                                                         (3U 
                                                          & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)))))))) 
                          & (- (QData)((IData)((1U 
                                                & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 5U)))))))
                    : ((0x00000020U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                        ? ((0x00000010U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                            ? ((- (QData)((IData)((4U 
                                                   == 
                                                   (0x0000000cU 
                                                    & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))))) 
                               & vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25)
                            : ((((- (QData)((IData)(
                                                    (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                     >> 0x0000001fU)))) 
                                 << 0x0000000cU) | (QData)((IData)(
                                                                   ((0x00000fe0U 
                                                                     & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                        >> 0x00000014U)) 
                                                                    | (0x0000001fU 
                                                                       & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                          >> 7U)))))) 
                               & (- (QData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)))))
                        : ((0x00000010U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                            ? ((8U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                ? (((0U == (7U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                  >> 0x0000000cU)))
                                     ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_dec__DOT__imm_i
                                     : ((1U == (7U 
                                                & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 0x0000000cU)))
                                         ? (QData)((IData)(
                                                           (0x0000001fU 
                                                            & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                               >> 0x00000014U))))
                                         : ((5U == 
                                             (7U & 
                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                               >> 0x0000000cU)))
                                             ? (QData)((IData)(
                                                               (0x0000001fU 
                                                                & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                   >> 0x00000014U))))
                                             : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_dec__DOT__imm_i))) 
                                   & (- (QData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8))))
                                : ((4U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                    ? vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25
                                    : (((0x00002000U 
                                         & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                         ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_dec__DOT__imm_i
                                         : ((0x00001000U 
                                             & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                             ? (QData)((IData)(
                                                               (0x0000003fU 
                                                                & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                   >> 0x00000014U))))
                                             : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_dec__DOT__imm_i)) 
                                       & (- (QData)((IData)(
                                                            (3U 
                                                             == 
                                                             (3U 
                                                              & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))))))))
                            : (vlSelfRef.__VdfgRegularize_h6e95ff9d_0_26 
                               & ((- (QData)((IData)(
                                                     (1U 
                                                      & (~ 
                                                         (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                          >> 3U)))))) 
                                  & (- (QData)((IData)(
                                                       (1U 
                                                        & (~ 
                                                           (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                            >> 2U)))))))))));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rd 
                = (0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                  >> 7U));
        }
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v0) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[0U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[1U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[2U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[3U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[4U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[5U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[6U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[7U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[8U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[9U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[10U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[11U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[12U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[13U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[14U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[15U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[16U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[17U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[18U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[19U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[20U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[21U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[22U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[23U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[24U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[25U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[26U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[27U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[28U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[29U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[30U] = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[31U] = 0ULL;
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[__VdlyDim0__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32] 
            = __VdlyVal__tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs__v32;
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0] 
            = __VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v0;
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1] 
            = ((0x00000000ffffffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v1)) 
                  << 0x00000020U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2] 
            = ((0xffffffff00000000ULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2]) 
               | (IData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v2)));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3] 
            = ((0x0000ffffffffffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v3)) 
                  << 0x00000030U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4] 
            = ((0xffff0000ffffffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v4)) 
                  << 0x00000020U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5] 
            = ((0xffffffff0000ffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v5)) 
                  << 0x00000010U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6] 
            = ((0xffffffffffff0000ULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6]) 
               | (IData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v6)));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7] 
            = ((0x00ffffffffffffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v7)) 
                  << 0x00000038U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8] 
            = ((0xff00ffffffffffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v8)) 
                  << 0x00000030U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9] 
            = ((0xffff00ffffffffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v9)) 
                  << 0x00000028U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10] 
            = ((0xffffff00ffffffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v10)) 
                  << 0x00000020U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11] 
            = ((0xffffffff00ffffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v11)) 
                  << 0x00000018U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12] 
            = ((0xffffffffff00ffffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v12)) 
                  << 0x00000010U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13] 
            = ((0xffffffffffff00ffULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13]) 
               | ((QData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v13)) 
                  << 8U));
    }
    if (__VdlySet__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14] 
            = ((0xffffffffffffff00ULL & vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__dmem
                [__VdlyDim0__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14]) 
               | (IData)((IData)(__VdlyVal__tb_rv64i_pipe__DOT__u_mem__DOT__dmem__v14)));
    }
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_reg_we 
        = ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__rst))) 
           && (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_reg_we));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__wb_wdata 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_is_jump)
            ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_alu_y
            : ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_re)
                ? ((((0U == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_size))
                      ? ((- (QData)((IData)((1U & (IData)(
                                                          (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data 
                                                           >> 7U)))))) 
                         & (- (QData)((IData)((1U & 
                                               (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_unsigned)))))))
                      : ((0x00ffffffffffff00ULL & (
                                                   ((1U 
                                                     == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_size))
                                                     ? 
                                                    ((- (QData)((IData)(
                                                                        (1U 
                                                                         & (IData)(
                                                                                (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data 
                                                                                >> 0x0000000fU)))))) 
                                                     & (- (QData)((IData)(
                                                                          (1U 
                                                                           & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_unsigned)))))))
                                                     : 
                                                    (((QData)((IData)(
                                                                      ((2U 
                                                                        == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_size))
                                                                        ? 
                                                                       ((- (IData)(
                                                                                (1U 
                                                                                & (IData)(
                                                                                (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data 
                                                                                >> 0x0000001fU))))) 
                                                                        & (- (IData)(
                                                                                (1U 
                                                                                & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_unsigned))))))
                                                                        : (IData)(
                                                                                (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data 
                                                                                >> 0x00000020U))))) 
                                                      << 0x00000010U) 
                                                     | (QData)((IData)(
                                                                       (0x0000ffffU 
                                                                        & (IData)(
                                                                                (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data 
                                                                                >> 0x00000010U))))))) 
                                                   << 8U)) 
                         | (QData)((IData)((0x000000ffU 
                                            & (IData)(
                                                      (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data 
                                                       >> 8U))))))) 
                    << 8U) | (QData)((IData)((0x000000ffU 
                                              & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data)))))
                : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_alu_y));
    vlSelfRef.tb_rv64i_pipe__DOT__halted = __Vdly__tb_rv64i_pipe__DOT__halted;
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_reg_we 
        = ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__rst))) 
           && (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_reg_we));
    if (vlSelfRef.tb_rv64i_pipe__DOT__rst) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_pc = 0ULL;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_reg_we = 0U;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid = 0U;
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr = 0U;
    } else {
        if ((1U & (~ ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush) 
                      | (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard))))) {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_pc 
                = vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc;
        }
        if (((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush) 
             | (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard))) {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_reg_we = 0U;
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_re = 0U;
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_we = 0U;
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_halt = 0U;
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_jump = 0U;
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_branch = 0U;
        } else {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_reg_we 
                = (((0x00000040U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                     ? ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                         >> 5U) & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38))
                     : ((0x00000020U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                         ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21) 
                            & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                               >> 4U)) : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_22))) 
                   & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_re 
                = (IData)((((0U == (0x00000060U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12)) 
                           & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid)));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_we 
                = (IData)((((0x00000020U == (0x00000060U 
                                             & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12)) 
                           & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid)));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_halt 
                = ((((0x00000040U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                      ? ((~ (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                             >> 5U)) | ((0x00000010U 
                                         & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                         ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                            | ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)) 
                                               | (0U 
                                                  != 
                                                  (3U 
                                                   & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                      >> 2U)))))
                                         : ((8U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                             ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_36)
                                             : ((4U 
                                                 & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                 ? 
                                                (~ (IData)(
                                                           (3U 
                                                            == 
                                                            (0x00007003U 
                                                             & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))))
                                                 : 
                                                ((IData)(
                                                         (0x00002000U 
                                                          == 
                                                          (0x00006000U 
                                                           & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))) 
                                                 | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))))))
                      : ((0x00000020U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                          ? ((0x00000010U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                              ? ((8U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                  ? ((~ ((3U == (3U 
                                                 & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                                         & ((0U == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                            | ((0x0100U 
                                                == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                               | ((1U 
                                                   == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                                  | ((5U 
                                                      == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                                     | ((0x0105U 
                                                         == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                                        | ((8U 
                                                            == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                                           | ((0x000cU 
                                                               == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                                              | ((0x000dU 
                                                                  == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                                                 | ((0x000eU 
                                                                     == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0)) 
                                                                    | (0x000fU 
                                                                       == (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0))))))))))))) 
                                     | (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                        >> 2U)) : (
                                                   (4U 
                                                    & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                    ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                    : 
                                                   ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                                    | ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                        >> 0x0000001fU) 
                                                       | ((0x40000000U 
                                                           & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                           ? 
                                                          (1U 
                                                           & ((0U 
                                                               != 
                                                               (0x3e000000U 
                                                                & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                                                              | ((0x00004000U 
                                                                  & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                                  ? (IData)(
                                                                            (0x00001000U 
                                                                             != 
                                                                             (0x00003000U 
                                                                              & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)))
                                                                  : 
                                                                 (0U 
                                                                  != 
                                                                  (3U 
                                                                   & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                      >> 0x0000000cU))))))
                                                           : 
                                                          (0U 
                                                           != 
                                                           (0x0000000fU 
                                                            & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                               >> 0x0000001aU))))))))
                              : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                 | (0U != (0x0000400cU 
                                           & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))))
                          : ((0x00000010U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                              ? ((8U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                  ? ((~ ((3U == (3U 
                                                 & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                                         & ((0U == 
                                             (7U & 
                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                               >> 0x0000000cU))) 
                                            | ((1U 
                                                == 
                                                (7U 
                                                 & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                    >> 0x0000000cU)))
                                                ? (0U 
                                                   == 
                                                   (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                    >> 0x00000019U))
                                                : (IData)(
                                                          ((0x00005000U 
                                                            == 
                                                            (0x00007000U 
                                                             & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                                                           & ((0U 
                                                               == 
                                                               (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                >> 0x00000019U)) 
                                                              | (0x20U 
                                                                 == 
                                                                 (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                  >> 0x00000019U))))))))) 
                                     | (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                        >> 2U)) : (
                                                   (4U 
                                                    & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                    ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                    : 
                                                   (((0x00004000U 
                                                      & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                      ? 
                                                     ((0x10U 
                                                       != 
                                                       (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                        >> 0x0000001aU)) 
                                                      & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_37))
                                                      : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_37)) 
                                                    | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))))
                              : ((8U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                  ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_36)
                                  : ((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                      >> 2U) | ((IData)(
                                                        (0x00007000U 
                                                         == 
                                                         (0x00007000U 
                                                          & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))) 
                                                | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))))))) 
                    | (IData)((((0x00000070U == (0x00000070U 
                                                 & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                                & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)) 
                               & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)))) 
                   & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_jump 
                = (IData)((((0x00000060U == (0x00000060U 
                                             & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38)) 
                           & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid)));
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_branch 
                = (IData)((((0x00000060U == (0x00000060U 
                                             & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12)) 
                           & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid)));
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard)))) {
            vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid 
                = (1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush)));
            if ((1U & (~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush)))) {
                vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                    = vlSelfRef.tb_rv64i_pipe__DOT__u_mem__DOT__imem
                    [(0x00007fffU & (IData)(((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__pc 
                                              - 0x0000000080000000ULL) 
                                             >> 2U)))];
            }
        }
    }
    __VdfgRegularize_h6e95ff9d_0_15 = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_reg_we) 
                                       & (0U != (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd)));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc 
        = __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc;
    __VdfgRegularize_h6e95ff9d_0_14 = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_reg_we) 
                                       & (0U != (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd)));
    tb_rv64i_pipe__DOT__u_core__DOT__fwd_b_exmem = 
        (((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd) 
          == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2)) 
         & (IData)(__VdfgRegularize_h6e95ff9d_0_14));
    tb_rv64i_pipe__DOT__u_core__DOT__fwd_a_exmem = 
        (((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd) 
          == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1)) 
         & (IData)(__VdfgRegularize_h6e95ff9d_0_14));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2 
        = ((IData)(tb_rv64i_pipe__DOT__u_core__DOT__fwd_b_exmem)
            ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y
            : (((IData)(__VdfgRegularize_h6e95ff9d_0_15) 
                & ((~ (IData)(tb_rv64i_pipe__DOT__u_core__DOT__fwd_b_exmem)) 
                   & ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2) 
                      == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd))))
                ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__wb_wdata
                : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2_val));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1 
        = ((IData)(tb_rv64i_pipe__DOT__u_core__DOT__fwd_a_exmem)
            ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y
            : (((IData)(__VdfgRegularize_h6e95ff9d_0_15) 
                & ((~ (IData)(tb_rv64i_pipe__DOT__u_core__DOT__fwd_a_exmem)) 
                   & ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1) 
                      == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd))))
                ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__wb_wdata
                : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1_val));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_b_imm)
            ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_imm
            : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2);
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_redirect 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_jump) 
           | ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_is_branch) 
              & ((4U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op))
                  ? ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op))
                      ? ((~ (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op)) 
                         & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1 
                            >= vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2))
                      : ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op))
                          ? (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1 
                             < vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2)
                          : VL_GTES_IQQ(64, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2)))
                  : ((2U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op))
                      ? ((1U & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op))
                          ? VL_LTS_IQQ(64, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2)
                          : (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1 
                             != vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2))
                      : ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op) 
                         & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1 
                            == vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2))))));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_a_pc)
            ? vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_pc
            : vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1);
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__pc = __Vdly__tb_rv64i_pipe__DOT__u_core__DOT__pc;
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__flush 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__halted) 
           | (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ex_redirect));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__addw_full 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s) 
           + (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__subw_full 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s) 
           - (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sllw_full 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s) 
           << (0x0000001fU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sraw_full 
        = VL_SHIFTRS_III(32,32,5, (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s), 
                         (0x0000001fU & (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mulw_full 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s) 
           * (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divuw_r 
        = (VL_DIV_III(32, (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)) 
           | (- (IData)((0U == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17 = ((0x8000000000000000ULL 
                                                  == vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s) 
                                                 & (0xffffffffffffffffULL 
                                                    == vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s));
    VL_EXTENDS_WQ(128,64, vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s);
    __VdfgRegularize_h6e95ff9d_0_18 = ((0x80000000U 
                                        == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s)) 
                                       & (0xffffffffU 
                                          == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)));
    if ((0U == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))) {
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remuw_r 
            = (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s);
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remw_s 
            = (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s);
    } else {
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remuw_r 
            = VL_MODDIV_III(32, (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s));
        vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remw_s 
            = (VL_MODDIVS_III(32, (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)) 
               & (- (IData)((1U & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_18))))));
    }
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0 = ((0x000003f8U 
                                                 & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                    >> 0x00000016U)) 
                                                | (7U 
                                                   & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                      >> 0x0000000cU)));
    if ((0x00002000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_20 = 
            (3U & (2U | (- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                          >> 0x0000000cU))))));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_23 = 
            ((0x00001000U & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
              ? 4U : 3U);
    } else {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_20 = 
            (3U & (1U & (- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                          >> 0x0000000cU))))));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_23 = 
            (2U & (- (IData)((1U & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                    >> 0x0000000cU)))));
    }
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25 = ((((QData)((IData)(
                                                                   (- (IData)(
                                                                              (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                               >> 0x0000001fU))))) 
                                                   << 0x00000020U) 
                                                  | (QData)((IData)(
                                                                    (0xfffff000U 
                                                                     & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)))) 
                                                 & (- (QData)((IData)(
                                                                      (3U 
                                                                       == 
                                                                       (3U 
                                                                        & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35 = (IData)(
                                                        ((0U 
                                                          == 
                                                          (0x000f8f80U 
                                                           & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)) 
                                                         & ((1U 
                                                             == 
                                                             (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                              >> 0x00000014U)) 
                                                            | (0U 
                                                               == 
                                                               (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                                >> 0x00000014U)))));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard 
        = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_re) 
           & ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid) 
              & ((0U != (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rd)) 
                 & (((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rd) 
                     == (0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                        >> 0x0000000fU))) 
                    | ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rd) 
                       == (0x0000001fU & (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                          >> 0x00000014U)))))));
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_dec__DOT__imm_i 
        = (((- (QData)((IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                >> 0x0000001fU)))) 
            << 0x0000000cU) | (QData)((IData)((vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                               >> 0x00000014U))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = (IData)(
                                                       (3U 
                                                        != 
                                                        (3U 
                                                         & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_31 = (IData)(
                                                        (0x00001000U 
                                                         == 
                                                         (0x00003000U 
                                                          & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)));
    __VdfgRegularize_h6e95ff9d_0_13 = (IData)((7U == 
                                               (7U 
                                                & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8 = (IData)(
                                                       (3U 
                                                        == 
                                                        (7U 
                                                         & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)));
    VL_EXTENDS_WQ(128,64, __Vtemp_7, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s);
    VL_MULS_WWW(128, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mul_ss, vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16, __Vtemp_7);
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divw_s 
        = (((IData)(__VdfgRegularize_h6e95ff9d_0_18)
             ? 0x80000000U : VL_DIVS_III(32, (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))) 
           | (- (IData)((0U == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_26 = (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_dec__DOT__imm_i 
                                                 & (- (QData)((IData)(
                                                                      (3U 
                                                                       == 
                                                                       (3U 
                                                                        & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_36 = (1U 
                                                 & ((~ 
                                                     (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                      >> 2U)) 
                                                    | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_37 = ((0U 
                                                  != 
                                                  (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 0x0000001aU)) 
                                                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_31));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_38 = ((~ 
                                                  (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 4U)) 
                                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_13));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11 = ((~ 
                                                  (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 3U)) 
                                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_13));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21 = ((8U 
                                                  & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                  ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8)
                                                  : 
                                                 (3U 
                                                  == 
                                                  (3U 
                                                   & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6 = ((~ 
                                                 (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                  >> 3U)) 
                                                & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_8));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34 = ((~ 
                                                  (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 4U)) 
                                                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12 = ((~ 
                                                  (vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr 
                                                   >> 4U)) 
                                                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_22 = ((0x00000010U 
                                                  & vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr)
                                                  ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_21)
                                                  : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6));
}

void Vtb_rv64i_pipe___024root___timing_ready(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___timing_ready\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h3f920df9__0.ready("@(posedge tb_rv64i_pipe.clk)");
    }
}

void Vtb_rv64i_pipe___024root___timing_resume(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___timing_resume\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h3f920df9__0.moveToResumeQueue(
                                                          "@(posedge tb_rv64i_pipe.clk)");
    vlSelfRef.__VtrigSched_h3f920df9__0.resume("@(posedge tb_rv64i_pipe.clk)");
    if ((2ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtb_rv64i_pipe___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

void Vtb_rv64i_pipe___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

void Vtb_rv64i_pipe___024root____VbeforeTrig_h3f920df9__0(Vtb_rv64i_pipe___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root____VbeforeTrig_h3f920df9__0\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((IData)(vlSelfRef.tb_rv64i_pipe__DOT__clk) 
                                  & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i_pipe__DOT__clk__0)))));
    vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i_pipe__DOT__clk__0 
        = vlSelfRef.tb_rv64i_pipe__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h3f920df9__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtb_rv64i_pipe___024root___eval_debug_assertions(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_debug_assertions\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
