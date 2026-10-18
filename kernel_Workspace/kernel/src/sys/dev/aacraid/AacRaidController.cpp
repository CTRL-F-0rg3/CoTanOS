// AacRaidController.cpp (Key methods)
#include "AacRaidController.hpp"
#include "Endian.hpp"

// Translating aacraid_new_intr_type1 (The Interrupt Handler)
void AacRaidController::HandleInterrupt() 
{
    uint32_t bellbits = m_hw->GetInterruptStatus(m_bar0, m_msiEnabled);
    if (!bellbits) return;

    // 1. Handle Sync Command completion (Initialization / IOCTLs)
    if (bellbits & AAC_DB_SYNC_COMMAND) {
        // If you have a sleeping thread waiting for a sync FIB, wake it up here.
        // In bare metal without threads, you just set a flag.
        m_hw->ClearInterruptStatus(m_bar0, AAC_DB_SYNC_COMMAND, m_msiEnabled);
    }

    // 2. Handle Async Responses (The actual I/O completions)
    // For NEW_COMM_TYPE1 (SRC), responses come via the Host RRQ (Response Ring Queue)
    ProcessResponseQueue();

    // 3. Clear AIF (Adapter Initiated FIB / Events) if pending
    if (bellbits & AAC_DB_AIF_PENDING) {
        // Handle AIF (e.g., drive hotplug, rebuild status)
        m_hw->ClearInterruptStatus(m_bar0, AAC_DB_AIF_PENDING, m_msiEnabled);
    }
}

void AacRaidController::ProcessResponseQueue() 
{
    // Translating the inner loop of aacraid_new_intr_type1
    uint32_t index = 0; // Simplified: assumes single vector for now
    while (true) {
        uint32_t handle = le32toh(m_common->ac_host_rrq[index]);
        handle &= 0x7fffffff; // Remove toggle bit
        
        uint32_t fib_index = handle & 0x0000ffff;
        if (fib_index == 0) break; // Queue empty

        struct aac_command* cm = &m_commands[fib_index - 1];
        struct aac_fib* fib = cm->cm_fib;
        
        // Convert header to host order
        fib->Header.XferState = le32toh(fib->Header.XferState);
        fib->Header.Command = le16toh(fib->Header.Command);

        // Mark command as completed and wake up waiter / call callback
        cm->cm_flags |= AAC_CMD_COMPLETED;
        
        // Clear the RRQ entry
        m_common->ac_host_rrq[index] = 0;
        
        index++;
        if (index >= m_maxFibs) index = 0;
    }
}

// Translating aac_init (Sending the Init Struct to firmware)
bool AacRaidController::InitHardware() 
{
    struct aac_adapter_init* ip = &m_common->ac_init;
    
    ip->InitStructRevision = htole32(AAC_INIT_STRUCT_REVISION_7); // For SRCv
    ip->AdapterFibsPhysicalAddress = htole32(m_commonPhys + offsetof(struct aac_common, ac_fibs));
    ip->PrintfBufferAddress = htole32(m_commonPhys + offsetof(struct aac_common, ac_printf));
    ip->HostRRQ_AddrLow = htole32(m_commonPhys + offsetof(struct aac_common, ac_host_rrq));
    
    // ... (Fill other fields as in aac_init)

    // Clear outbound doorbell
    *reinterpret_cast<volatile uint32_t*>(m_bar0 + AAC_SRC_ODBR_C) = 0xFFFFFFFF;

    // Send init struct via Mailbox
    uint32_t status = 0;
    if (SendSyncCommand(AAC_MONKER_INITSTRUCT, 
                        m_commonPhys + offsetof(struct aac_common, ac_init), 
                        0, 0, 0, &status)) {
        return false; // Firmware rejected init
    }

    return true;
}