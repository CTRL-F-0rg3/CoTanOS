// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_rv64i.h for the primary calling header

#include "Vtb_rv64i__pch.h"

void Vtb_rv64i___024root___ctor_var_reset(Vtb_rv64i___024root* vlSelf);

Vtb_rv64i___024root::Vtb_rv64i___024root(Vtb_rv64i__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtb_rv64i___024root___ctor_var_reset(this);
}

void Vtb_rv64i___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtb_rv64i___024root::~Vtb_rv64i___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
