// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTB_RV64I__SYMS_H_
#define VERILATED_VTB_RV64I__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtb_rv64i.h"

// INCLUDE MODULE CLASSES
#include "Vtb_rv64i___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vtb_rv64i__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtb_rv64i* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool& __Vm_didInit;

    // MODULE INSTANCE STATE
    Vtb_rv64i___024root            TOP;

    // CONSTRUCTORS
    Vtb_rv64i__Syms(VerilatedContext* contextp, const char* namep, Vtb_rv64i* modelp);
    ~Vtb_rv64i__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
