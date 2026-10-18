// aac_baremetal_compat.h
#pragma once
#include <cstdint>
#include <cstddef>

// Typy FreeBSD -> C++ Standard
using u_int8_t   = uint8_t;
using u_int16_t  = uint16_t;
using u_int32_t  = uint32_t;
using u_int64_t  = uint64_t;
using int32_t    = int32_t; // Czasem używają bez 'u'

// Makra pakowania (zastępujące __packed)
#define __packed __attribute__((packed))

// Zastępstwo dla TAILQ (kolejki dwukierunkowe FreeBSD)
// W C++ użyjemy po prostu std::list albo napiszemy prosty intrusive list, 
// ale na start, żeby nie komplikować, możemy użyć tablicy lub prostego wskaźnika next/prev.
// Zostawmy to na później, na razie placeholder:
#define TAILQ_ENTRY(type) struct { type *tqe_next; type **tqe_prev; }
#define TAILQ_HEAD(name, type) struct name { type *tqh_first; type **tqh_last; }
// ... (resztę makr TAILQ zaimplementujemy jako inline functions w C++)

// Zastępstwo dla bus_space (MMIO)
// W FreeBSD to: bus_space_write_4(tag, handle, offset, val)
// W C++ bare metal: po prostu rzutujemy adres na wskaźnik i piszemy.
// Zdefiniujemy to jako makra lub inline funkcje, żeby kod z .c się skompilował.