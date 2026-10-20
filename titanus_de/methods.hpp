#pragma once
#include "class_file.hpp"
#include <vector>

struct CodeAttribute {
    uint16_t max_stack;
    uint16_t max_locals;
    std::vector<uint8_t> code;
};

struct MethodInfo {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    bool has_code = false;
    CodeAttribute code_attr;
};

// Funkcja do parowania metod z pliku .class (wywoływana wewnątrz klasy ClassFile lub obok niej)
inline bool parse_methods(ByteReader& reader, ClassFile& cls, std::vector<MethodInfo>& methods) {
    uint16_t methods_count = reader.read_u2();
    methods.resize(methods_count);

    for (int i = 0; i < methods_count; ++i) {
        MethodInfo& m = methods[i];
        m.access_flags = reader.read_u2();
        m.name_index = reader.read_u2();
        m.descriptor_index = reader.read_u2();

        uint16_t attributes_count = reader.read_u2();
        for (int j = 0; j < attributes_count; ++j) {
            uint16_t attr_name_index = reader.read_u2();
            uint32_t attr_length = reader.read_u4();

            std::string attr_name = cls.get_utf8(attr_name_index);
            if (attr_name == "Code") {
                m.has_code = true;
                m.code_attr.max_stack = reader.read_u2();
                m.code_attr.max_locals = reader.read_u2();
                
                uint32_t code_length = reader.read_u4();
                m.code_attr.code = reader.read_bytes(code_length);

                // Pomijamy exception_table oraz attributes_table wewnątrz Code (na razie upraszczamy)
                uint16_t exception_table_length = reader.read_u2();
                for (int e = 0; e < exception_table_length; ++e) {
                    reader.read_u2(); reader.read_u2(); reader.read_u2(); reader.read_u2(); // start, end, handler, catch_type
                }

                uint16_t code_attributes_count = reader.read_u2();
                for (int ca = 0; ca < code_attributes_count; ++ca) {
                    uint16_t ca_name_index = reader.read_u2();
                    uint32_t ca_length = reader.read_u4();
                    reader.read_bytes(ca_length); // pomijamy atrybuty wewnątrz kodu (np. LineNumberTable)
                }
            } else {
                reader.read_bytes(attr_length);
            }
        }
    }
    return true;
}