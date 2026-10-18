// Zamiast:
// struct aac_sg_table { u_int32_t SgCount; struct aac_sg_entry SgEntry[0]; } __packed;

// W C++ bare metal:
#pragma pack(push, 1)
struct AacSgEntry {
    uint32_t SgAddress;
    uint32_t SgByteCount;
};

struct AacSgTable {
    uint32_t SgCount;
    AacSgEntry SgEntry[1]; // W C++ dajemy [1], a w runtime alokujemy tyle, ile trzeba
    
    // Helper do bezpiecznego dostępu (zamiast SgEntry[i])
    AacSgEntry& GetEntry(size_t index) {
        // Rzutowanie pamięci, żeby ominąć ograniczenia C++
        return reinterpret_cast<AacSgEntry*>(this + 1)[index]; 
    }
};
#pragma pack(pop)