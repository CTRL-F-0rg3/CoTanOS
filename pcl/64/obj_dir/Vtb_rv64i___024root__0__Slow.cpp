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

extern const VlWide<22>/*703:0*/ Vtb_rv64i__ConstPool__CONST_h7f7e386a_0;
extern const VlUnpacked<CData/*0:0*/, 11> Vtb_rv64i__ConstPool__TABLE_h86a3d98a_0;

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
    VlWide<4>/*127:0*/ __Vtemp_1;
    IData/*31:0*/ __Vtemp_2;
    VlWide<4>/*127:0*/ __Vtemp_3;
    VlWide<4>/*127:0*/ __Vtemp_4;
    VlWide<4>/*127:0*/ __Vtemp_5;
    VlWide<3>/*95:0*/ __Vtemp_6;
    VlWide<4>/*127:0*/ __Vtemp_7;
    VlWide<4>/*127:0*/ __Vtemp_8;
    // Body
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
    vlSelf->tb_rv64i__DOT__dmem_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15978919348986674222ull);
    vlSelf->tb_rv64i__DOT__halted = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14525361701771437312ull);
    vlSelf->tb_rv64i__DOT__tohost_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10829216005648711002ull);
    vlSelf->tb_rv64i__DOT__tohost_data = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 488155041137007560ull);
    vlSelf->tb_rv64i__DOT__pass = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3604462170424685774ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 5333421308365106398ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 224977357538897784ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__imm = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 14179008574680552084ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__mem_size = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13573979601271460498ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc_sel_imm = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4899014044729168277ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc_sel_alu = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 827627036267663784ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__branch_op = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 15968778265909765432ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__is_ebreak = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1668302255370817533ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__is_ecall = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11341866797186660368ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__is_csr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5092306318667162997ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__is_mret = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6334918007030648171ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__csr_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 5233053273462430006ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__rs1_val = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 2347044536012726989ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__rs2_val = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 6575832608496957656ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__csr_rdata = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 14873809498460408689ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__trap_take = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9382548777320033049ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__alu_y = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 17304155201496633731ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc_plus4 = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 10614713019939840070ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__pc_plus_imm = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 1070112709639751579ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mstatus_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 13836359596868409042ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__misa_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 12359866203015280045ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mie_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 5425996523532373926ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtvec_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 3614178593088332853ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mscratch_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 2469453127855756627ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mepc_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 16687745943053405389ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mcause_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 8498628918991324826ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mtval_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 12156962711133751488ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mip_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 5570299679002641758ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__mhartid_reg = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 5369660084458508931ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_new_val = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 14554539075273867369ull);
    vlSelf->tb_rv64i__DOT__u_core__DOT__u_csr__DOT__csr_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 111397122813023416ull);
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
    vlSelf->__VdfgRegularize_h6e95ff9d_0_1 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_5 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_6 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_18 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_34 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_35 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_40 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_41 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_60 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_63 = 0;
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
