#pragma once
#include <vector>
#include <stack>
#include "class_file.hpp"
#include "heap.hpp"

struct CallFrame {
    const MethodInfo& method;
    size_t pc = 0;
    std::vector<JVMValue> local_variables;
    std::stack<JVMValue> operand_stack;

    CallFrame(const MethodInfo& m) : method(m) {
        local_variables.resize(m.code_attr.max_locals);
    }
};

class Interpreter {
private:
    std::stack<CallFrame> call_stack;
    JVMHeap heap;

public:
    void execute(const MethodInfo& entry_method, const ClassFile& cls);
};