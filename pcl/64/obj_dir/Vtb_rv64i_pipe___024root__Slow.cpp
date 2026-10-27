// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_rv64i_pipe.h for the primary calling header

#include "Vtb_rv64i_pipe__pch.h"

void Vtb_rv64i_pipe___024root___ctor_var_reset(Vtb_rv64i_pipe___024root* vlSelf);

Vtb_rv64i_pipe___024root::Vtb_rv64i_pipe___024root(Vtb_rv64i_pipe__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtb_rv64i_pipe___024root___ctor_var_reset(this);
}

void Vtb_rv64i_pipe___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtb_rv64i_pipe___024root::~Vtb_rv64i_pipe___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
