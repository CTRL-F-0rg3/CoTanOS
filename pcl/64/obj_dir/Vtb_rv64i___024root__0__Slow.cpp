// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_rv64i.h for the primary calling header

#include "Vtb_rv64i__pch.h"

void Vtb_rv64i___024root___timing_ready(Vtb_rv64i___024root* vlSelf);

VL_ATTR_COLD void Vtb_rv64i___024root___eval_static(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_static\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_static__TOP
        vlSelfRef.tb_rv64i__DOT__clk = 0U;
        vlSelfRef.tb_rv64i__DOT__rst = 1U;
        vlSelfRef.tb_rv64i__DOT__cycles = 0U;
    }
    vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i__DOT__clk__0 = 0U;
    Vtb_rv64i___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtb_rv64i___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);
VL_ATTR_COLD void Vtb_rv64i___024root___stl_sequent__TOP__0(Vtb_rv64i___024root* vlSelf);

VL_ATTR_COLD bool Vtb_rv64i___024root___eval_stl(Vtb_rv64i___024root* vlSelf, CData/*0:0*/ firstIteration) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_stl\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(firstIteration)));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_rv64i___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtb_rv64i___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        {
            // Inlined CFunc: _eval_body__stl
            if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
                Vtb_rv64i___024root___stl_sequent__TOP__0(vlSelf);
            }
        }
    }
    return (__VstlExecute);
}

VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__stl(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_dump_triggers__stl\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_rv64i___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__ico(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_dump_triggers__ico\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_rv64i___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__act(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_dump_triggers__act\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_rv64i___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
}

VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__nba(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_dump_triggers__nba\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_rv64i___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
}

VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__obs(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_dump_triggers__obs\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__react(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_dump_triggers__react\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vtb_rv64i___024root___eval_final(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___eval_final\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_rv64i___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtb_rv64i___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtb_rv64i___024root___stl_sequent__TOP__0(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___stl_sequent__TOP__0\n"); );
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
    // Body
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

bool Vtb_rv64i___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_rv64i___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vtb_rv64i___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_rv64i___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge tb_rv64i.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_rv64i___024root___ctor_var_reset(Vtb_rv64i___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i___024root___ctor_var_reset\n"); );
    Vtb_rv64i__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->tb_rv64i__DOT__dmem_rdata = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 7637114326018047456ull);
    vlSelf->tb_rv64i__DOT__halted = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14525361701771437312ull);
    vlSelf->tb_rv64i__DOT__tohost_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10829216005648711002ull);
    vlSelf->tb_rv64i__DOT__tohost_data = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 488155041137007560ull);
    vlSelf->tb_rv64i__DOT__pass = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3604462170424685774ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 5333421308365106398ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 224977357538897784ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__imm = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 14179008574680552084ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__mem_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12755776895843476946ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__mem_size = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13573979601271460498ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc_sel_imm = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4899014044729168277ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc_sel_alu = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 827627036267663784ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__branch_op = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 15968778265909765432ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__rs1_val = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 2347044536012726989ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__rs2_val = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 6575832608496957656ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__alu_y = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 17304155201496633731ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc_plus4 = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 10614713019939840070ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc_plus_imm = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 1070112709639751579ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->tb_rv64i__DOT__u_core__DOT__u_rf__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 8427060982338457508ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->tb_rv64i__DOT__u_mem__DOT__imem[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4237095986742123771ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->tb_rv64i__DOT__u_mem__DOT__dmem[__Vi0] = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 6672191587978217672ull);
    }
    VL_SCOPED_RAND_RESET_W(1024, vlSelf->tb_rv64i__DOT__u_mem__DOT__hexpath, __VscopeHash, 3914050469738029243ull);
    vlSelf->__VdfgRegularize_h6e95ff9d_0_0 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_1 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_2 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_3 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_4 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_5 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_6 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_8 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_9 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_12 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_13 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_16 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_17 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_21 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_25 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_28 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_31 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_32 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_33 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_34 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_35 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_36 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_37 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_38 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_42 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_43 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_44 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_45 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__tb_rv64i__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
