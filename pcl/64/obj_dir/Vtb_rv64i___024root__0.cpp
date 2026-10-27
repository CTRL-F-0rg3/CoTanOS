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

extern const VlWide<22>/*703:0*/ Vtb_rv64i__ConstPool__CONST_h7f7e386a_0;
extern const VlUnpacked<CData/*0:0*/, 11> Vtb_rv64i__ConstPool__TABLE_h86a3d98a_0;

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
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__illegal;
    tb_rv64i__DOT__u_core__DOT__illegal = 0;
    CData/*2:0*/ tb_rv64i__DOT__u_core__DOT__csr_op;
    tb_rv64i__DOT__u_core__DOT__csr_op = 0;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__csr_instr_illegal;
    tb_rv64i__DOT__u_core__DOT__csr_instr_illegal = 0;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__csr_src;
    tb_rv64i__DOT__u_core__DOT__csr_src = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__addw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__addw_full = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__subw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__subw_full = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sllw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sllw_full = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sraw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sraw_full = 0;
    VlWide<4>/*127:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mul_ss;
    VL_ZERO_W(128, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mul_ss);
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mulw_full;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mulw_full = 0;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s = 0;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divw_s;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divw_s = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divuw_r;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divuw_r = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remw_s;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remw_s = 0;
    IData/*31:0*/ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remuw_r;
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remuw_r = 0;
    CData/*0:0*/ tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_should_write;
    tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_should_write = 0;
    QData/*63:0*/ tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i;
    tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_0;
    __VdfgRegularize_h6e95ff9d_0_0 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_2;
    __VdfgRegularize_h6e95ff9d_0_2 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_3;
    __VdfgRegularize_h6e95ff9d_0_3 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_4;
    __VdfgRegularize_h6e95ff9d_0_4 = 0;
    SData/*9:0*/ __VdfgRegularize_h6e95ff9d_0_7;
    __VdfgRegularize_h6e95ff9d_0_7 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_8;
    __VdfgRegularize_h6e95ff9d_0_8 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_9;
    __VdfgRegularize_h6e95ff9d_0_9 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_10;
    __VdfgRegularize_h6e95ff9d_0_10 = 0;
    CData/*2:0*/ __VdfgRegularize_h6e95ff9d_0_13;
    __VdfgRegularize_h6e95ff9d_0_13 = 0;
    SData/*11:0*/ __VdfgRegularize_h6e95ff9d_0_14;
    __VdfgRegularize_h6e95ff9d_0_14 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_22;
    __VdfgRegularize_h6e95ff9d_0_22 = 0;
    CData/*6:0*/ __VdfgRegularize_h6e95ff9d_0_23;
    __VdfgRegularize_h6e95ff9d_0_23 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_24;
    __VdfgRegularize_h6e95ff9d_0_24 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_29;
    __VdfgRegularize_h6e95ff9d_0_29 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_30;
    __VdfgRegularize_h6e95ff9d_0_30 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_31;
    __VdfgRegularize_h6e95ff9d_0_31 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_32;
    __VdfgRegularize_h6e95ff9d_0_32 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_33;
    __VdfgRegularize_h6e95ff9d_0_33 = 0;
    VlWide<4>/*127:0*/ __VdfgRegularize_h6e95ff9d_0_37;
    VL_ZERO_W(128, __VdfgRegularize_h6e95ff9d_0_37);
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_38;
    __VdfgRegularize_h6e95ff9d_0_38 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_39;
    __VdfgRegularize_h6e95ff9d_0_39 = 0;
    CData/*1:0*/ __VdfgRegularize_h6e95ff9d_0_42;
    __VdfgRegularize_h6e95ff9d_0_42 = 0;
    CData/*4:0*/ __VdfgRegularize_h6e95ff9d_0_43;
    __VdfgRegularize_h6e95ff9d_0_43 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_46;
    __VdfgRegularize_h6e95ff9d_0_46 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_47;
    __VdfgRegularize_h6e95ff9d_0_47 = 0;
    CData/*1:0*/ __VdfgRegularize_h6e95ff9d_0_48;
    __VdfgRegularize_h6e95ff9d_0_48 = 0;
    CData/*1:0*/ __VdfgRegularize_h6e95ff9d_0_49;
    __VdfgRegularize_h6e95ff9d_0_49 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_51;
    __VdfgRegularize_h6e95ff9d_0_51 = 0;
    QData/*63:0*/ __VdfgRegularize_h6e95ff9d_0_52;
    __VdfgRegularize_h6e95ff9d_0_52 = 0;
    QData/*63:0*/ __VdfgRegularize_h6e95ff9d_0_53;
    __VdfgRegularize_h6e95ff9d_0_53 = 0;
    CData/*4:0*/ __VdfgRegularize_h6e95ff9d_0_54;
    __VdfgRegularize_h6e95ff9d_0_54 = 0;
    CData/*4:0*/ __VdfgRegularize_h6e95ff9d_0_55;
    __VdfgRegularize_h6e95ff9d_0_55 = 0;
    CData/*4:0*/ __VdfgRegularize_h6e95ff9d_0_56;
    __VdfgRegularize_h6e95ff9d_0_56 = 0;
    CData/*4:0*/ __VdfgRegularize_h6e95ff9d_0_57;
    __VdfgRegularize_h6e95ff9d_0_57 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_58;
    __VdfgRegularize_h6e95ff9d_0_58 = 0;
    CData/*5:0*/ __VdfgRegularize_h6e95ff9d_0_59;
    __VdfgRegularize_h6e95ff9d_0_59 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_61;
    __VdfgRegularize_h6e95ff9d_0_61 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_62;
    __VdfgRegularize_h6e95ff9d_0_62 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_64;
    __VdfgRegularize_h6e95ff9d_0_64 = 0;
    QData/*63:0*/ __Vdly__tb_rv64i__DOT__u_core__DOT__pc;
    __Vdly__tb_rv64i__DOT__u_core__DOT__pc = 0;
    CData/*0:0*/ __Vdly__tb_rv64i__DOT__halted;
    __Vdly__tb_rv64i__DOT__halted = 0;
    QData/*63:0*/ __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg;
    __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg = 0;
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
    VlWide<4>/*127:0*/ __Vtemp_1;
    IData/*31:0*/ __Vtemp_2;
    VlWide<4>/*127:0*/ __Vtemp_3;
    VlWide<4>/*127:0*/ __Vtemp_4;
    VlWide<4>/*127:0*/ __Vtemp_5;
    VlWide<3>/*95:0*/ __Vtemp_6;
    VlWide<4>/*127:0*/ __Vtemp_7;
    VlWide<4>/*127:0*/ __Vtemp_8;
    // Body
    __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
        = vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg;
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
        __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg = 0x0000000000001800ULL;
        __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v0 = 1U;
        __Vdly__tb_rv64i__DOT__u_core__DOT__pc = 0x0000000080000000ULL;
        __Vdly__tb_rv64i__DOT__halted = 0U;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mhartid_reg = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__misa_reg = 0x8000000000000100ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mip_reg = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mscratch_reg = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mie_reg = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mcause_reg = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtval_reg = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtvec_reg = 0ULL;
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mepc_reg = 0ULL;
    } else {
        if ((((~ (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__trap_take)) 
              & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18)
                  ? (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                       ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_63)
                       : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34)) 
                     & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6))
                  : ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                      ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_40) 
                         & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))
                      : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_41)))) 
             & (0U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd)))) {
            __VdlyVal__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 
                = ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_csr)
                    ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_rdata
                    : (((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_imm) 
                        | (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_alu))
                        ? (4ULL + vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc)
                        : (((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18)) 
                            & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)) 
                               & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_60)))
                            ? ((((0U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))
                                  ? ((- (QData)((IData)(
                                                        (1U 
                                                         & (IData)(
                                                                   (vlSelfRef.tb_rv64i__DOT__dmem_rdata 
                                                                    >> 7U)))))) 
                                     & (- (QData)((IData)(
                                                          (1U 
                                                           & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)))))))
                                  : ((0x00ffffffffffff00ULL 
                                      & (((1U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size))
                                           ? ((- (QData)((IData)(
                                                                 (1U 
                                                                  & (IData)(
                                                                            (vlSelfRef.tb_rv64i__DOT__dmem_rdata 
                                                                             >> 0x0000000fU)))))) 
                                              & (- (QData)((IData)(
                                                                   (1U 
                                                                    & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)))))))
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
                                                                               & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5))))))
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
                            : vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y)));
            __VdlyDim0__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 
                = vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd;
            __VdlySet__tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs__v32 = 1U;
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_rv64i__DOT__halted)))) {
            __Vdly__tb_rv64i__DOT__u_core__DOT__pc 
                = ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__trap_take)
                    ? (0xfffffffffffffffcULL & vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtvec_reg)
                    : ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_mret)
                        ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mepc_reg
                        : ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_alu)
                            ? (0xfffffffffffffffeULL 
                               & (vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val 
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
                                    : vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_plus4)))));
            if (vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_ebreak) {
                __Vdly__tb_rv64i__DOT__halted = 1U;
            }
        }
        if (vlSelfRef.tb_rv64i__DOT__u_core__DOT__trap_take) {
            __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                = ((0xffffffffffffff7fULL & __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg) 
                   | ((QData)((IData)((1U & (IData)(
                                                    (vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                                                     >> 3U))))) 
                      << 7U));
            __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                = (0xfffffffffffffff7ULL & __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg);
            __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                = (0x0000000000001800ULL | __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg);
            vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mcause_reg 
                = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35)
                    ? 2ULL : ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_ebreak)
                               ? 3ULL : (0x000000000000000bULL 
                                         & (- (QData)((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_ecall))))));
            vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtval_reg 
                = ((- (QData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35))) 
                   & (QData)((IData)(vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                     [(0x00007fffU 
                                       & (IData)(((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                   - 0x0000000080000000ULL) 
                                                  >> 2U)))])));
            vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mepc_reg 
                = (0xfffffffffffffffcULL & vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc);
        } else {
            if (vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_mret) {
                __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                    = ((0xfffffffffffffff7ULL & __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg) 
                       | ((QData)((IData)((1U & (IData)(
                                                        (vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                                                         >> 7U))))) 
                          << 3U));
                __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                    = (0x0000000000000080ULL | __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg);
                __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                    = (0x0000000000001800ULL | __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg);
            } else if (vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_we) {
                if ((0x0300U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                    __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                        = ((0xfffffffffffffff7ULL & __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg) 
                           | ((QData)((IData)((1U & (IData)(
                                                            (vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val 
                                                             >> 3U))))) 
                              << 3U));
                    __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                        = ((0xffffffffffffff7fULL & __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg) 
                           | ((QData)((IData)((1U & (IData)(
                                                            (vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val 
                                                             >> 7U))))) 
                              << 7U));
                    __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
                        = (0x0000000000001800ULL | __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg);
                }
            }
            if ((1U & (~ (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_mret)))) {
                if (vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_we) {
                    if ((0x0300U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                        if ((0x0304U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                            if ((0x0305U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                if ((0x0340U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                    if ((0x0341U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                        if ((0x0342U 
                                             == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                            vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mcause_reg 
                                                = vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val;
                                        }
                                        if ((0x0342U 
                                             != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                            if ((0x0343U 
                                                 == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                                vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtval_reg 
                                                    = vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val;
                                            }
                                        }
                                    }
                                    if ((0x0341U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mepc_reg 
                                            = (0xfffffffffffffffcULL 
                                               & vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__trap_take)))) {
            if ((1U & (~ (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_mret)))) {
                if (vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_we) {
                    if ((0x0300U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                        if ((0x0304U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                            if ((0x0305U != (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                if ((0x0340U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                    vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mscratch_reg 
                                        = vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val;
                                }
                            }
                            if ((0x0305U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                                vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtvec_reg 
                                    = (0xfffffffffffffffcULL 
                                       & vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val);
                            }
                        }
                        if ((0x0304U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) {
                            vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mie_reg 
                                = vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val;
                        }
                    }
                }
            }
        }
    }
    vlSelfRef.tb_rv64i__DOT__tohost_we = 0U;
    if (vlSelfRef.tb_rv64i__DOT__dmem_we) {
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
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg 
        = __Vdly__tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg;
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
    __VdfgRegularize_h6e95ff9d_0_54 = (0x0000001fU 
                                       & (- (IData)(
                                                    (1U 
                                                     & (~ 
                                                        (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                         [
                                                         (0x00007fffU 
                                                          & (IData)(
                                                                    ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                      - 0x0000000080000000ULL) 
                                                                     >> 2U)))] 
                                                         >> 0x0000001dU))))));
    __VdfgRegularize_h6e95ff9d_0_55 = (0x0000001fU 
                                       & (- (IData)(
                                                    (1U 
                                                     & (~ 
                                                        (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                         [
                                                         (0x00007fffU 
                                                          & (IData)(
                                                                    ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                      - 0x0000000080000000ULL) 
                                                                     >> 2U)))] 
                                                         >> 0x0000001cU))))));
    __VdfgRegularize_h6e95ff9d_0_56 = (0x0000001fU 
                                       & (- (IData)(
                                                    (1U 
                                                     & (~ 
                                                        (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                         [
                                                         (0x00007fffU 
                                                          & (IData)(
                                                                    ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                      - 0x0000000080000000ULL) 
                                                                     >> 2U)))] 
                                                         >> 0x0000001bU))))));
    __VdfgRegularize_h6e95ff9d_0_57 = (0x0000001fU 
                                       & (- (IData)(
                                                    (1U 
                                                     & (~ 
                                                        (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                         [
                                                         (0x00007fffU 
                                                          & (IData)(
                                                                    ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                      - 0x0000000080000000ULL) 
                                                                     >> 2U)))] 
                                                         >> 0x0000001aU))))));
    __VdfgRegularize_h6e95ff9d_0_46 = (0U != (3U & 
                                              (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                               [(0x00007fffU 
                                                 & (IData)(
                                                           ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                             - 0x0000000080000000ULL) 
                                                            >> 2U)))] 
                                               >> 2U)));
    __VdfgRegularize_h6e95ff9d_0_30 = (1U & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                             [(0x00007fffU 
                                               & (IData)(
                                                         ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                           - 0x0000000080000000ULL) 
                                                          >> 2U)))] 
                                             >> 0x00000019U));
    __VdfgRegularize_h6e95ff9d_0_58 = (1U & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                             [(0x00007fffU 
                                               & (IData)(
                                                         ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                           - 0x0000000080000000ULL) 
                                                          >> 2U)))] 
                                             >> 0x0000001eU));
    __VdfgRegularize_h6e95ff9d_0_59 = (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                       [(0x00007fffU 
                                         & (IData)(
                                                   ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                     - 0x0000000080000000ULL) 
                                                    >> 2U)))] 
                                       >> 0x0000001aU);
    __VdfgRegularize_h6e95ff9d_0_24 = (0U != (3U & 
                                              (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                               [(0x00007fffU 
                                                 & (IData)(
                                                           ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                             - 0x0000000080000000ULL) 
                                                            >> 2U)))] 
                                               >> 0x0000000cU)));
    __VdfgRegularize_h6e95ff9d_0_49 = (3U & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                             [(0x00007fffU 
                                               & (IData)(
                                                         ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                           - 0x0000000080000000ULL) 
                                                          >> 2U)))] 
                                             >> 5U));
    __VdfgRegularize_h6e95ff9d_0_29 = (1U & (~ (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                [(0x00007fffU 
                                                  & (IData)(
                                                            ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                              - 0x0000000080000000ULL) 
                                                             >> 2U)))] 
                                                >> 1U)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 0x0000000eU));
    __VdfgRegularize_h6e95ff9d_0_23 = (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                       [(0x00007fffU 
                                         & (IData)(
                                                   ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                     - 0x0000000080000000ULL) 
                                                    >> 2U)))] 
                                       >> 0x00000019U);
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd = (0x0000001fU 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 7U));
    __VdfgRegularize_h6e95ff9d_0_13 = (7U & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                             [(0x00007fffU 
                                               & (IData)(
                                                         ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                           - 0x0000000080000000ULL) 
                                                          >> 2U)))] 
                                             >> 0x0000000cU));
    tb_rv64i__DOT__u_core__DOT__rs2 = (0x0000001fU 
                                       & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                          [(0x00007fffU 
                                            & (IData)(
                                                      ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                        - 0x0000000080000000ULL) 
                                                       >> 2U)))] 
                                          >> 0x00000014U));
    tb_rv64i__DOT__u_core__DOT__rs1 = (0x0000001fU 
                                       & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                          [(0x00007fffU 
                                            & (IData)(
                                                      ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                        - 0x0000000080000000ULL) 
                                                       >> 2U)))] 
                                          >> 0x0000000fU));
    __VdfgRegularize_h6e95ff9d_0_0 = (1U & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                            [(0x00007fffU 
                                              & (IData)(
                                                        ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                          - 0x0000000080000000ULL) 
                                                         >> 2U)))] 
                                            >> 0x0000000cU));
    __VdfgRegularize_h6e95ff9d_0_2 = (1U & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                            [(0x00007fffU 
                                              & (IData)(
                                                        ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                          - 0x0000000080000000ULL) 
                                                         >> 2U)))] 
                                            >> 0x0000000dU));
    __VdfgRegularize_h6e95ff9d_0_14 = (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                       [(0x00007fffU 
                                         & (IData)(
                                                   ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                     - 0x0000000080000000ULL) 
                                                    >> 2U)))] 
                                       >> 0x00000014U);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 5U));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18 = (1U 
                                                 & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                    [
                                                    (0x00007fffU 
                                                     & (IData)(
                                                               ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                 - 0x0000000080000000ULL) 
                                                                >> 2U)))] 
                                                    >> 6U));
    __VdfgRegularize_h6e95ff9d_0_62 = (1U & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                       [(0x00007fffU 
                                         & (IData)(
                                                   ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                     - 0x0000000080000000ULL) 
                                                    >> 2U)))]);
    __VdfgRegularize_h6e95ff9d_0_8 = (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                      [(0x00007fffU 
                                        & (IData)((
                                                   (vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                    - 0x0000000080000000ULL) 
                                                   >> 2U)))] 
                                      >> 0x0000001fU);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = (1U 
                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                   [
                                                   (0x00007fffU 
                                                    & (IData)(
                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                - 0x0000000080000000ULL) 
                                                               >> 2U)))] 
                                                   >> 4U));
    __VdfgRegularize_h6e95ff9d_0_3 = (1U & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                            [(0x00007fffU 
                                              & (IData)(
                                                        ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                          - 0x0000000080000000ULL) 
                                                         >> 2U)))] 
                                            >> 2U));
    __VdfgRegularize_h6e95ff9d_0_4 = (1U & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                            [(0x00007fffU 
                                              & (IData)(
                                                        ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                          - 0x0000000080000000ULL) 
                                                         >> 2U)))] 
                                            >> 3U));
    __VdfgRegularize_h6e95ff9d_0_48 = (3U & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                       [(0x00007fffU 
                                         & (IData)(
                                                   ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                     - 0x0000000080000000ULL) 
                                                    >> 2U)))]);
    __VdfgRegularize_h6e95ff9d_0_7 = (((IData)(__VdfgRegularize_h6e95ff9d_0_23) 
                                       << 3U) | (IData)(__VdfgRegularize_h6e95ff9d_0_13));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val = 
        ((0U == (IData)(tb_rv64i__DOT__u_core__DOT__rs2))
          ? 0ULL : vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs
         [tb_rv64i__DOT__u_core__DOT__rs2]);
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val = 
        ((0U == (IData)(tb_rv64i__DOT__u_core__DOT__rs1))
          ? 0ULL : vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs
         [tb_rv64i__DOT__u_core__DOT__rs1]);
    if (__VdfgRegularize_h6e95ff9d_0_2) {
        __VdfgRegularize_h6e95ff9d_0_42 = (3U & (2U 
                                                 | (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_0)))));
        __VdfgRegularize_h6e95ff9d_0_43 = ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                            ? 4U : 3U);
    } else {
        __VdfgRegularize_h6e95ff9d_0_42 = (3U & (1U 
                                                 & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_0)))));
        __VdfgRegularize_h6e95ff9d_0_43 = (2U & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_0))));
    }
    __VdfgRegularize_h6e95ff9d_0_51 = ((0U == (IData)(tb_rv64i__DOT__u_core__DOT__rs1)) 
                                       & ((0U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd)) 
                                          & (0x0302U 
                                             == (IData)(__VdfgRegularize_h6e95ff9d_0_14))));
    __VdfgRegularize_h6e95ff9d_0_31 = ((0U == (IData)(tb_rv64i__DOT__u_core__DOT__rs1)) 
                                       & ((0U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd)) 
                                          & (1U == (IData)(__VdfgRegularize_h6e95ff9d_0_14))));
    __VdfgRegularize_h6e95ff9d_0_32 = ((0U == (IData)(tb_rv64i__DOT__u_core__DOT__rs1)) 
                                       & ((0U == (IData)(__VdfgRegularize_h6e95ff9d_0_14)) 
                                          & (0U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd))));
    __VdfgRegularize_h6e95ff9d_0_9 = (1U & ((IData)(__VdfgRegularize_h6e95ff9d_0_29) 
                                            | (~ (IData)(__VdfgRegularize_h6e95ff9d_0_62))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34 = ((3U 
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
                                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_62));
    tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i = 
        (((- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_8))) 
          << 0x0000000cU) | (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_14)));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__branch_op 
        = (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)
             ? ((IData)(__VdfgRegularize_h6e95ff9d_0_2)
                 ? ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                     ? 6U : 5U) : ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                    ? 4U : 3U)) : (
                                                   ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                     ? 2U
                                                     : 1U) 
                                                   & (- (IData)(
                                                                (1U 
                                                                 & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_2))))))) 
           & (- (IData)(((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)) 
                         & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_4)) 
                            & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_3)) 
                               & (3U == ((IData)(__VdfgRegularize_h6e95ff9d_0_48) 
                                         & (IData)(__VdfgRegularize_h6e95ff9d_0_49)))))))));
    __VdfgRegularize_h6e95ff9d_0_52 = ((((QData)((IData)(
                                                         (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_8))))) 
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
                                                             == (IData)(__VdfgRegularize_h6e95ff9d_0_48))))));
    __VdfgRegularize_h6e95ff9d_0_22 = ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_3)) 
                                       & (3U == (IData)(__VdfgRegularize_h6e95ff9d_0_48)));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__mem_size 
        = (3U & (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                   ? ((IData)(__VdfgRegularize_h6e95ff9d_0_42) 
                      | ((- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_4))) 
                         | ((- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_3))) 
                            | ((- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_29))) 
                               | ((- (IData)((1U & 
                                              (~ (IData)(__VdfgRegularize_h6e95ff9d_0_62))))) 
                                  | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))) 
                                     | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)))))))))
                   : ((- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_4))) 
                      | ((- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_3))) 
                         | ((- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_29))) 
                            | ((- (IData)((1U & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_62))))) 
                               | ((IData)(__VdfgRegularize_h6e95ff9d_0_42) 
                                  | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))))))))) 
                 | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18)))));
    __VdfgRegularize_h6e95ff9d_0_47 = (1U & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_3)) 
                                             | (IData)(__VdfgRegularize_h6e95ff9d_0_9)));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_imm 
        = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)) 
           & ((IData)(__VdfgRegularize_h6e95ff9d_0_4) 
              & ((3U == (IData)(__VdfgRegularize_h6e95ff9d_0_49)) 
                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34))));
    __VdfgRegularize_h6e95ff9d_0_33 = ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_4)) 
                                       & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34));
    __VdfgRegularize_h6e95ff9d_0_53 = (tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i 
                                       & (- (QData)((IData)(
                                                            (3U 
                                                             == (IData)(__VdfgRegularize_h6e95ff9d_0_48))))));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_mret = 
        ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
         & ((IData)(__VdfgRegularize_h6e95ff9d_0_22) 
            & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
               & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_0)) 
                  & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_4)) 
                     & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_2)) 
                        & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_32)) 
                           & ((3U == (IData)(__VdfgRegularize_h6e95ff9d_0_49)) 
                              & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_31)) 
                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_51))))))))));
    tb_rv64i__DOT__u_core__DOT__alu_op = (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                                            ? (((IData)(__VdfgRegularize_h6e95ff9d_0_4)
                                                 ? 
                                                ((- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_22))) 
                                                 & ((0U 
                                                     == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                     ? 0x0bU
                                                     : 
                                                    ((0x0100U 
                                                      == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                      ? 0x0cU
                                                      : 
                                                     ((1U 
                                                       == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                       ? 0x0dU
                                                       : 
                                                      ((5U 
                                                        == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                        ? 0x0eU
                                                        : 
                                                       ((0x0105U 
                                                         == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                         ? 0x0fU
                                                         : 
                                                        ((8U 
                                                          == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                          ? 0x18U
                                                          : 
                                                         ((0x000cU 
                                                           == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                           ? 0x19U
                                                           : 
                                                          ((0x000dU 
                                                            == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                            ? 0x1aU
                                                            : 
                                                           ((0x000eU 
                                                             == (IData)(__VdfgRegularize_h6e95ff9d_0_7))
                                                             ? 0x1bU
                                                             : 
                                                            (0x1cU 
                                                             & (- (IData)(
                                                                          (0x000fU 
                                                                           == (IData)(__VdfgRegularize_h6e95ff9d_0_7)))))))))))))))
                                                 : 
                                                ((IData)(__VdfgRegularize_h6e95ff9d_0_3)
                                                  ? 
                                                 (0x0aU 
                                                  & (- (IData)(
                                                               (3U 
                                                                == (IData)(__VdfgRegularize_h6e95ff9d_0_48)))))
                                                  : 
                                                 ((- (IData)(
                                                             (1U 
                                                              & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_8))))) 
                                                  & (((IData)(__VdfgRegularize_h6e95ff9d_0_58)
                                                       ? 
                                                      ((IData)(__VdfgRegularize_h6e95ff9d_0_55) 
                                                       & ((IData)(__VdfgRegularize_h6e95ff9d_0_56) 
                                                          & ((IData)(__VdfgRegularize_h6e95ff9d_0_57) 
                                                             & ((- (IData)(
                                                                           (1U 
                                                                            & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_30))))) 
                                                                & (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)
                                                                     ? 
                                                                    (7U 
                                                                     & (- (IData)(
                                                                                ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_2)) 
                                                                                & (IData)(__VdfgRegularize_h6e95ff9d_0_0)))))
                                                                     : 
                                                                    (1U 
                                                                     & ((- (IData)(
                                                                                (1U 
                                                                                & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_0))))) 
                                                                        & (- (IData)(
                                                                                (1U 
                                                                                & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_2)))))))) 
                                                                   & (IData)(__VdfgRegularize_h6e95ff9d_0_54))))))
                                                       : 
                                                      ((IData)(__VdfgRegularize_h6e95ff9d_0_55) 
                                                       & ((IData)(__VdfgRegularize_h6e95ff9d_0_56) 
                                                          & ((IData)(__VdfgRegularize_h6e95ff9d_0_57) 
                                                             & (((IData)(__VdfgRegularize_h6e95ff9d_0_30)
                                                                  ? 
                                                                 ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)
                                                                   ? 
                                                                  ((IData)(__VdfgRegularize_h6e95ff9d_0_2)
                                                                    ? 
                                                                   ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                                     ? 0x17U
                                                                     : 0x16U)
                                                                    : 
                                                                   ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                                     ? 0x15U
                                                                     : 0x14U))
                                                                   : 
                                                                  ((IData)(__VdfgRegularize_h6e95ff9d_0_2)
                                                                    ? 
                                                                   ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                                     ? 0x13U
                                                                     : 0x12U)
                                                                    : 
                                                                   ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                                     ? 0x11U
                                                                     : 0x10U)))
                                                                  : 
                                                                 ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)
                                                                   ? 
                                                                  ((IData)(__VdfgRegularize_h6e95ff9d_0_2)
                                                                    ? 
                                                                   ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                                     ? 9U
                                                                     : 8U)
                                                                    : 
                                                                   ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                                     ? 6U
                                                                     : 5U))
                                                                   : (IData)(__VdfgRegularize_h6e95ff9d_0_43))) 
                                                                & (IData)(__VdfgRegularize_h6e95ff9d_0_54)))))) 
                                                     & (- (IData)(
                                                                  (3U 
                                                                   == (IData)(__VdfgRegularize_h6e95ff9d_0_48)))))))) 
                                               & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))))
                                            : (((IData)(__VdfgRegularize_h6e95ff9d_0_4)
                                                 ? 
                                                (((0U 
                                                   == (IData)(__VdfgRegularize_h6e95ff9d_0_13))
                                                   ? 0x0bU
                                                   : 
                                                  ((1U 
                                                    == (IData)(__VdfgRegularize_h6e95ff9d_0_13))
                                                    ? 0x0dU
                                                    : 
                                                   (((0U 
                                                      == (IData)(__VdfgRegularize_h6e95ff9d_0_23))
                                                      ? 0x0eU
                                                      : 
                                                     (0x0fU 
                                                      & (- (IData)(
                                                                   (0x20U 
                                                                    == (IData)(__VdfgRegularize_h6e95ff9d_0_23)))))) 
                                                    & (- (IData)(
                                                                 (5U 
                                                                  == (IData)(__VdfgRegularize_h6e95ff9d_0_13))))))) 
                                                 & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_22))))
                                                 : 
                                                ((- (IData)(
                                                            (1U 
                                                             & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_3))))) 
                                                 & (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)
                                                      ? 
                                                     ((IData)(__VdfgRegularize_h6e95ff9d_0_2)
                                                       ? 
                                                      ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                        ? 9U
                                                        : 8U)
                                                       : 
                                                      ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                                                        ? 
                                                       ((0U 
                                                         == (IData)(__VdfgRegularize_h6e95ff9d_0_59))
                                                         ? 6U
                                                         : 
                                                        (7U 
                                                         & (- (IData)(
                                                                      (0x10U 
                                                                       == (IData)(__VdfgRegularize_h6e95ff9d_0_59))))))
                                                        : 5U))
                                                      : (IData)(__VdfgRegularize_h6e95ff9d_0_43)) 
                                                    & (- (IData)(
                                                                 (3U 
                                                                  == (IData)(__VdfgRegularize_h6e95ff9d_0_48))))))) 
                                               & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))))) 
                                          & (- (IData)(
                                                       (1U 
                                                        & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_40 = ((IData)(__VdfgRegularize_h6e95ff9d_0_4)
                                                  ? (IData)(__VdfgRegularize_h6e95ff9d_0_22)
                                                  : 
                                                 (3U 
                                                  == (IData)(__VdfgRegularize_h6e95ff9d_0_48)));
    __VdfgRegularize_h6e95ff9d_0_10 = ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_4)) 
                                       & (IData)(__VdfgRegularize_h6e95ff9d_0_22));
    if (vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18) {
        tb_rv64i__DOT__u_core__DOT__illegal = (1U & 
                                               ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)) 
                                                | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                    ? 
                                                   ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_62)) 
                                                    | ((IData)(__VdfgRegularize_h6e95ff9d_0_29) 
                                                       | ((~ 
                                                           ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)
                                                             ? (IData)(__VdfgRegularize_h6e95ff9d_0_24)
                                                             : 
                                                            ((IData)(__VdfgRegularize_h6e95ff9d_0_32) 
                                                             | ((IData)(__VdfgRegularize_h6e95ff9d_0_31) 
                                                                | ((IData)(__VdfgRegularize_h6e95ff9d_0_24) 
                                                                   | (IData)(__VdfgRegularize_h6e95ff9d_0_51)))))) 
                                                          | (IData)(__VdfgRegularize_h6e95ff9d_0_46))))
                                                    : 
                                                   ((IData)(__VdfgRegularize_h6e95ff9d_0_4)
                                                     ? (IData)(__VdfgRegularize_h6e95ff9d_0_47)
                                                     : 
                                                    ((IData)(__VdfgRegularize_h6e95ff9d_0_3)
                                                      ? 
                                                     (~ 
                                                      ((0U 
                                                        == (IData)(__VdfgRegularize_h6e95ff9d_0_13)) 
                                                       & (3U 
                                                          == (IData)(__VdfgRegularize_h6e95ff9d_0_48))))
                                                      : 
                                                     (((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                                                       & (IData)(__VdfgRegularize_h6e95ff9d_0_2)) 
                                                      | (IData)(__VdfgRegularize_h6e95ff9d_0_9)))))));
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm = 
            ((- (QData)((IData)((1U & (~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)))))) 
             & (((IData)(__VdfgRegularize_h6e95ff9d_0_4)
                  ? ((((- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_8))) 
                       << 0x00000015U) | (QData)((IData)(
                                                         ((((IData)(__VdfgRegularize_h6e95ff9d_0_8) 
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
                     & (- (QData)((IData)((7U == (7U 
                                                  & vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                  [
                                                  (0x00007fffU 
                                                   & (IData)(
                                                             ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                               - 0x0000000080000000ULL) 
                                                              >> 2U)))]))))))
                  : ((IData)(__VdfgRegularize_h6e95ff9d_0_3)
                      ? __VdfgRegularize_h6e95ff9d_0_53
                      : ((((- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_8))) 
                           << 0x0000000dU) | (QData)((IData)(
                                                             ((((IData)(__VdfgRegularize_h6e95ff9d_0_8) 
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
                         & (- (QData)((IData)((3U == (IData)(__VdfgRegularize_h6e95ff9d_0_48)))))))) 
                & (- (QData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)))));
    } else if (vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6) {
        if (vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) {
            tb_rv64i__DOT__u_core__DOT__illegal = (1U 
                                                   & ((IData)(__VdfgRegularize_h6e95ff9d_0_4)
                                                       ? 
                                                      ((~ 
                                                        ((3U 
                                                          == (IData)(__VdfgRegularize_h6e95ff9d_0_48)) 
                                                         & ((0U 
                                                             == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                            | ((0x0100U 
                                                                == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                               | ((1U 
                                                                   == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                                  | ((5U 
                                                                      == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                                     | ((0x0105U 
                                                                         == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                                        | ((8U 
                                                                            == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                                           | ((0x000cU 
                                                                               == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                                              | ((0x000dU 
                                                                                == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                                                | ((0x000eU 
                                                                                == (IData)(__VdfgRegularize_h6e95ff9d_0_7)) 
                                                                                | (0x000fU 
                                                                                == (IData)(__VdfgRegularize_h6e95ff9d_0_7))))))))))))) 
                                                       | (IData)(__VdfgRegularize_h6e95ff9d_0_3))
                                                       : 
                                                      ((IData)(__VdfgRegularize_h6e95ff9d_0_3)
                                                        ? (IData)(__VdfgRegularize_h6e95ff9d_0_9)
                                                        : 
                                                       ((IData)(__VdfgRegularize_h6e95ff9d_0_9) 
                                                        | ((IData)(__VdfgRegularize_h6e95ff9d_0_8) 
                                                           | ((IData)(__VdfgRegularize_h6e95ff9d_0_58)
                                                               ? 
                                                              ((IData)(__VdfgRegularize_h6e95ff9d_0_30) 
                                                               | ((0U 
                                                                   != 
                                                                   (3U 
                                                                    & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                       [
                                                                       (0x00007fffU 
                                                                        & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                       >> 0x0000001aU))) 
                                                                  | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)
                                                                       ? 
                                                                      ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_0)) 
                                                                       | (IData)(__VdfgRegularize_h6e95ff9d_0_2))
                                                                       : (IData)(__VdfgRegularize_h6e95ff9d_0_24)) 
                                                                     | (0U 
                                                                        != 
                                                                        (3U 
                                                                         & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                            [
                                                                            (0x00007fffU 
                                                                             & (IData)(
                                                                                ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                                >> 2U)))] 
                                                                            >> 0x0000001cU))))))
                                                               : 
                                                              (0U 
                                                               != 
                                                               (0x0000000fU 
                                                                & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                                   [
                                                                   (0x00007fffU 
                                                                    & (IData)(
                                                                              ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                                - 0x0000000080000000ULL) 
                                                                               >> 2U)))] 
                                                                   >> 0x0000001aU)))))))));
            vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm 
                = ((- (QData)((IData)(((~ (IData)(__VdfgRegularize_h6e95ff9d_0_4)) 
                                       & (IData)(__VdfgRegularize_h6e95ff9d_0_3))))) 
                   & __VdfgRegularize_h6e95ff9d_0_52);
        } else {
            tb_rv64i__DOT__u_core__DOT__illegal = (1U 
                                                   & ((IData)(__VdfgRegularize_h6e95ff9d_0_9) 
                                                      | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5) 
                                                         | (IData)(__VdfgRegularize_h6e95ff9d_0_46))));
            vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm 
                = ((((- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_8))) 
                     << 0x0000000cU) | (QData)((IData)(
                                                       (((IData)(__VdfgRegularize_h6e95ff9d_0_23) 
                                                         << 5U) 
                                                        | (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__rd))))) 
                   & (- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_10))));
        }
    } else if (vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) {
        if (__VdfgRegularize_h6e95ff9d_0_4) {
            tb_rv64i__DOT__u_core__DOT__illegal = (1U 
                                                   & ((~ 
                                                       ((3U 
                                                         == (IData)(__VdfgRegularize_h6e95ff9d_0_48)) 
                                                        & ((0U 
                                                            == (IData)(__VdfgRegularize_h6e95ff9d_0_13)) 
                                                           | ((1U 
                                                               == (IData)(__VdfgRegularize_h6e95ff9d_0_13))
                                                               ? 
                                                              (0U 
                                                               == (IData)(__VdfgRegularize_h6e95ff9d_0_23))
                                                               : 
                                                              ((5U 
                                                                == (IData)(__VdfgRegularize_h6e95ff9d_0_13)) 
                                                               & ((0U 
                                                                   == (IData)(__VdfgRegularize_h6e95ff9d_0_23)) 
                                                                  | (0x20U 
                                                                     == (IData)(__VdfgRegularize_h6e95ff9d_0_23)))))))) 
                                                      | (IData)(__VdfgRegularize_h6e95ff9d_0_3)));
            vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm 
                = (((0U == (IData)(__VdfgRegularize_h6e95ff9d_0_13))
                     ? tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i
                     : ((1U == (IData)(__VdfgRegularize_h6e95ff9d_0_13))
                         ? (QData)((IData)(tb_rv64i__DOT__u_core__DOT__rs2))
                         : ((5U == (IData)(__VdfgRegularize_h6e95ff9d_0_13))
                             ? (QData)((IData)(tb_rv64i__DOT__u_core__DOT__rs2))
                             : tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i))) 
                   & (- (QData)((IData)(__VdfgRegularize_h6e95ff9d_0_22))));
        } else if (__VdfgRegularize_h6e95ff9d_0_3) {
            tb_rv64i__DOT__u_core__DOT__illegal = (1U 
                                                   & (IData)(__VdfgRegularize_h6e95ff9d_0_9));
            vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm 
                = __VdfgRegularize_h6e95ff9d_0_52;
        } else {
            tb_rv64i__DOT__u_core__DOT__illegal = (1U 
                                                   & (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)
                                                        ? 
                                                       ((~ 
                                                         ((IData)(__VdfgRegularize_h6e95ff9d_0_2) 
                                                          | ((0U 
                                                              == (IData)(__VdfgRegularize_h6e95ff9d_0_59)) 
                                                             | (0x10U 
                                                                == (IData)(__VdfgRegularize_h6e95ff9d_0_59))))) 
                                                        & (IData)(__VdfgRegularize_h6e95ff9d_0_0))
                                                        : 
                                                       ((~ 
                                                         ((0U 
                                                           == (IData)(__VdfgRegularize_h6e95ff9d_0_59)) 
                                                          | (IData)(__VdfgRegularize_h6e95ff9d_0_2))) 
                                                        & (IData)(__VdfgRegularize_h6e95ff9d_0_0))) 
                                                      | (IData)(__VdfgRegularize_h6e95ff9d_0_9)));
            vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm 
                = (((IData)(__VdfgRegularize_h6e95ff9d_0_2)
                     ? tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i
                     : ((IData)(__VdfgRegularize_h6e95ff9d_0_0)
                         ? (QData)((IData)((0x0000003fU 
                                            & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                               [(0x00007fffU 
                                                 & (IData)(
                                                           ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                             - 0x0000000080000000ULL) 
                                                            >> 2U)))] 
                                               >> 0x00000014U))))
                         : tb_rv64i__DOT__u_core__DOT__u_dec__DOT__imm_i)) 
                   & (- (QData)((IData)((3U == (IData)(__VdfgRegularize_h6e95ff9d_0_48))))));
        }
    } else {
        tb_rv64i__DOT__u_core__DOT__illegal = (1U & 
                                               ((IData)(__VdfgRegularize_h6e95ff9d_0_4)
                                                 ? (IData)(__VdfgRegularize_h6e95ff9d_0_47)
                                                 : 
                                                ((IData)(__VdfgRegularize_h6e95ff9d_0_3) 
                                                 | (((3U 
                                                      == 
                                                      (3U 
                                                       & (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                          [
                                                          (0x00007fffU 
                                                           & (IData)(
                                                                     ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                                       - 0x0000000080000000ULL) 
                                                                      >> 2U)))] 
                                                          >> 0x0000000dU))) 
                                                     & (IData)(__VdfgRegularize_h6e95ff9d_0_0)) 
                                                    | (IData)(__VdfgRegularize_h6e95ff9d_0_9)))));
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm = 
            (__VdfgRegularize_h6e95ff9d_0_53 & ((- (QData)((IData)(
                                                                   (1U 
                                                                    & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_4)))))) 
                                                & (- (QData)((IData)(
                                                                     (1U 
                                                                      & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_3))))))));
    }
    __VdfgRegularize_h6e95ff9d_0_61 = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)) 
                                       & (IData)(__VdfgRegularize_h6e95ff9d_0_33));
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s = (
                                                   ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18)) 
                                                    & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)) 
                                                       & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                                          & (IData)(__VdfgRegularize_h6e95ff9d_0_33))))
                                                    ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc
                                                    : vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val);
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_60 = ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)) 
                                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_10));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_ecall 
        = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
           & ((IData)(__VdfgRegularize_h6e95ff9d_0_10) 
              & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                 & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_2)) 
                    & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_0)) 
                       & ((3U == (IData)(__VdfgRegularize_h6e95ff9d_0_49)) 
                          & (IData)(__VdfgRegularize_h6e95ff9d_0_32)))))));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_ebreak 
        = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
           & ((IData)(__VdfgRegularize_h6e95ff9d_0_10) 
              & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_5)) 
                 & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_2)) 
                    & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_0)) 
                       & ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_32)) 
                          & ((3U == (IData)(__VdfgRegularize_h6e95ff9d_0_49)) 
                             & (IData)(__VdfgRegularize_h6e95ff9d_0_31))))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_63 = ((IData)(__VdfgRegularize_h6e95ff9d_0_24) 
                                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_10));
    __VdfgRegularize_h6e95ff9d_0_64 = ((7U == (7U & 
                                               (vlSelfRef.tb_rv64i__DOT__u_mem__DOT__imem
                                                [(0x00007fffU 
                                                  & (IData)(
                                                            ((vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
                                                              - 0x0000000080000000ULL) 
                                                             >> 2U)))] 
                                                >> 4U))) 
                                       & (IData)(__VdfgRegularize_h6e95ff9d_0_10));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_41 = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                  ? (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_40)
                                                  : (IData)(__VdfgRegularize_h6e95ff9d_0_10));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_sel_alu 
        = ((3U == (IData)(__VdfgRegularize_h6e95ff9d_0_49)) 
           & (IData)(__VdfgRegularize_h6e95ff9d_0_61));
    VL_EXTENDS_WQ(128,64, __VdfgRegularize_h6e95ff9d_0_37, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s);
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_csr = 
        ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
         & ((3U == (IData)(__VdfgRegularize_h6e95ff9d_0_49)) 
            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_63)));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr 
        = (((IData)(__VdfgRegularize_h6e95ff9d_0_2)
             ? (IData)(__VdfgRegularize_h6e95ff9d_0_14)
             : ((IData)(__VdfgRegularize_h6e95ff9d_0_14) 
                & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_0))))) 
           & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_64))));
    tb_rv64i__DOT__u_core__DOT__csr_op = (((IData)(__VdfgRegularize_h6e95ff9d_0_2)
                                            ? (IData)(__VdfgRegularize_h6e95ff9d_0_13)
                                            : ((IData)(__VdfgRegularize_h6e95ff9d_0_13) 
                                               & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_0))))) 
                                          & (- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_64))));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc_plus_imm 
        = (vlSelfRef.tb_rv64i__DOT__u_core__DOT__pc 
           + vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm);
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s = (
                                                   ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18)
                                                     ? 
                                                    ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6) 
                                                     & (IData)(__VdfgRegularize_h6e95ff9d_0_61))
                                                     : 
                                                    ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6)
                                                      ? 
                                                     ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                       ? (IData)(__VdfgRegularize_h6e95ff9d_0_33)
                                                       : (IData)(__VdfgRegularize_h6e95ff9d_0_10))
                                                      : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_41)))
                                                    ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__imm
                                                    : vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs2_val);
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_rdata 
        = ((0x0300U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
            ? (0x0000000000001800ULL | vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg)
            : ((0x0301U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
                ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__misa_reg
                : ((0x0304U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
                    ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mie_reg
                    : ((0x0305U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
                        ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtvec_reg
                        : ((0x0340U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
                            ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mscratch_reg
                            : ((0x0341U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
                                ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mepc_reg
                                : ((0x0342U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
                                    ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mcause_reg
                                    : ((0x0343U == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
                                        ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtval_reg
                                        : ((0x0344U 
                                            == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))
                                            ? vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mip_reg
                                            : (vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mhartid_reg 
                                               & (- (QData)((IData)(
                                                                    (0x0f14U 
                                                                     == (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr)))))))))))))));
    tb_rv64i__DOT__u_core__DOT__csr_src = ((4U & (IData)(tb_rv64i__DOT__u_core__DOT__csr_op))
                                            ? (QData)((IData)(tb_rv64i__DOT__u_core__DOT__rs1))
                                            : vlSelfRef.tb_rv64i__DOT__u_core__DOT__rs1_val);
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__addw_full 
        = ((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s) 
           + (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s));
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__subw_full 
        = ((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s) 
           - (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s));
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sllw_full 
        = ((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s) 
           << (0x0000001fU & (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)));
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__sraw_full 
        = VL_SHIFTRS_III(32,32,5, (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s), 
                         (0x0000001fU & (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)));
    VL_EXTENDS_WQ(128,64, __Vtemp_1, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s);
    VL_MULS_WWW(128, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mul_ss, __VdfgRegularize_h6e95ff9d_0_37, __Vtemp_1);
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mulw_full 
        = ((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s) 
           * (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s));
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divuw_r 
        = (VL_DIV_III(32, (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)) 
           | (- (IData)((0U == (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)))));
    __VdfgRegularize_h6e95ff9d_0_38 = ((0x8000000000000000ULL 
                                        == tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s) 
                                       & (0xffffffffffffffffULL 
                                          == tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s));
    __VdfgRegularize_h6e95ff9d_0_39 = ((0x80000000U 
                                        == (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s)) 
                                       & (0xffffffffU 
                                          == (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)));
    if ((0U == (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s))) {
        tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remuw_r 
            = (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s);
        tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remw_s 
            = (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s);
    } else {
        tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remuw_r 
            = VL_MODDIV_III(32, (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s));
        tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remw_s 
            = (VL_MODDIVS_III(32, (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)) 
               & (- (IData)((1U & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_39))))));
    }
    if ((2U & (IData)(tb_rv64i__DOT__u_core__DOT__csr_op))) {
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val 
            = ((1U & (IData)(tb_rv64i__DOT__u_core__DOT__csr_op))
                ? ((~ tb_rv64i__DOT__u_core__DOT__csr_src) 
                   & vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_rdata)
                : (tb_rv64i__DOT__u_core__DOT__csr_src 
                   | vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_rdata));
        tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_should_write 
            = (1U & (0ULL != tb_rv64i__DOT__u_core__DOT__csr_src));
    } else {
        vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val 
            = ((1U & (IData)(tb_rv64i__DOT__u_core__DOT__csr_op))
                ? tb_rv64i__DOT__u_core__DOT__csr_src
                : vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_rdata);
        tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_should_write 
            = (1U & (IData)(tb_rv64i__DOT__u_core__DOT__csr_op));
    }
    tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divw_s 
        = (((IData)(__VdfgRegularize_h6e95ff9d_0_39)
             ? 0x80000000U : VL_DIVS_III(32, (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s))) 
           | (- (IData)((0U == (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)))));
    __Vtemp_2 = VL_MATCHMASKED_I(12, (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr), Vtb_rv64i__ConstPool__CONST_h7f7e386a_0);
    tb_rv64i__DOT__u_core__DOT__csr_instr_illegal = 
        (((~ (IData)(Vtb_rv64i__ConstPool__TABLE_h86a3d98a_0
                     [__Vtemp_2])) | (IData)(((0x0c00U 
                                               == (0x0c00U 
                                                   & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__csr_addr))) 
                                              & (IData)(tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_should_write)))) 
         & (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_csr));
    __Vtemp_3[0U] = (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s);
    __Vtemp_3[1U] = (IData)((tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s 
                             >> 0x00000020U));
    __Vtemp_3[2U] = 0U;
    __Vtemp_3[3U] = 0U;
    __Vtemp_4[0U] = (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s);
    __Vtemp_4[1U] = (IData)((tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                             >> 0x00000020U));
    __Vtemp_4[2U] = 0U;
    __Vtemp_4[3U] = 0U;
    VL_MUL_W(4, __Vtemp_5, __Vtemp_3, __Vtemp_4);
    __Vtemp_6[0U] = (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s);
    __Vtemp_6[1U] = (IData)((tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s 
                             >> 0x00000020U));
    __Vtemp_6[2U] = 0U;
    VL_EXTENDS_WW(128,65, __Vtemp_7, __Vtemp_6);
    VL_MULS_WWW(128, __Vtemp_8, __VdfgRegularize_h6e95ff9d_0_37, __Vtemp_7);
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__alu_y = (
                                                   (0x00000010U 
                                                    & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                    ? 
                                                   ((8U 
                                                     & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                     ? 
                                                    ((4U 
                                                      & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                      ? 
                                                     ((- (QData)((IData)(
                                                                         (1U 
                                                                          & (~ (IData)(tb_rv64i__DOT__u_core__DOT__alu_op)))))) 
                                                      & ((((QData)((IData)(
                                                                           (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remuw_r 
                                                                                >> 0x0000001fU))))) 
                                                           << 0x00000020U) 
                                                          | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remuw_r))) 
                                                         & (- (QData)((IData)(
                                                                              (1U 
                                                                               & (~ 
                                                                                ((IData)(tb_rv64i__DOT__u_core__DOT__alu_op) 
                                                                                >> 1U))))))))
                                                      : 
                                                     ((2U 
                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (((QData)((IData)(
                                                                         (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remw_s 
                                                                                >> 0x0000001fU))))) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__remw_s)))
                                                        : 
                                                       (((QData)((IData)(
                                                                         (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divuw_r 
                                                                                >> 0x0000001fU))))) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divuw_r))))
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (((QData)((IData)(
                                                                         (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divw_s 
                                                                                >> 0x0000001fU))))) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__divw_s)))
                                                        : 
                                                       (((QData)((IData)(
                                                                         (- (IData)(
                                                                                (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mulw_full 
                                                                                >> 0x0000001fU))))) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mulw_full))))))
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
                                                       ((0ULL 
                                                         == tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)
                                                         ? tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s
                                                         : 
                                                        VL_MODDIV_QQQ(64, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s))
                                                        : 
                                                       ((0ULL 
                                                         == tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)
                                                         ? tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s
                                                         : 
                                                        (VL_MODDIVS_QQQ(64, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s) 
                                                         & (- (QData)((IData)(
                                                                              (1U 
                                                                               & (~ (IData)(__VdfgRegularize_h6e95ff9d_0_38)))))))))
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (VL_DIV_QQQ(64, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s) 
                                                        | (- (QData)((IData)(
                                                                             (0ULL 
                                                                              == tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)))))
                                                        : 
                                                       (((IData)(__VdfgRegularize_h6e95ff9d_0_38)
                                                          ? 0x8000000000000000ULL
                                                          : 
                                                         VL_DIVS_QQQ(64, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)) 
                                                        | (- (QData)((IData)(
                                                                             (0ULL 
                                                                              == tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)))))))
                                                      : 
                                                     ((2U 
                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (((QData)((IData)(__Vtemp_5[3U])) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(__Vtemp_5[2U])))
                                                        : 
                                                       (((QData)((IData)(__Vtemp_8[3U])) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(__Vtemp_8[2U]))))
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (((QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mul_ss[3U])) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mul_ss[2U])))
                                                        : 
                                                       (((QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mul_ss[1U])) 
                                                         << 0x00000020U) 
                                                        | (QData)((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__mul_ss[0U])))))))
                                                    : 
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
                                                                          ((IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s) 
                                                                           >> 
                                                                           (0x0000001fU 
                                                                            & (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s))))))
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
                                                        : tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                        & tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)
                                                        : 
                                                       (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                        | tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s))))
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
                                                       VL_SHIFTRS_QQI(64,64,6, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s, 
                                                                      (0x0000003fU 
                                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)))
                                                        : 
                                                       (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                        >> 
                                                        (0x0000003fU 
                                                         & (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s))))
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                        ^ tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)
                                                        : 
                                                       (1ULL 
                                                        & (- (QData)((IData)(
                                                                             (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                                              < tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)))))))
                                                      : 
                                                     ((2U 
                                                       & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (1ULL 
                                                        & (- (QData)((IData)(
                                                                             VL_LTS_IQQ(64, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s, tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)))))
                                                        : 
                                                       (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                        << 
                                                        (0x0000003fU 
                                                         & (IData)(tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s))))
                                                       : 
                                                      ((1U 
                                                        & (IData)(tb_rv64i__DOT__u_core__DOT__alu_op))
                                                        ? 
                                                       (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                        - tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s)
                                                        : 
                                                       (tb_rv64i__DOT__u_core__DOT__u_alu__DOT__a_s 
                                                        + tb_rv64i__DOT__u_core__DOT__u_alu__DOT__b_s))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_35 = ((IData)(tb_rv64i__DOT__u_core__DOT__illegal) 
                                                 | (IData)(tb_rv64i__DOT__u_core__DOT__csr_instr_illegal));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__trap_take 
        = ((IData)(tb_rv64i__DOT__u_core__DOT__illegal) 
           | ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_ecall) 
              | ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_ebreak) 
                 | (IData)(tb_rv64i__DOT__u_core__DOT__csr_instr_illegal))));
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
    vlSelfRef.tb_rv64i__DOT__dmem_we = ((~ (IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__trap_take)) 
                                        & ((~ (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18)) 
                                           & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_6) 
                                              & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_60))));
    vlSelfRef.tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_we 
        = ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_csr) 
           & ((~ ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__trap_take) 
                  | ((IData)(vlSelfRef.tb_rv64i__DOT__u_core__DOT__is_mret) 
                     | (IData)(tb_rv64i__DOT__u_core__DOT__csr_instr_illegal)))) 
              & (IData)(tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_should_write)));
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
