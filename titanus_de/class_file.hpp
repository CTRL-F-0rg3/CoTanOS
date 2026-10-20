#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <stdexcept>

enum CpTag : uint8_t {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_Class = 7,
    CONSTANT_String = 8,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_NameAndType = 12,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18
};

struct CpInfo {
    uint8_t tag;
    std::string utf8_string;
    uint16_t index1{0};
    uint16_t index2{0};
    uint32_t bytes{0};
};

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

class ClassFile {
private:
    std::vector<CpInfo> constant_pool;
    std::vector<MethodInfo> methods;
    uint32_t magic = 0;
    uint16_t minor_version = 0;
    uint16_t major_version = 0;
    uint16_t constant_pool_count = 0;

    uint8_t read_u1(std::ifstream& file);
    uint16_t read_u2(std::ifstream& file);
    uint32_t read_u4(std::ifstream& file);
    std::vector<uint8_t> read_bytes(std::ifstream& file, size_t length);

public:
    bool parse(const std::string& filepath);
    std::string get_utf8(uint16_t index) const;
    const MethodInfo* find_method(const std::string& name, const std::string& descriptor) const;
    const std::vector<MethodInfo>& get_methods() const { return methods; }
};