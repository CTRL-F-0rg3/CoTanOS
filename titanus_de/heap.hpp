#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

enum class ValType {
    Int,
    Float,
    Reference
};

struct JVMValue {
    ValType type;
    union {
        int32_t i;
        float f;
        uint32_t ref;
    };

    JVMValue() : type(ValType::Int), i(0) {}
    explicit JVMValue(int32_t val) : type(ValType::Int), i(val) {}
    explicit JVMValue(float val) : type(ValType::Float), f(val) {}
    
    static JVMValue make_ref(uint32_t r) {
        JVMValue v;
        v.type = ValType::Reference;
        v.ref = r;
        return v;
    }
};

struct JVMObject {
    std::string class_name;
    std::unordered_map<std::string, JVMValue> fields;
    
    // Obsługa tablic (np. byte[], int[])
    bool is_array = false;
    std::vector<int32_t> int_array; 
    std::vector<uint8_t> byte_array;

    bool marked = false;
};

class JVMHeap {
private:
    std::vector<std::unique_ptr<JVMObject>> objects;
    std::vector<uint32_t> free_indices;

public:
    uint32_t allocate(const std::string& class_name);
    uint32_t allocate_array(size_t size, bool is_byte_array);
    JVMObject* get_object(uint32_t ref);
    void collect_garbage();
};