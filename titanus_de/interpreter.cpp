#include "interpreter.hpp"
#include <iostream>

void Interpreter::execute(const MethodInfo& entry_method, const ClassFile& cls) {
    call_stack.push(CallFrame(entry_method));

    while (!call_stack.empty()) {
        CallFrame& frame = call_stack.top();

        if (frame.pc >= frame.method.code_attr.code.size()) {
            call_stack.pop();
            continue;
        }

        uint8_t opcode = frame.method.code_attr.code[frame.pc++];

        switch (opcode) {
            case 0x00: // nop
                break;

            // Stałe stałeczkowe
            case 0x03: // iconst_0
                frame.operand_stack.push(JVMValue(0));
                break;
            case 0x04: // iconst_1
                frame.operand_stack.push(JVMValue(1));
                break;
            case 0x10: { // bipush
                int8_t val = static_cast<int8_t>(frame.method.code_attr.code[frame.pc++]);
                frame.operand_stack.push(JVMValue(val));
                break;
            }

            // Ładowanie / Zapis zmiennych lokalnych (Ogólne)
            case 0x15: { // iload (indeks w kolejnym bajcie)
                uint8_t idx = frame.method.code_attr.code[frame.pc++];
                frame.operand_stack.push(frame.local_variables[idx]);
                break;
            }
            case 0x36: { // istore (indeks w kolejnym bajcie)
                uint8_t idx = frame.method.code_attr.code[frame.pc++];
                frame.local_variables[idx] = frame.operand_stack.top();
                frame.operand_stack.pop();
                break;
            }

            // Skróty iload_0 - iload_3
            case 0x1a: case 0x1b: case 0x1c: case 0x1d: {
                uint8_t idx = opcode - 0x1a;
                frame.operand_stack.push(frame.local_variables[idx]);
                break;
            }
            // Skróty istore_0 - istore_3
            case 0x3b: case 0x3c: case 0x3d: case 0x3e: {
                uint8_t idx = opcode - 0x3b;
                frame.local_variables[idx] = frame.operand_stack.top();
                frame.operand_stack.pop();
                break;
            }

            // Arytmetyka
            case 0x60: { // iadd
                JVMValue v2 = frame.operand_stack.top(); frame.operand_stack.pop();
                JVMValue v1 = frame.operand_stack.top(); frame.operand_stack.pop();
                frame.operand_stack.push(JVMValue(v1.i + v2.i));
                break;
            }
            case 0x64: { // isub
                JVMValue v2 = frame.operand_stack.top(); frame.operand_stack.pop();
                JVMValue v1 = frame.operand_stack.top(); frame.operand_stack.pop();
                frame.operand_stack.push(JVMValue(v1.i - v2.i));
                break;
            }

            // Kontrola przepływu (Skoki)
            case 0xa7: { // goto (2-bajtowy offset)
                int16_t branch = static_cast<int16_t>((frame.method.code_attr.code[frame.pc] << 8) | frame.method.code_attr.code[frame.pc + 1]);
                frame.pc--; // Cofamy się do początku offsetu
                frame.pc += branch; // Skaczemo względnie
                break;
            }
            case 0x99: { // ifeq (jeśli równe 0)
                int16_t branch = static_cast<int16_t>((frame.method.code_attr.code[frame.pc] << 8) | frame.method.code_attr.code[frame.pc + 1]);
                frame.pc += 2;
                JVMValue val = frame.operand_stack.top(); frame.operand_stack.pop();
                if (val.i == 0) {
                    frame.pc += branch - 3; // Skok
                }
                break;
            }
            case 0x9f: { // if_icmpeq (jeśli dwa inty na stosie są równe)
                int16_t branch = static_cast<int16_t>((frame.method.code_attr.code[frame.pc] << 8) | frame.method.code_attr.code[frame.pc + 1]);
                frame.pc += 2;
                JVMValue v2 = frame.operand_stack.top(); frame.operand_stack.pop();
                JVMValue v1 = frame.operand_stack.top(); frame.operand_stack.pop();
                if (v1.i == v2.i) {
                    frame.pc += branch - 3;
                }
                break;
            }

            // Operacje na tablicach
            case 0xbc: { // newarray (tworzenie tablicy prymitywów np. byte / int)
                uint8_t atype = frame.method.code_attr.code[frame.pc++];
                JVMValue count = frame.operand_stack.top(); frame.operand_stack.pop();
                
                bool is_byte = (atype == 8); // 8 to zazwyczaj T_BYTE w JVM
                uint32_t arr_ref = heap.allocate_array(count.i, is_byte);
                frame.operand_stack.push(JVMValue::make_ref(arr_ref));
                break;
            }
            case 0xbe: { // arraylength (pobranie długości tablicy)
                JVMValue arr_ref = frame.operand_stack.top(); frame.operand_stack.pop();
                JVMObject* obj = heap.get_object(arr_ref.ref);
                int32_t len = obj->is_array ? (obj->int_array.empty() ? obj->byte_array.size() : obj->int_array.size()) : 0;
                frame.operand_stack.push(JVMValue(len));
                break;
            }
            case 0x4f: { // iastore (zapis inta do tablicy int[])
                JVMValue val = frame.operand_stack.top(); frame.operand_stack.pop();
                JVMValue index = frame.operand_stack.top(); frame.operand_stack.pop();
                JVMValue arr_ref = frame.operand_stack.top(); frame.operand_stack.pop();
                
                JVMObject* obj = heap.get_object(arr_ref.ref);
                obj->int_array[index.i] = val.i;
                break;
            }
            case 0x2e: { // iaload (odczyt inta z tablicy int[])
                JVMValue index = frame.operand_stack.top(); frame.operand_stack.pop();
                JVMValue arr_ref = frame.operand_stack.top(); frame.operand_stack.pop();
                
                JVMObject* obj = heap.get_object(arr_ref.ref);
                int32_t val = obj->int_array[index.i];
                frame.operand_stack.push(JVMValue(val));
                break;
            }

            case 0xb1: // return
                call_stack.pop();
                break;

            default:
                std::cerr << "[Interpreter] Nieobsługiwany opkod: 0x" 
                          << std::hex << (int)opcode << std::dec << " przy PC=" << (frame.pc - 1) << "\n";
                call_stack.pop();
                break;
        }
    }
}