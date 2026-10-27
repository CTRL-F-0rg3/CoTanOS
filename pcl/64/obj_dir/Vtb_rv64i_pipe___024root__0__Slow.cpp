// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_rv64i_pipe.h for the primary calling header

#include "Vtb_rv64i_pipe__pch.h"

void Vtb_rv64i_pipe___024root___timing_ready(Vtb_rv64i_pipe___024root* vlSelf);

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___eval_static(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_static\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_static__TOP
        vlSelfRef.tb_rv64i_pipe__DOT__clk = 0U;
        vlSelfRef.tb_rv64i_pipe__DOT__rst = 1U;
        vlSelfRef.tb_rv64i_pipe__DOT__cycles = 0U;
    }
    vlSelfRef.__Vtrigprevexpr___TOP__tb_rv64i_pipe__DOT__clk__0 = 0U;
    Vtb_rv64i_pipe___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtb_rv64i_pipe___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___stl_sequent__TOP__0(Vtb_rv64i_pipe___024root* vlSelf);

VL_ATTR_COLD bool Vtb_rv64i_pipe___024root___eval_stl(Vtb_rv64i_pipe___024root* vlSelf, CData/*0:0*/ firstIteration) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_stl\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(firstIteration)));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_rv64i_pipe___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtb_rv64i_pipe___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        {
            // Inlined CFunc: _eval_body__stl
            if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
                Vtb_rv64i_pipe___024root___stl_sequent__TOP__0(vlSelf);
            }
        }
    }
    return (__VstlExecute);
}

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___eval_dump_triggers__stl(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_dump_triggers__stl\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_rv64i_pipe___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___eval_dump_triggers__ico(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_dump_triggers__ico\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_rv64i_pipe___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___eval_dump_triggers__act(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_dump_triggers__act\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_rv64i_pipe___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
}

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___eval_dump_triggers__nba(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_dump_triggers__nba\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_rv64i_pipe___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
}

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___eval_dump_triggers__obs(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_dump_triggers__obs\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___eval_dump_triggers__react(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_dump_triggers__react\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___eval_final(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___eval_final\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_rv64i_pipe___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtb_rv64i_pipe___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___stl_sequent__TOP__0(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___stl_sequent__TOP__0\n"); );
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
    VlWide<4>/*127:0*/ __Vtemp_1;
    // Body
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
    __VdfgRegularize_h6e95ff9d_0_15 = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_reg_we) 
                                       & (0U != (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd)));
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
    __VdfgRegularize_h6e95ff9d_0_14 = ((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_reg_we) 
                                       & (0U != (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd)));
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
    tb_rv64i_pipe__DOT__u_core__DOT__fwd_b_exmem = 
        (((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd) 
          == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2)) 
         & (IData)(__VdfgRegularize_h6e95ff9d_0_14));
    tb_rv64i_pipe__DOT__u_core__DOT__fwd_a_exmem = 
        (((IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd) 
          == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1)) 
         & (IData)(__VdfgRegularize_h6e95ff9d_0_14));
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
    VL_EXTENDS_WQ(128,64, __Vtemp_1, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s);
    VL_MULS_WWW(128, vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mul_ss, vlSelfRef.__VdfgRegularize_h6e95ff9d_0_16, __Vtemp_1);
    vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divw_s 
        = (((IData)(__VdfgRegularize_h6e95ff9d_0_18)
             ? 0x80000000U : VL_DIVS_III(32, (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s), (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s))) 
           | (- (IData)((0U == (IData)(vlSelfRef.tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s)))));
}

bool Vtb_rv64i_pipe___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_rv64i_pipe___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vtb_rv64i_pipe___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i_pipe___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_rv64i_pipe___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge tb_rv64i_pipe.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_rv64i_pipe___024root___ctor_var_reset(Vtb_rv64i_pipe___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_rv64i_pipe___024root___ctor_var_reset\n"); );
    Vtb_rv64i_pipe__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->tb_rv64i_pipe__DOT__halted = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7666806725706000838ull);
    vlSelf->tb_rv64i_pipe__DOT__tohost_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9883346250121576958ull);
    vlSelf->tb_rv64i_pipe__DOT__tohost_data = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 3519554936546952561ull);
    vlSelf->tb_rv64i_pipe__DOT__pass = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6624283967104696167ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__pc = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 492033210095016075ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__ifid_pc = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 16761154510598552011ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__ifid_instr = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16284649343923716980ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__ifid_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2654016834628364700ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__wb_wdata = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 15393034241444509844ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_pc = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 17270841178553178536ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1_val = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 4105336974772461612ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2_val = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 14364354396932978435ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_imm = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 1434525513178799009ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_rs1 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 6937200536719611772ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_rs2 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 11589181561710130481ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 1680090330323813936ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_op = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 10419073459333449419ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_a_pc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14493821634077221391ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_alu_b_imm = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14371564682939908409ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_reg_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13855701995836351784ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1523076965284401651ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11142369265034629181ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_size = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 6213831451862330612ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_pc_sel_alu = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18263106105060234134ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_branch_op = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 295991723038051017ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_is_halt = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10959550227961292119ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_is_jump = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14716373114237079985ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_is_branch = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14723056018596910498ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__idex_mem_unsigned = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 598945715239804205ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 1702378228135352171ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_reg_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15162417989261568348ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_alu_y = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 7377736535311313411ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__memwb_rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 14554267558388267697ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__memwb_reg_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3477821771354597357ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__ex_rs1 = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 863583633072501597ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__ex_rs2 = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 2700191134519353204ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__ex_redirect = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14559945240826625074ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_rs2_val = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 12891758825322709526ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15977928909849627255ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11997384403027837246ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_size = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 7219880581978764138ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_is_jump = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7996201639665712000ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__exmem_mem_unsigned = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12690633777837622350ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__memwb_alu_y = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 17551185304380373153ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_data = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 15283508697163447477ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4171125955125281707ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_size = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 17092416836273540078ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__memwb_is_jump = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 528899324942679522ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__memwb_mem_unsigned = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4758840046971891660ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__load_use_hazard = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17964736673088716800ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__flush = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1490025054385204102ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__a_s = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 11313099959554988603ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__b_s = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 15500677423844367118ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__addw_full = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 11212616077738514317ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__subw_full = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8769704654415105721ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sllw_full = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13309652651229334897ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__sraw_full = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5893022094199779299ull);
    VL_SCOPED_RAND_RESET_W(128, vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mul_ss, __VscopeHash, 13982802232720861185ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__mulw_full = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1713210749348170672ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divw_s = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4563562312465410081ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__divuw_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8307511070197016295ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remw_s = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7416210675082549165ull);
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_alu__DOT__remuw_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12901810806416548063ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_rf__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 7887928098509354958ull);
    }
    vlSelf->tb_rv64i_pipe__DOT__u_core__DOT__u_dec__DOT__imm_i = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 6271952006569100145ull);
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->tb_rv64i_pipe__DOT__u_mem__DOT__imem[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13184811201526172873ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->tb_rv64i_pipe__DOT__u_mem__DOT__dmem[__Vi0] = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 7475719805630813611ull);
    }
    VL_SCOPED_RAND_RESET_W(1024, vlSelf->tb_rv64i_pipe__DOT__u_mem__DOT__hexpath, __VscopeHash, 13575892368917705855ull);
    vlSelf->__VdfgRegularize_h6e95ff9d_0_0 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_1 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_6 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_8 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_11 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_12 = 0;
    VL_ZERO_RESET_W(128, vlSelf->__VdfgRegularize_h6e95ff9d_0_16);
    vlSelf->__VdfgRegularize_h6e95ff9d_0_17 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_20 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_21 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_22 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_23 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_25 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_26 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_31 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_34 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_35 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_36 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_37 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_38 = 0;
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
    vlSelf->__Vtrigprevexpr___TOP__tb_rv64i_pipe__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
