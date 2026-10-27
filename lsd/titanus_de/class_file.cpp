#include "class_file.hpp"
#include <iostream>

uint8_t ClassFile::read_u1(std::ifstream& file) {
    uint8_t val;
    file.read(reinterpret_cast<char*>(&val), 1);
    return val;
}

uint16_t ClassFile::read_u2(std::ifstream& file) {
    uint8_t b1 = read_u1(file);
    uint8_t b2 = read_u1(file);
    return (static_cast<uint16_t>(b1) << 8) | b2;
}

uint32_t ClassFile::read_u4(std::ifstream& file) {
    uint32_t b1 = read_u1(file);
    uint32_t b2 = read_u1(file);
    uint32_t b3 = read_u1(file);
    uint32_t b4 = read_u1(file);
    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

std::vector<uint8_t> ClassFile::read_bytes(std::ifstream& file, size_t length) {
    std::vector<uint8_t> buffer(length);
    file.read(reinterpret_cast<char*>(buffer.data()), length);
    return buffer;
}

bool ClassFile::parse(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[ClassFile Error] Nie udało się otworzyć pliku: " << filepath << "\n";
        return false;
    }

    magic = read_u4(file);
    if (magic != 0xCAFEBABE) {
        std::cerr << "[ClassFile Error] Błędny Magic Number: 0x" << std::hex << magic << std::dec << "\n";
        return false;
    }

    minor_version = read_u2(file);
    major_version = read_u2(file);
    constant_pool_count = read_u2(file);
    
    constant_pool.resize(constant_pool_count);
    for (int i = 1; i < constant_pool_count; ++i) {
        CpInfo info;
        info.tag = read_u1(file);

        switch (info.tag) {
            case CONSTANT_Utf8: {
                uint16_t len = read_u2(file);
                auto bytes = read_bytes(file, len);
                info.utf8_string = std::string(bytes.begin(), bytes.end());
                break;
            }
            case CONSTANT_Class:
            case CONSTANT_String:
            case CONSTANT_MethodType:
                info.index1 = read_u2(file);
                break;
            case CONSTANT_Fieldref:
            case CONSTANT_Methodref:
            case CONSTANT_InterfaceMethodref:
            case CONSTANT_NameAndType:
                info.index1 = read_u2(file);
                info.index2 = read_u2(file);
                break;
            case CONSTANT_Integer:
            case CONSTANT_Float:
                info.bytes = read_u4(file);
                break;
            case CONSTANT_Long:
            case CONSTANT_Double:
                read_u4(file);
                read_u4(file);
                i++; // Long i Double zajmują dwa wpisy
                break;
            default:
                break;
        }
        constant_pool[i] = info;
    }

    read_u2(file); // access_flags
    read_u2(file); // this_class
    read_u2(file); // super_class

    uint16_t interfaces_count = read_u2(file);
    for (int i = 0; i < interfaces_count; ++i) read_u2(file);

    uint16_t fields_count = read_u2(file);
    for (int i = 0; i < fields_count; ++i) {
        read_u2(file); read_u2(file); read_u2(file);
        uint16_t f_attr_count = read_u2(file);
        for (int j = 0; j < f_attr_count; ++j) {
            read_u2(file);
            uint32_t len = read_u4(file);
            read_bytes(file, len);
        }
    }

    uint16_t methods_count = read_u2(file);
    methods.resize(methods_count);
    for (int i = 0; i < methods_count; ++i) {
        MethodInfo& m = methods[i];
        m.access_flags = read_u2(file);
        m.name_index = read_u2(file);
        m.descriptor_index = read_u2(file);

        uint16_t attr_count = read_u2(file);
        for (int j = 0; j < attr_count; ++j) {
            uint16_t attr_name_idx = read_u2(file);
            uint32_t attr_len = read_u4(file);
            std::string attr_name = get_utf8(attr_name_idx);

            if (attr_name == "Code") {
                m.has_code = true;
                m.code_attr.max_stack = read_u2(file);
                m.code_attr.max_locals = read_u2(file);
                uint32_t code_len = read_u4(file);
                m.code_attr.code = read_bytes(file, code_len);

                uint16_t ex_table_len = read_u2(file);
                for (int e = 0; e < ex_table_len; ++e) {
                    read_u2(file); read_u2(file); read_u2(file); read_u2(file);
                }

                uint16_t code_attr_count = read_u2(file);
                for (int ca = 0; ca < code_attr_count; ++ca) {
                    read_u2(file);
                    uint32_t ca_len = read_u4(file);
                    read_bytes(file, ca_len);
                }
            } else {
                read_bytes(file, attr_len);
            }
        }
    }

    return true;
}

std::string ClassFile::get_utf8(uint16_t index) const {
    if (index > 0 && index < constant_pool.size() && constant_pool[index].tag == CONSTANT_Utf8) {
        return constant_pool[index].utf8_string;
    }
    return "";
}

const MethodInfo* ClassFile::find_method(const std::string& name, const std::string& descriptor) const {
    for (const auto& m : methods) {
        if (get_utf8(m.name_index) == name && get_utf8(m.descriptor_index) == descriptor) {
            return &m;
        }
    }
    return nullptr;
}