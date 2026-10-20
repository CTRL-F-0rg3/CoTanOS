#pragma once
#include <fstream>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string>

class ByteReader {
private:
    std::ifstream file;

public:
    explicit ByteReader(const std::string& filepath) {
        file.open(filepath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Nie udalo sie otworzyc pliku: " + filepath);
        }
    }

    uint8_t read_u1() {
        uint8_t val;
        file.read(reinterpret_cast<char*>(&val), 1);
        return val;
    }

    uint16_t read_u2() {
        uint8_t b1 = read_u1();
        uint8_t b2 = read_u1();
        return (static_cast<uint16_t>(b1) << 8) | b2;
    }

    uint32_t read_u4() {
        uint32_t b1 = read_u1();
        uint32_t b2 = read_u1();
        uint32_t b3 = read_u1();
        uint32_t b4 = read_u1();
        return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
    }

    std::vector<uint8_t> read_bytes(size_t length) {
        std::vector<uint8_t> buffer(length);
        file.read(reinterpret_cast<char*>(buffer.data()), length);
        return buffer;
    }

    bool is_eof() const { return file.eof(); }
};