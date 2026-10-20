#include "heap.hpp"
#include <stdexcept>

uint32_t JVMHeap::allocate(const std::string& class_name) {
    auto obj = std::make_unique<JVMObject>();
    obj->class_name = class_name;
    obj->is_array = false;

    if (!free_indices.empty()) {
        uint32_t idx = free_indices.back();
        free_indices.pop_back();
        objects[idx] = std::move(obj);
        return idx;
    } else {
        objects.push_back(std::move(obj));
        return static_cast<uint32_t>(objects.size() - 1);
    }
}

uint32_t JVMHeap::allocate_array(size_t size, bool is_byte_array) {
    auto obj = std::make_unique<JVMObject>();
    obj->class_name = is_byte_array ? "[B" : "[I";
    obj->is_array = true;
    if (is_byte_array) {
        obj->byte_array.resize(size, 0);
    } else {
        obj->int_array.resize(size, 0);
    }

    if (!free_indices.empty()) {
        uint32_t idx = free_indices.back();
        free_indices.pop_back();
        objects[idx] = std::move(obj);
        return idx;
    } else {
        objects.push_back(std::move(obj));
        return static_cast<uint32_t>(objects.size() - 1);
    }
}

JVMObject* JVMHeap::get_object(uint32_t ref) {
    if (ref >= objects.size() || !objects[ref]) {
        throw std::runtime_error("NullPointerException dla referencji: " + std::to_string(ref));
    }
    return objects[ref].get();
}

void JVMHeap::collect_garbage() {
    for (size_t i = 0; i < objects.size(); ++i) {
        if (objects[i] && !objects[i]->marked) {
            objects[i].reset();
            free_indices.push_back(static_cast<uint32_t>(i));
        } else if (objects[i]) {
            objects[i]->marked = false;
        }
    }
}