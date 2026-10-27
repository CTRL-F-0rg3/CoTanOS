#pragma once
#include <fstream>
#include <cstdint>
#include <vector>

// Prosty helper nagłówkowy do czytania danych w formacie Big-Endian (wymaganym przez JVM)
class ClassReader {
    std::ifstream file;
public:
    ClassReader(const std::string& filename) {
        file.open(filename, std::ios::binary);
    }

    uint8_t read_u1() {
        uint8_t val;
        file.read(reinterpret_cast<char*>(&val), 1);
        return val;
    }

    uint16_t read_u2() {
        uint16_t val;
        file.read(reinterpret_cast<char*>(&val), 2);
        // Konwersja z Big-Endian na systemową endianness (jeśli potrzebna)
        return (val << 8) | (val >> 8); 
    }

    uint32_t read_u4() {
        uint32_t val;
        file.read(reinterpret_cast<char*>(&val), 4);
        return ((val >> 24) & 0x000000FF) |
               ((val >> 8)  & 0x0000FF00) |
               ((val << 8)  & 0x00FF0000) |
               ((val << 24) & 0xFF000000);
    }
    
    bool is_open() { return file.is_open(); }
};