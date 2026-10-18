// PciBinding.cpp
#include "AacRaidController.hpp"
#include "Endian.hpp"
#include <cstdint>

// Forward declaration of your OS-specific PCI abstraction
class OsPciDevice; 
class OsDmaAllocator;

// Translates aacraid_pci_attach
AacRaidController* AacRaidPciAttach(OsPciDevice& pciDev, OsDmaAllocator& dmaAlloc) 
{
    // 1. Enable Bus Mastering (Crucial for DMA!)
    if (!pciDev.EnableBusMaster()) {
        return nullptr; 
    }

    uint16_t cmd = pciDev.ReadConfig(0x04, 2);
    if (!(cmd & 0x04)) { // PCIM_CMD_BUSMASTEREN
        return nullptr;
    }

    // 2. Map BARs (Memory Mapped I/O)
    // SRC/SRCv uses BAR0 for main registers. Some might use BAR2.
    uintptr_t bar0_virt = pciDev.MapBar(0);
    if (!bar0_virt) return nullptr;

    // 3. Allocate Interrupt (MSI/MSI-X or Legacy INTx)
    uint32_t irq_vector = pciDev.AllocateInterrupt(true); // Try MSI first

    // 4. Instantiate the Controller
    // The controller will determine the exact HW interface (SRC vs SRCv) based on PCI IDs
    auto* controller = new AacRaidController(pciDev, dmaAlloc, bar0_virt, irq_vector);
    
    if (!controller->Initialize()) {
        delete controller;
        return nullptr;
    }

    return controller;
}