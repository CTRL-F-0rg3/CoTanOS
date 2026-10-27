#pragma once
#include <vector>
#include <stack>
#include <memory>
#include "methods.hpp"
#include "heap.hpp"

// Pojedyncza ramka wywołania metody
struct CallFrame {
    const MethodInfo& method;
    size_t pc = 0;                          // Program Counter dla tej metody
    std::vector<JVMValue> local_variables;  // Zmienne lokalne + argumenty
    std::stack<JVMValue> operand_stack;     // Stos operandów dla tej metody

    CallFrame(const MethodInfo& m) : method(m) {
        local_variables.resize(m.code_attr.max_locals);
    }
};

class HeavyDutyInterpreter {
private:
    std::stack<CallFrame> call_stack;       // Stos ramek wywołań metod
    JVMHeap heap;                           // Sterta dla obiektów

public:
    void run(const MethodInfo& entry_method, const ClassFile& cls) {
        // Wrzucamy metodę startową (np. main w tej drugiej JVM) na stos ramek
        call_stack.push(CallFrame(entry_method));

        while (!call_stack.empty()) {
            // Pobieramy aktywną ramkę z wierzchołka stosu
            CallFrame& frame = call_stack.top();
            
            if (frame.pc >= frame.method.code_attr.code.size()) {
                // Jeśli skończył się kod, a nie było return -> wychodzimy z ramki
                call_stack.pop();
                continue;
            }

            uint8_t opcode = frame.method.code_attr.code[frame.pc++];

            switch (opcode) {
                case 0x03: // iconst_0
                    frame.operand_stack.push(JVMValue(0));
                    break;
                case 0x04: // iconst_1
                    frame.operand_stack.push(JVMValue(1));
                    break;

                // Tutaj lądować będą setki kolejnych opkodów...
                
                case 0xb1: // return (void)
                    call_stack.pop(); // Usuwamy ramkę zakończonej metody
                    break;

                default:
                    // Obsługa kolejnych instrukcji...
                    break;
            }
        }
    }
};