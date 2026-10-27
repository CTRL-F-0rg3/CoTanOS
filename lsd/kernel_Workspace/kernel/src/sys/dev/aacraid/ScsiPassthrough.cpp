// ScsiPassthrough.cpp
#include "AacRaidController.hpp"
#include "aacraid_reg.h"
#include "Endian.hpp"

class ScsiPassthrough {
public:
    ScsiPassthrough(AacRaidController& ctrl) : m_ctrl(ctrl) {}

    // Execute a SCSI command (e.g., INQUIRY, READ CAPACITY)
    int ExecuteScsi(uint8_t bus, uint8_t target, uint8_t lun, 
                    const uint8_t* cdb, uint8_t cdb_len,
                    void* data_buf, uint32_t data_len, bool is_read) 
    {
        struct aac_command* cm = AllocateCommand();
        if (!cm) return -1;

        struct aac_fib* fib = cm->cm_fib;
        struct aac_srb* srb = reinterpret_cast<struct aac_srb*>(fib->data);

        // 1. Fill SRB (SCSI Request Block)
        srb->function = htole32(AAC_SRB_FUNC_EXECUTE_SCSI);
        srb->bus = htole32(bus);
        srb->target = htole32(target);
        srb->lun = htole32(lun);
        srb->timeout = htole32(30); // 30 seconds
        srb->cdb_len = htole32(cdb_len);
        
        // Copy CDB (max 16 bytes)
        for (int i = 0; i < cdb_len && i < 16; i++) {
            srb->cdb[i] = cdb[i];
        }

        // 2. Setup Data Direction & SG List
        if (data_len > 0) {
            srb->data_len = htole32(data_len);
            srb->flags = htole32(is_read ? AAC_SRB_FLAGS_DATA_IN : AAC_SRB_FLAGS_DATA_OUT);
            
            // Map data_buf to physical address for DMA (OS specific)
            uint64_t phys_addr = m_ctrl.GetDmaAllocator().MapVirtual(data_buf, data_len);
            
            struct aac_sg_table* sg = &srb->sg_map;
            sg->SgCount = htole32(1);
            sg->SgEntry[0].SgAddress = htole32((uint32_t)phys_addr);
            sg->SgEntry[0].SgByteCount = htole32(data_len);
        } else {
            srb->flags = htole32(AAC_SRB_FLAGS_NO_DATA_XFER);
        }

        // 3. Fill FIB Header
        fib->Header.Command = htole16(ScsiPortCommand);
        fib->Header.Size = htole16(sizeof(struct aac_fib_header) + sizeof(struct aac_srb));
        
        // 4. Submit and wait
        int err = m_ctrl.SubmitAndWait(cm);
        
        // 5. Check response
        if (err == 0) {
            struct aac_srb_response* resp = reinterpret_cast<struct aac_srb_response*>(fib->data);
            uint32_t srb_status = le32toh(resp->srb_status);
            if (srb_status != AAC_SRB_STS_SUCCESS) {
                err = -2; // SCSI Error
            }
        }

        FreeCommand(cm);
        return err;
    }

private:
    AacRaidController& m_ctrl;
    struct aac_command* AllocateCommand(); // Implement pool logic
    void FreeCommand(struct aac_command* cm);
};