// HwInterface.hpp
#pragma once
#include <cstdint>

class AacRaidController;

// Hardware Abstraction Layer for SRC / SRCv chipsets
class AacHwInterface {
public:
    virtual ~AacHwInterface() = default;

    virtual uint32_t GetFwStatus(uintptr_t bar0) = 0;
    virtual void QNotify(uintptr_t bar0, uint32_t qbit) = 0;
    virtual uint32_t GetInterruptStatus(uintptr_t bar0, bool msi_enabled) = 0;
    virtual void ClearInterruptStatus(uintptr_t bar0, uint32_t mask, bool msi_enabled) = 0;
    
    virtual void SetMailbox(uintptr_t bar0, uint32_t cmd, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3) = 0;
    virtual uint32_t GetMailbox(uintptr_t bar0, int mb) = 0;
    
    virtual void AccessDevReg(uintptr_t bar0, int mode) = 0;
    virtual int SendCommand(uintptr_t bar0, AacRaidController& sc, struct aac_command* cm) = 0;
};