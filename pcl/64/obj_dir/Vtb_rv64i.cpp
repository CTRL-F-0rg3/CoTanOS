// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtb_rv64i__pch.h"

//============================================================
// Constructors

Vtb_rv64i::Vtb_rv64i(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtb_rv64i__Syms(contextp(), _vcname__, this)}
    , m_evalLoop{*this, /*convergeLimit:*/ 10000}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtb_rv64i::Vtb_rv64i(const char* _vcname__)
    : Vtb_rv64i(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtb_rv64i::~Vtb_rv64i() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtb_rv64i___024root___eval_debug_assertions(Vtb_rv64i___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD void Vtb_rv64i___024root___eval_static(Vtb_rv64i___024root* vlSelf);
void Vtb_rv64i___024root___eval_initial(Vtb_rv64i___024root* vlSelf);
VL_ATTR_COLD bool Vtb_rv64i___024root___eval_stl(Vtb_rv64i___024root* vlSelf, CData/*0:0*/ firstIteration);
void Vtb_rv64i___024root___eval_sample(Vtb_rv64i___024root* vlSelf);
bool Vtb_rv64i___024root___eval_ico(Vtb_rv64i___024root* vlSelf, CData/*0:0*/ firstIteration);
bool Vtb_rv64i___024root___eval_act(Vtb_rv64i___024root* vlSelf);
bool Vtb_rv64i___024root___eval_inact(Vtb_rv64i___024root* vlSelf);
bool Vtb_rv64i___024root___eval_nba(Vtb_rv64i___024root* vlSelf);
bool Vtb_rv64i___024root___eval_obs(Vtb_rv64i___024root* vlSelf);
bool Vtb_rv64i___024root___eval_react(Vtb_rv64i___024root* vlSelf);
void Vtb_rv64i___024root___eval_postponed(Vtb_rv64i___024root* vlSelf);
VL_ATTR_COLD void Vtb_rv64i___024root___eval_final(Vtb_rv64i___024root* vlSelf);
VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__stl(Vtb_rv64i___024root* vlSelf);
VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__ico(Vtb_rv64i___024root* vlSelf);
VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__act(Vtb_rv64i___024root* vlSelf);
VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__nba(Vtb_rv64i___024root* vlSelf);
VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__obs(Vtb_rv64i___024root* vlSelf);
VL_ATTR_COLD void Vtb_rv64i___024root___eval_dump_triggers__react(Vtb_rv64i___024root* vlSelf);

void Vtb_rv64i::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtb_rv64i::eval_step\n"); );
    m_evalLoop.eval();
}

void Vtb_rv64i::evalBegin() {
#ifdef VL_DEBUG
    // Debug assertions
    Vtb_rv64i___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
}

void Vtb_rv64i::evalEnd() {
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
    vlSymsp->TOP.__VdlySched.cleanupForevered();
}

void Vtb_rv64i::evalStatic() {
    Vtb_rv64i___024root___eval_static(&(vlSymsp->TOP));
}

void Vtb_rv64i::evalInitial() {
    Vtb_rv64i___024root___eval_initial(&(vlSymsp->TOP));
}

bool Vtb_rv64i::evalStl(bool firstIteration) {
    return Vtb_rv64i___024root___eval_stl(&(vlSymsp->TOP), firstIteration);
}

void Vtb_rv64i::evalSample() {
    Vtb_rv64i___024root___eval_sample(&(vlSymsp->TOP));
}

bool Vtb_rv64i::evalIco(bool firstIteration) {
    return Vtb_rv64i___024root___eval_ico(&(vlSymsp->TOP), firstIteration);
}

bool Vtb_rv64i::evalAct() {
    return Vtb_rv64i___024root___eval_act(&(vlSymsp->TOP));
}

bool Vtb_rv64i::evalInact() {
    return Vtb_rv64i___024root___eval_inact(&(vlSymsp->TOP));
}

bool Vtb_rv64i::evalNba() {
    return Vtb_rv64i___024root___eval_nba(&(vlSymsp->TOP));
}

bool Vtb_rv64i::evalObs() {
    return Vtb_rv64i___024root___eval_obs(&(vlSymsp->TOP));
}

bool Vtb_rv64i::evalReact() {
    return Vtb_rv64i___024root___eval_react(&(vlSymsp->TOP));
}

void Vtb_rv64i::evalPostponed() {
    Vtb_rv64i___024root___eval_postponed(&(vlSymsp->TOP));
}

void Vtb_rv64i::evalFinal() {
    Vtb_rv64i___024root___eval_final(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_rv64i::dumpTriggersStl() {
    Vtb_rv64i___024root___eval_dump_triggers__stl(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_rv64i::dumpTriggersIco() {
    Vtb_rv64i___024root___eval_dump_triggers__ico(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_rv64i::dumpTriggersAct() {
    Vtb_rv64i___024root___eval_dump_triggers__act(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_rv64i::dumpTriggersNba() {
    Vtb_rv64i___024root___eval_dump_triggers__nba(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_rv64i::dumpTriggersObs() {
    Vtb_rv64i___024root___eval_dump_triggers__obs(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_rv64i::dumpTriggersReact() {
    Vtb_rv64i___024root___eval_dump_triggers__react(&(vlSymsp->TOP));
}

//============================================================
// Events and timing
bool Vtb_rv64i::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtb_rv64i::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtb_rv64i::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

VL_ATTR_COLD void Vtb_rv64i::final() {
    contextp()->executingFinal(true);
    evalFinal();
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtb_rv64i::hierName() const { return vlSymsp->name(); }
const char* Vtb_rv64i::modelName() const { return "Vtb_rv64i"; }
unsigned Vtb_rv64i::threads() const { return 1; }
void Vtb_rv64i::prepareClone() const { contextp()->prepareClone(); }
void Vtb_rv64i::atClone() const {
    contextp()->threadPoolpOnClone();
}
