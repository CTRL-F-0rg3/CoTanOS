// Endian.hpp
#pragma once
#include <cstdint>
#include <type_traits>

// Assuming bare-metal x86/x64 (Little-Endian). 
// If porting to Big-Endian, replace with compiler intrinsics (__builtin_bswap32).

template <typename T>
inline constexpr T LeToHost(T val) { return val; }

template <typename T>
inline constexpr T HostToLe(T val) { return val; }

// Convenience macros matching FreeBSD naming for easy 1:1 translation of logic
#define le16toh(x) LeToHost<uint16_t>(x)
#define le32toh(x) LeToHost<uint32_t>(x)
#define le64toh(x) LeToHost<uint64_t>(x)
#define htole16(x) HostToLe<uint16_t>(x)
#define htole32(x) HostToLe<uint32_t>(x)
#define htole64(x) HostToLe<uint64_t>(x)