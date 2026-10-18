// SrcInterface.cpp (Translating aac_src_* functions)
#include "HwInterface.hpp"
#include "aacraid_reg.h"
#include "Endian.hpp"

class AacSrcInterface : public AacHwInterface {
public:
    uint32_t GetFwStatus(uintptr_t bar0) override {
        return *reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_OMR);
    }

    void QNotify(uintptr_t bar0, uint32_t qbit) override {
        *reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_IDBR) = (qbit << AAC_SRC_IDR_SHIFT);
    }

    uint32_t GetInterruptStatus(uintptr_t bar0, bool msi_enabled) override {
        if (msi_enabled) {
            uint32_t val = *reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_ODBR_MSI);
            return (val & AAC_MSI_SYNC_STATUS) ? AAC_DB_SYNC_COMMAND : 0;
        }
        return *reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_ODBR_R) >> AAC_SRC_ODR_SHIFT;
    }

    void ClearInterruptStatus(uintptr_t bar0, uint32_t mask, bool msi_enabled) override {
        if (msi_enabled) {
            if (mask == AAC_DB_SYNC_COMMAND) AccessDevReg(bar0, AAC_CLEAR_SYNC_BIT);
        } else {
            *reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_ODBR_C) = (mask << AAC_SRC_ODR_SHIFT);
        }
    }

    void SetMailbox(uintptr_t bar0, uint32_t cmd, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3) override {
        volatile uint32_t* mb = reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_MAILBOX);
        mb[0] = cmd; mb[1] = a0; mb[2] = a1; mb[3] = a2; mb[4] = a3;
    }

    uint32_t GetMailbox(uintptr_t bar0, int mb) override {
        return *reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_MAILBOX + (mb * 4));
    }

    void AccessDevReg(uintptr_t bar0, int mode) override {
        volatile uint32_t* idbr = reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_IDBR);
        volatile uint32_t* oimr = reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_OIMR);
        volatile uint32_t* ioar = reinterpret_cast<volatile uint32_t*>(bar0 + AAC_SRC_IOAR);

        uint32_t val;
        switch (mode) {
            case AAC_ENABLE_INTERRUPT:
                *oimr = 0xfffffffa; // Simplified for example
                break;
            case AAC_DISABLE_INTERRUPT:
                *oimr = 0xffffffff;
                break;
            case AAC_ENABLE_MSIX:
                val = *idbr; val |= 0x40; *idbr = val;
                *ioar = PMC_ALL_INTERRUPT_BITS;
                val = *oimr; *oimr = val & (~(PMC_GLOBAL_INT_BIT2 | PMC_GLOBAL_INT_BIT0));
                break;
            // ... (Translate other cases from aac_src_access_devreg)
        }
    }

    int SendCommand(uintptr_t bar0, AacRaidController& sc, struct aac_command* cm) override;
};