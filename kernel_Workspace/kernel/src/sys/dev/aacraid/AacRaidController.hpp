// AacRaidController.hpp
#pragma once
#include <cstdint>
#include <memory>
#include "HwInterface.hpp"
#include "aacraid_reg.h"
#include "aacraid_var.h"

class OsPciDevice;
class OsDmaAllocator;

class AacRaidController {
public:
    AacRaidController(OsPciDevice& pci, OsDmaAllocator& dma, uintptr_t bar0, uint32_t irq);
    ~AacRaidController();

    bool Initialize();
    void HandleInterrupt(); // Called by your OS's IRQ dispatcher

    // Command submission
    int SubmitCommand(struct aac_command* cm);
    int WaitForCommand(struct aac_command* cm);

private:
    bool InitHardware();
    bool AllocResources();
    void ProcessResponseQueue();

    OsPciDevice& m_pci;
    OsDmaAllocator& m_dma;
    uintptr_t m_bar0;
    uint32_t m_irq;
    
    std::unique_ptr<AacHwInterface> m_hw;
    
    // Translated from struct aac_softc (simplified)
    uint32_t m_flags;
    bool m_msiEnabled;
    struct aac_common* m_common;
    uint64_t m_commonPhys;
    
    // Command pools (simplified, replace with your OS's list/queue)
    struct aac_command* m_commands;
    uint32_t m_maxFibs;
};