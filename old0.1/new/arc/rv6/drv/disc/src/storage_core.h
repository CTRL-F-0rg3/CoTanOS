/*
 * ============================================================================
 *  STORAGE_CORE.H - CoTanOS Storage Driver Suite (public C API)
 *  Architecture: RISC-V (RV32 / RV64)
 *  Scope: NVMe (1.4c + 2.0), AHCI/SATA, AAC/AACRAID, unified block layer.
 *
 *  Conventions:
 *    - All structs __packed match on-wire HW layout exactly.
 *    - Async submit/poll model (NVMe-style) used for every controller.
 *    - Zero libc; uses stor_mmio.h + stor_mm.h as foundation.
 *    - All public API returns STOR_OK / STOR_ERR_*.
 *    - No malloc; every buffer is caller-provided or from stor_dma_alloc.
 * ============================================================================
 */

#ifndef STORAGE_CORE_H
#define STORAGE_CORE_H

#include "stor_mmio.h"
#include "stor_mm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* SECTION 0: LOG & ERROR CODES                                               */
/* ========================================================================== */

#define STOR_LOG_ERR     0
#define STOR_LOG_WARN    1
#define STOR_LOG_INFO    2
#define STOR_LOG_DBG     3
#define STOR_LOG_TRACE   4

#ifndef STOR_LOG_LEVEL
#define STOR_LOG_LEVEL STOR_LOG_INFO
#endif

typedef void (*stor_log_fn_t)(int level, const char *fmt, ...);
extern stor_log_fn_t stor_log_fn;

#define stor_log(lvl, fmt, ...) \
    do { if ((lvl) <= STOR_LOG_LEVEL && stor_log_fn) \
        stor_log_fn((lvl), fmt, ##__VA_ARGS__); } while (0)

#define stor_err(fmt, ...)   stor_log(STOR_LOG_ERR,   "[STOR ERR] " fmt, ##__VA_ARGS__)
#define stor_warn(fmt, ...)  stor_log(STOR_LOG_WARN,  "[STOR WRN] " fmt, ##__VA_ARGS__)
#define stor_info(fmt, ...)  stor_log(STOR_LOG_INFO,  "[STOR INF] " fmt, ##__VA_ARGS__)
#define stor_dbg(fmt, ...)   stor_log(STOR_LOG_DBG,   "[STOR DBG] " fmt, ##__VA_ARGS__)
#define stor_trace(fmt, ...) stor_log(STOR_LOG_TRACE, "[STOR TRC] " fmt, ##__VA_ARGS__)

#define STOR_OK                 0
#define STOR_ERR_GENERIC       (-1)
#define STOR_ERR_TIMEOUT       (-2)
#define STOR_ERR_INVAL         (-3)
#define STOR_ERR_NOMEM         (-4)
#define STOR_ERR_BUSY          (-5)
#define STOR_ERR_NOT_SUPPORTED (-6)
#define STOR_ERR_PROTOCOL      (-7)
#define STOR_ERR_NO_DEVICE     (-8)
#define STOR_ERR_IO            (-9)
#define STOR_ERR_CHECKSUM      (-10)
#define STOR_ERR_NOT_READY     (-11)
#define STOR_ERR_ABORTED       (-12)
#define STOR_ERR_RANGE         (-13)
#define STOR_ERR_PERM          (-14)

/* ========================================================================== */
/* SECTION 1: UNIFIED ASYNC BLOCK DEVICE API                                  */
/* ========================================================================== */
/*
 * All controllers expose block devices through this single interface.
 * Modeled on NVMe: submit() is non-blocking, completion fires via callback
 * either from IRQ context or from poll() (polling mode).
 *
 * Required ops: submit, poll.
 * Optional: flush_queue, quiesce, resume, get_health, get_geometry.
 */

typedef enum {
    STOR_BLK_READ = 0,
    STOR_BLK_WRITE,
    STOR_BLK_FLUSH,
    STOR_BLK_DISCARD,
    STOR_BLK_WRITE_ZEROES,
    STOR_BLK_COMPARE,
    STOR_BLK_SECURE_ERASE,
    STOR_BLK_SANITIZE,
    STOR_BLK_ZONE_APPEND,
    STOR_BLK_ZONE_MGMT_SEND,
    STOR_BLK_ZONE_MGMT_RECV,
    STOR_BLK_KV_STORE,
    STOR_BLK_KV_RETRIEVE,
    STOR_BLK_KV_DELETE,
    STOR_BLK_KV_LIST,
    STOR_BLK_KV_EXIST,
    STOR_BLK_PASSTHROUGH,
    STOR_BLK_MAX_OP
} stor_blk_opcode_t;

typedef enum {
    STOR_BLK_TYPE_UNKNOWN = 0,
    STOR_BLK_TYPE_NVME,
    STOR_BLK_TYPE_SATA_HDD,
    STOR_BLK_TYPE_SATA_SSD,
    STOR_BLK_TYPE_AAC_LD,     /* AAC logical volume */
    STOR_BLK_TYPE_AAC_PD,     /* AAC physical device (passthrough) */
} stor_blk_type_t;

typedef struct stor_blk_dev    stor_blk_dev_t;
typedef struct stor_blk_req    stor_blk_req_t;
typedef void (*stor_blk_complete_fn_t)(stor_blk_req_t *req);

struct stor_blk_req {
    stor_blk_dev_t        *dev;
    stor_blk_opcode_t      op;
    u64                    lba;
    u32                    nsect;
    void                  *buf;
    u32                    buf_len;
    u32                    flags;
    void                  *cookie;
    stor_blk_complete_fn_t complete;
    int                    status;

    /* Passthrough raw command (only for STOR_BLK_PASSTHROUGH) */
    union {
        struct { u8 opcode; u8 flags; u32 nsid;
                 void *cmd; void *result; u32 cdw[16]; } nvme;
        struct { u16 cmd; u32 features; u8 device;
                 void *taskfile; } ata;
        struct { u8 cdb[16]; u32 cdb_len; } scsi;
    } passthrough;

    /* ZNS/KV-specific parameters */
    union {
        struct { u64 zone_id; u8 op; u8 all; void *desc; } zone;
        struct { void *key; u32 key_len; u32 value_len; u32 store_opt; } kv;
    } ext;

    /* Driver linkage (do not touch from callers) */
    stor_blk_req_t        *next;
    u64                    submit_time;
    u32                    tag;
};

/* Geometry snapshot returned by get_geometry() */
typedef struct {
    u64 capacity_sectors;
    u32 sector_size;
    u32 physical_block_size;
    u32 logical_block_size;
    u32 max_transfer_sectors;
    u32 discard_granularity;
    u32 queue_depth;
    bool rotational;
    bool removable;
    bool write_cache;
} stor_blk_geometry_t;

/* Health snapshot (SMART/health log) */
typedef struct {
    u8   critical_warning;
    u16  temperature_k;
    u8   available_spare;
    u8   spare_thresh;
    u8   percent_used;
    u64  data_units_read;
    u64  data_units_written;
    u64  host_read_cmds;
    u64  host_write_cmds;
    u64  controller_busy_time;
    u64  power_cycles;
    u64  power_on_hours;
    u64  unsafe_shutdowns;
    u64  media_errors;
    u32  num_err_log_entries;
    bool valid;
} stor_blk_health_t;

struct stor_blk_dev {
    const char *name;          /* e.g. "nvme0n1", "sda", "aac0ld0" */
    const char *serial;
    const char *model;
    const char *fw_rev;

    u64   capacity_sectors;
    u32   sector_size;
    u64   capacity_bytes;

    bool  rotational;
    bool  removable;
    bool  write_cache;
    bool  fua_supported;
    bool  trim_supported;
    bool  secure_erase_supported;
    bool  zoned;

    u32   max_transfer_sectors;
    u32   discard_granularity;
    u32   physical_block_size;
    u32   queue_depth;
    u32   max_hw_sectors;

    u64   zone_size_sectors;
    u64   zone_count;

    stor_blk_type_t type;
    void *priv;                /* driver context (namespace/port/ld) */

    /* Async ops — required */
    int  (*submit)(stor_blk_dev_t *dev, stor_blk_req_t *req);
    int  (*poll)(stor_blk_dev_t *dev, u32 budget);

    /* Optional ops */
    int  (*flush_queue)(stor_blk_dev_t *dev);
    int  (*quiesce)(stor_blk_dev_t *dev);
    int  (*resume)(stor_blk_dev_t *dev);
    int  (*get_health)(stor_blk_dev_t *dev, stor_blk_health_t *out);
    int  (*get_geometry)(stor_blk_dev_t *dev, stor_blk_geometry_t *out);
};

/* ========================================================================== */
/* SECTION 2: PCIe CONFIG SPACE ACCESS                                        */
/* ========================================================================== */

typedef struct {
    u8 bus;
    u8 dev;
    u8 fn;
} stor_pci_addr_t;

typedef struct {
    u8  bus;
    u8  dev;
    u8  fn;
    u16 vendor_id;
    u16 device_id;
    u16 subsystem_vendor_id;
    u16 subsystem_id;
    u8  class_code;
    u8  subclass;
    u8  prog_if;
    u8  revision;
    u8  header_type;
    u8  irq_line;
} stor_pci_device_t;

typedef struct {
    u64 base_phys;
    u64 size;
    bool is_64bit;
    bool is_prefetchable;
    bool is_io;
} stor_pci_bar_t;

/* Platform PCIe ops vtable (ECAM vs legacy CF8/CFC, chosen by kernel). */
typedef struct {
    int  (*read32)(u8 bus, u8 dev, u8 fn, u16 off, u32 *out);
    int  (*write32)(u8 bus, u8 dev, u8 fn, u16 off, u32 v);
} stor_pci_ops_t;

int  stor_pci_init(const stor_pci_ops_t *ops);
bool stor_pci_ready(void);

int  stor_pci_read32(u8 bus, u8 dev, u8 fn, u16 off, u32 *out);
int  stor_pci_write32(u8 bus, u8 dev, u8 fn, u16 off, u32 v);
int  stor_pci_read16(u8 bus, u8 dev, u8 fn, u16 off, u16 *out);
int  stor_pci_write16(u8 bus, u8 dev, u8 fn, u16 off, u16 v);
int  stor_pci_read8(u8 bus, u8 dev, u8 fn, u16 off, u8 *out);
int  stor_pci_write8(u8 bus, u8 dev, u8 fn, u16 off, u8 v);

int  stor_pci_enumerate(u8 class_code, u8 subclass, u8 prog_if,
                        stor_pci_device_t *out, u32 max_out, u32 *found);
int  stor_pci_get_bar(u8 bus, u8 dev, u8 fn, u8 bar_idx, stor_pci_bar_t *out);
int  stor_pci_enable_mem(u8 bus, u8 dev, u8 fn);
int  stor_pci_enable_busmaster(u8 bus, u8 dev, u8 fn);
int  stor_pci_enable_intx_disable(u8 bus, u8 dev, u8 fn);

/* PCIe class codes we care about */
#define STOR_PCI_CLASS_STORAGE          0x01
#define STOR_PCI_SUBCLASS_NVM           0x08   /* NVM Express */
#define STOR_PCI_PROGIF_NVME            0x02
#define STOR_PCI_SUBCLASS_SATA          0x06   /* AHCI 1.0 */
#define STOR_PCI_PROGIF_AHCI            0x01
#define STOR_PCI_SUBCLASS_RAID          0x04
#define STOR_PCI_PROGIF_AACRAID         0x00   /* AAC family */

/* ========================================================================== */
/* SECTION 3: NVMe — REGISTERS, OPCODES, STATUS                                */
/* ========================================================================== */

/* BAR0 register offsets */
#define NVME_REG_CAP        0x0000   /* 64-bit Controller Capabilities */
#define NVME_REG_VS         0x0008   /* Version */
#define NVME_REG_INTMS      0x000C   /* Interrupt Mask Set */
#define NVME_REG_INTMC      0x0010   /* Interrupt Mask Clear */
#define NVME_REG_CC         0x0014   /* Controller Configuration */
#define NVME_REG_CSTS       0x001C   /* Controller Status */
#define NVME_REG_NSSR       0x0020   /* NVM Subsystem Reset */
#define NVME_REG_AQA        0x0024   /* Admin Queue Attributes */
#define NVME_REG_ASQ        0x0028   /* Admin SQ Base Address (64-bit) */
#define NVME_REG_ACQ        0x0030   /* Admin CQ Base Address (64-bit) */
#define NVME_REG_CMBLOC     0x0038
#define NVME_REG_CMBSZ      0x003C
#define NVME_REG_BPINFO     0x0040
#define NVME_REG_BPRSEL     0x0044
#define NVME_REG_BPMBL      0x0048
#define NVME_REG_CMBMSC     0x0050
#define NVME_REG_PMRCAP     0x0E00
#define NVME_REG_PMRCTL     0x0E04
#define NVME_REG_PMRSTS     0x0E08
#define NVME_REG_DBS        0x1000   /* Doorbell base; stride from CAP.DSTRD */

/* CAP bits */
#define NVME_CAP_MQES_SHIFT    0
#define NVME_CAP_MQES_MASK     STOR_GENMASK_ULL(15, 0)
#define NVME_CAP_CQR           STOR_BIT_ULL(16)
#define NVME_CAP_AMS_SHIFT     17
#define NVME_CAP_AMS_MASK      STOR_GENMASK_ULL(18, 17)
#define NVME_CAP_TO_SHIFT      24
#define NVME_CAP_TO_MASK       STOR_GENMASK_ULL(31, 24)
#define NVME_CAP_DSTRD_SHIFT   32
#define NVME_CAP_DSTRD_MASK    STOR_GENMASK_ULL(35, 32)
#define NVME_CAP_NSSRS         STOR_BIT_ULL(36)
#define NVME_CAP_CSS_SHIFT     37
#define NVME_CAP_CSS_MASK      STOR_GENMASK_ULL(44, 37)
#define NVME_CAP_CSS_NVM       (1ULL << 0)
#define NVME_CAP_MPSMIN_SHIFT  48
#define NVME_CAP_MPSMIN_MASK   STOR_GENMASK_ULL(51, 48)
#define NVME_CAP_MPSMAX_SHIFT  52
#define NVME_CAP_MPSMAX_MASK   STOR_GENMASK_ULL(55, 52)
#define NVME_CAP_PMRS          STOR_BIT_ULL(56)
#define NVME_CAP_CMBS          STOR_BIT_ULL(57)
#define NVME_CAP_NSSS          STOR_BIT_ULL(58)
#define NVME_CAP_CRMS_SHIFT    59
#define NVME_CAP_CRMS_MASK     STOR_GENMASK_ULL(60, 59)

/* CC bits */
#define NVME_CC_EN             STOR_BIT(0)
#define NVME_CC_CSS_SHIFT      4
#define NVME_CC_CSS_MASK       STOR_GENMASK(6, 4)
#define NVME_CC_CSS_NVM        (0u << 4)
#define NVME_CC_MPS_SHIFT      7
#define NVME_CC_MPS_MASK       STOR_GENMASK(10, 7)
#define NVME_CC_AMS_SHIFT      11
#define NVME_CC_AMS_MASK       STOR_GENMASK(13, 11)
#define NVME_CC_SHN_SHIFT      14
#define NVME_CC_SHN_MASK       STOR_GENMASK(15, 14)
#define NVME_CC_IOSQES_SHIFT   16
#define NVME_CC_IOSQES_MASK    STOR_GENMASK(19, 16)
#define NVME_CC_IOCQES_SHIFT   20
#define NVME_CC_IOCQES_MASK    STOR_GENMASK(23, 20)

/* CSTS bits */
#define NVME_CSTS_RDY          STOR_BIT(0)
#define NVME_CSTS_CFS          STOR_BIT(1)
#define NVME_CSTS_SHST_SHIFT   2
#define NVME_CSTS_SHST_MASK    STOR_GENMASK(3, 2)
#define NVME_CSTS_NSSRO        STOR_BIT(4)
#define NVME_CSTS_PP           STOR_BIT(5)

/* Submit/CQ entry sizes */
#define NVME_SQE_SIZE          64
#define NVME_CQE_SIZE          16
#define NVME_IDENTIFY_SIZE     4096

/* Doorbell helpers */
#define NVME_DOORBELL_SQ0      0x1000
#define NVME_DOORBELL_CQ0      0x1004
#define NVME_DOORBELL_STRIDE(dstrd)  (4u << (dstrd))

/* Queue depth defaults */
#define NVME_ADMIN_QUEUE_DEPTH  64
#define NVME_IO_QUEUE_DEPTH     1024
#define NVME_MAX_QUEUES         64
#define NVME_MAX_NAMESPACES     32

/* Admin opcodes */
#define NVME_ADMIN_DELETE_SQ          0x00
#define NVME_ADMIN_CREATE_SQ          0x01
#define NVME_ADMIN_GET_LOG_PAGE       0x02
#define NVME_ADMIN_DELETE_CQ          0x04
#define NVME_ADMIN_CREATE_CQ          0x05
#define NVME_ADMIN_IDENTIFY           0x06
#define NVME_ADMIN_ABORT              0x08
#define NVME_ADMIN_SET_FEATURES       0x09
#define NVME_ADMIN_GET_FEATURES       0x0A
#define NVME_ADMIN_ASYNC_EVENT_REQ    0x0C
#define NVME_ADMIN_NS_MGMT            0x0D
#define NVME_ADMIN_FW_COMMIT          0x0E
#define NVME_ADMIN_FW_DOWNLOAD        0x0F
#define NVME_ADMIN_DEVICE_SELF_TEST   0x10
#define NVME_ADMIN_NS_ATTACH          0x11
#define NVME_ADMIN_KEEP_ALIVE         0x14
#define NVME_ADMIN_DIRECTIVE_SEND     0x15
#define NVME_ADMIN_DIRECTIVE_RECV     0x16
#define NVME_ADMIN_VIRT_MGMT          0x18
#define NVME_ADMIN_MI_SEND            0x19
#define NVME_ADMIN_MI_RECV            0x1A
#define NVME_ADMIN_CAP_MGMT           0x1B
#define NVME_ADMIN_LOCKDOWN           0x1C
#define NVME_ADMIN_CLEAR_EXP_NVM_RES  0x1D
#define NVME_ADMIN_DBBUF_CONFIG       0x1E
#define NVME_ADMIN_GET_LBA_STATUS     0x1F
#define NVME_ADMIN_FORMAT_NVM         0x7C
#define NVME_ADMIN_SECURITY_SEND      0x7D
#define NVME_ADMIN_SECURITY_RECV      0x7E
#define NVME_ADMIN_SANITIZE           0x7F

/* NVM opcodes */
#define NVME_NVM_FLUSH                0x00
#define NVME_NVM_WRITE                0x01
#define NVME_NVM_READ                 0x02
#define NVME_NVM_WRITE_UNCORR         0x04
#define NVME_NVM_COMPARE              0x05
#define NVME_NVM_WRITE_ZEROES         0x08
#define NVME_NVM_DSM                  0x09
#define NVME_NVM_VERIFY               0x0C
#define NVME_NVM_RESV_REGISTER        0x0D
#define NVME_NVM_RESV_ACQUIRE         0x0E
#define NVME_NVM_RESV_RELEASE         0x0F
#define NVME_NVM_RESV_REPORT          0x11

/* ZNS opcodes */
#define NVME_ZNS_ZONE_MGMT_SEND       0x79
#define NVME_ZNS_ZONE_MGMT_RECV       0x7A
#define NVME_ZNS_ZONE_APPEND          0x7D

/* KV opcodes */
#define NVME_KV_STORE                 0x80
#define NVME_KV_RETRIEVE              0x81
#define NVME_KV_DELETE                0x82
#define NVME_KV_LIST                  0x83
#define NVME_KV_EXIST                 0x84

/* Identify CNS values */
#define NVME_ID_CNS_NS                0x00
#define NVME_ID_CNS_CTRL              0x01
#define NVME_ID_CNS_NS_LIST           0x02
#define NVME_ID_CNS_NS_DESC_LIST      0x03
#define NVME_ID_CNS_NVM_SET_LIST      0x04
#define NVME_ID_CNS_CSI_CTRL          0x05
#define NVME_ID_CNS_CSI_NS            0x06

/* Get/Set Features IDs */
#define NVME_FEAT_ARBITRATION         0x01
#define NVME_FEAT_POWER_MGMT          0x02
#define NVME_FEAT_LBA_RANGE           0x03
#define NVME_FEAT_TEMP_THRESH         0x04
#define NVME_FEAT_ERR_RECOVERY        0x05
#define NVME_FEAT_VOLATILE_WC         0x06
#define NVME_FEAT_NUM_QUEUES          0x07
#define NVME_FEAT_IRQ_COALESCE        0x08
#define NVME_FEAT_IRQ_CONFIG          0x09
#define NVME_FEAT_WRITE_ATOMIC        0x0A
#define NVME_FEAT_ASYNC_EVENT         0x0B
#define NVME_FEAT_AUTO_PST            0x0C
#define NVME_FEAT_HOST_MEM_BUF        0x0D
#define NVME_FEAT_TIMESTAMP           0x0E
#define NVME_FEAT_KATO                0x0F
#define NVME_FEAT_HCTM                0x10
#define NVME_FEAT_NOPSC               0x11
#define NVME_FEAT_RRL                 0x12
#define NVME_FEAT_PLM_CONFIG          0x13
#define NVME_FEAT_HOST_BEHAVIOR       0x16
#define NVME_FEAT_SANITIZE            0x17

/* Log page IDs */
#define NVME_LOG_ERROR                0x01
#define NVME_LOG_SMART                0x02
#define NVME_LOG_FW_SLOT              0x03
#define NVME_LOG_CHANGED_NS           0x04
#define NVME_LOG_CMD_EFFECTS          0x05
#define NVME_LOG_DEVICE_SELF_TEST     0x06
#define NVME_LOG_TELEMETRY_HOST       0x07
#define NVME_LOG_TELEMETRY_CTRL       0x08
#define NVME_LOG_FW_TELEMETRY         0x09
#define NVME_LOG_ENDURANCE_GROUP      0x0A
#define NVME_LOG_PREDICT_LATENCY      0x0B
#define NVME_LOG_ASYMMETRIC_NS_ACC    0x0C
#define NVME_LOG_PERSISTENT_EVENT     0x0D
#define NVME_LOG_LBA_STATUS           0x0E
#define NVME_LOG_ENDURANCE_EVENT      0x0F
#define NVME_LOG_MEDIA_READ_RECOVERY  0x16
#define NVME_LOG_BOOT_PART            0x15

/* Status codes — generic (SCT=0) */
#define NVME_SC_SUCCESS               0x00
#define NVME_SC_INVALID_OPCODE        0x01
#define NVME_SC_INVALID_FIELD         0x02
#define NVME_SC_CMDID_CONFLICT        0x03
#define NVME_SC_DATA_XFER_ERROR       0x04
#define NVME_SC_POWER_LOSS            0x05
#define NVME_SC_INTERNAL              0x06
#define NVME_SC_ABORT_REQ             0x07
#define NVME_SC_ABORT_SQ_DEL          0x08
#define NVME_SC_ABORT_FAILED_FUSE     0x09
#define NVME_SC_ABORT_MISSING_FUSE    0x0A
#define NVME_SC_INVALID_NS            0x0B
#define NVME_SC_CMD_SEQ_ERROR         0x0C
#define NVME_SC_INVALID_SGL_LAST      0x0D
#define NVME_SC_INVALID_NUM_SGL       0x0E
#define NVME_SC_DATA_SGL_LEN_INVALID  0x0F
#define NVME_SC_METADATA_SGL_LEN      0x10
#define NVME_SC_SGL_DESC_TYPE         0x11
#define NVME_SC_INVALID_CMB           0x12
#define NVME_SC_INVALID_PRP_OFFSET    0x13
#define NVME_SC_ATOMIC_WRITE_UNIT     0x14
#define NVME_SC_OPERATION_DENIED      0x15
#define NVME_SC_SGL_DATA_BLOCK_OFF    0x16
#define NVME_SC_LBA_OUT_OF_RANGE      0x17
#define NVME_SC_CAPACITY_EXCEEDED     0x18
#define NVME_SC_NS_NOT_READY          0x19
#define NVME_SC_RESERVATION_CONFLICT  0x1A
#define NVME_SC_FORMAT_IN_PROGRESS    0x1B

/* Status codes — NVM specific (SCT=0, SC=0x80+) */
#define NVME_SC_WRITE_FAULT           0x80
#define NVME_SC_UNRECOVERED_READ      0x81
#define NVME_SC_GUARD_CHECK           0x82
#define NVME_SC_APP_TAG_CHECK         0x83
#define NVME_SC_REF_TAG_CHECK         0x84
#define NVME_SC_COMPARE_FAILURE       0x85
#define NVME_SC_ACCESS_DENIED         0x86
#define NVME_SC_DEALLOC_READ          0x87

/* SCT field is in bits[11:9] of the CQE status word */
#define NVME_CQE_STATUS_SC_MASK       0x7FF
#define NVME_CQE_STATUS_SCT_SHIFT     9
#define NVME_CQE_STATUS_SCT_MASK      0x07
#define NVME_CQE_STATUS_DNR           STOR_BIT(14)
#define NVME_CQE_STATUS_MORE          STOR_BIT(15)

#define NVME_CQE_STATUS_PHASE         STOR_BIT(0)

/* ========================================================================== */
/* SECTION 4: NVMe — SQE / CQE / COMMON STRUCTURES                            */
/* ========================================================================== */

typedef struct __packed {
    u8  opcode;
    u8  flags;         /* FUSE[1:0] | PSDT[7:6] */
    u16 cid;           /* Command ID */
    u32 nsid;
    u32 cdw2;
    u32 cdw3;
    u64 mptr;          /* Metadata pointer */
    u64 dptr_prp1;     /* PRP1 or SGL address */
    u64 dptr_prp2;     /* PRP2 or SGL length */
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
} stor_nvme_sqe_t;

typedef struct __packed {
    u32 result;
    u32 rsvd;
    u16 sq_head;
    u16 sq_id;
    u16 cid;
    u16 status;        /* bit15..1 = SC | SCT | DNR | MORE, bit0 = P */
} stor_nvme_cqe_t;

/* PRP entry helper macros */
#define NVME_PRP_OFFSET_MASK   0x0FFFu
#define NVME_PRP_PAGE_MASK     0xFFFFFFFFFFFFF000ULL
#define NVME_PRP_PAGE_SIZE     4096u
#define NVME_PRP_LIST_SIZE     4096u

/* Command DWORD 0 generic flags */
#define NVME_CMD_FUSE_SHIFT    0
#define NVME_CMD_FUSE_MASK     STOR_GENMASK(1, 0)
#define NVME_CMD_PSDT_SHIFT    6
#define NVME_CMD_PSDT_MASK     STOR_GENMASK(7, 6)
#define NVME_CMD_PSDT_PRP      0
#define NVME_CMD_PSDT_SGL_MPTR_CONTIG   1
#define NVME_CMD_PSDT_SGL_MPTR_SGL      2

/* ========================================================================== */
/* SECTION 5: NVMe — IDENTIFY STRUCTURES                                      */
/* ========================================================================== */

typedef struct __packed {
    u16 vid;
    u16 ssvid;
    char sn[20];
    char mn[40];
    char fr[8];
    u8  rab;
    u8  ieee[3];
    u8  cmic;
    u8  mdts;
    u16 cntlid;
    u32 ver;
    u32 rtd3r;
    u32 rtd3e;
    u32 oaes;
    u32 ctratt;
    u8  rsvd100[156];
    u16 oacs;
    u8  acl;
    u8  aerl;
    u8  frmw;
    u8  lpa;
    u8  elpe;
    u8  npss;
    u8  avscc;
    u8  apsta;
    u16 wctemp;
    u16 cctemp;
    u16 mtfa;
    u32 hmpre;
    u32 hmmin;
    u8  tnvmcap[16];
    u8  unvmcap[16];
    u32 rpmbs;
    u16 edstt;
    u8  dsto;
    u8  fwug;
    u16 kas;
    u16 hctma;
    u16 mntmt;
    u16 mxtmt;
    u32 sanicap;
    u32 hmminds;
    u16 hmmaxd;
    u8  rsvd338[174];
    u8  sqes;
    u8  cqes;
    u16 maxcmd;
    u32 nn;
    u16 oncs;
    u16 fuses;
    u8  fna;
    u8  vwc;
    u16 awun;
    u16 awupf;
    u8  nvscc;
    u8  nwpc;
    u16 acwu;
    u8  rsvd534[2];
    u32 sgls;
    u32 mnan;
    u8  rsvd544[224];
    u8  subnqn[256];
    u8  rsvd1024[768];
    /* NVM Command Set Specific */
    u8  rsvd1792[256];
    u8  rsvd2048[768];
    u8  nvmsr;
    u8  vwci;
    u8  mec;
    u16 oacs_ext;
    u8  rsvd2824[1064];
    u8  psd[32 * 32];
    u8  vs[1024];
} stor_nvme_id_ctrl_t;

BUILD_BUG_ON(sizeof(stor_nvme_id_ctrl_t) != 4096);

typedef struct __packed {
    u64 nsze;         /* Namespace Size (LBAs) */
    u64 ncap;         /* Namespace Capacity */
    u64 nuse;         /* Namespace Utilization */
    u8  nsfeat;
    u8  nlbaf;
    u8  flbas;
    u8  mc;
    u8  dpc;
    u8  dps;
    u8  nmic;
    u8  rescap;
    u8  fpi;
    u8  dlfeat;
    u16 nawun;
    u16 nawupf;
    u16 nacwu;
    u16 nabsn;
    u16 nabo;
    u16 nabspf;
    u16 noiob;
    u8  nvmcap[16];
    u16 npwg;
    u16 npwa;
    u16 npdg;
    u16 npda;
    u16 nows;
    u16 mssrl;
    u32 mcl;
    u8  msrc;
    u8  rsvd81[11];
    u32 anagrpid;
    u8  rsvd96[3];
    u8  nsattr;
    u16 nvmsetid;
    u16 endgid;
    u64 nguid[2];
    u8  eui64[8];
    u8  lbaf[64];     /* 16 or 64 LBA Format entries (variable) */
    u8  vs[3712];
} stor_nvme_id_ns_t;

BUILD_BUG_ON(sizeof(stor_nvme_id_ns_t) != 4096);

/* LBA Format descriptor (packed in ns.lbaf[]) */
typedef struct __packed {
    u16 ms;           /* Metadata size */
    u8  lbads;        /* LBA Data Size (log2) */
    u8  rp;           /* Relative Performance */
} stor_nvme_lbaf_t;

/* ========================================================================== */
/* SECTION 6: NVMe — LBA formats, admin command payloads                      */
/* ========================================================================== */

/* Get Log Page (Admin) cdw10-13 */
typedef struct __packed {
    u8  lid;
    u8  lsp;
    u16 numdl;        /* NUMD low 16 */
    u32 numdu_rsvd;   /* NUMD high 16 + rsvd (in cdw11) */
    u32 lpoff;        /* Log Page Offset low */
    u32 lpoff_hi;
    u32 csi_lsi;      /* CSI[7:0] | LSI[15:8] */
    u8  rsvd;
} stor_nvme_log_page_params_t;

/* Identify (Admin) cdw10 */
typedef struct __packed {
    u8  cns;
    u8  rsvd;
    u16 cntid;
    u32 nvmsetid_rsvd;
    u8  rsvd20[2];
    u16 csi;
} stor_nvme_identify_params_t;

/* Create I/O CQ cdw10-11 */
typedef struct __packed {
    u16 qid;
    u16 qsize;       /* 0-based; actual depth = qsize+1 */
    u8  pc;
    u8  ien;
    u16 iv;
} stor_nvme_create_cq_params_t;

/* Create I/O SQ cdw10-11 */
typedef struct __packed {
    u16 qid;
    u16 qsize;
    u8  pc;
    u8  qprio;
    u16 cqid;
} stor_nvme_create_sq_params_t;

/* ========================================================================== */
/* SECTION 7: NVMe — SMART / Health / Error log                               */
/* ========================================================================== */

typedef struct __packed {
    u8  critical_warning;
    u16 temperature;
    u8  avail_spare;
    u8  spare_thresh;
    u8  percent_used;
    u8  endu_grp_crit_warn_sumry;
    u8  rsvd7[25];
    u64 data_units_read[2];
    u64 data_units_written[2];
    u64 host_read_cmds[2];
    u64 host_write_cmds[2];
    u64 ctrl_busy_time[2];
    u64 power_cycles[2];
    u64 power_on_hours[2];
    u64 unsafe_shutdowns[2];
    u64 media_errors[2];
    u64 num_err_log_entries[2];
    u32 warning_temp_time;
    u32 critical_comp_time;
    u16 temp_sensor[8];
    u32 thm_temp1_trans_count;
    u32 thm_temp2_trans_count;
    u32 thm_temp1_total_time;
    u32 thm_temp2_total_time;
    u8  rsvd232[280];
} stor_nvme_smart_log_t;

BUILD_BUG_ON(sizeof(stor_nvme_smart_log_t) != 512);

typedef struct __packed {
    u64 error_count;
    u16 sqid;
    u16 cmdid;
    u16 status_field;
    u16 param_error_location;
    u64 lba;
    u32 nsid;
    u8  vs;
    u8  trtype;
    u8  rsvd[2];
    u64 cmd_specific;
    u16 trtype_spec_info;
    u8  rsvd2[22];
} stor_nvme_error_log_entry_t;

/* Firmware slot information log */
typedef struct __packed {
    u8  afi;              /* Active Firmware Slot */
    u8  rsvd1[7];
    u8  frs[7][8];        /* 7 firmware slots (read-only) */
    u8  rsvd64[64];
} stor_nvme_fw_slot_log_t;

/* ========================================================================== */
/* SECTION 8: NVMe 2.0 — ZNS (Zoned Namespace)                                */
/* ========================================================================== */

#define NVME_ZNS_ZONE_STATE_MASK      0x0F
#define NVME_ZNS_ZONE_TYPE_MASK       0xF0
#define NVME_ZNS_ZONE_TYPE_SEQ_WRITE  0x10

#define NVME_ZNS_ZT_EMPTY             0x1
#define NVME_ZNS_ZT_IMPLICIT_OPEN     0x2
#define NVME_ZNS_ZT_EXPLICIT_OPEN     0x3
#define NVME_ZNS_ZT_CLOSED            0x4
#define NVME_ZNS_ZT_READ_ONLY         0xD
#define NVME_ZNS_ZT_FULL              0xE
#define NVME_ZNS_ZT_OFFLINE           0xF

/* Zone Descriptor (64 bytes) */
typedef struct __packed {
    u8  zone_type;
    u8  zone_state;
    u8  zone_attributes;
    u8  rsvd;
    u32 zone_capacity;
    u64 zone_start_lba;
    u64 write_pointer;
    u8  zone_descriptor_extension[16];
} stor_nvme_zns_zone_desc_t;

/* Zone Mgmt Send — Report Zones action */
#define NVME_ZNS_ZMS_REPORT_ZONES     0x00
#define NVME_ZNS_ZMS_CLOSE_ZONE       0x01
#define NVME_ZNS_ZMS_FINISH_ZONE      0x02
#define NVME_ZNS_ZMS_OPEN_ZONE        0x03
#define NVME_ZNS_ZMS_RESET_ZONE       0x04
#define NVME_ZNS_ZMS_OFFLINE_ZONE     0x05
#define NVME_ZNS_ZMS_SET_ZONE_DESC    0x10

/* Report Zones Report Options */
#define NVME_ZNS_ZRA_LIST_ALL         0x00
#define NVME_ZNS_ZRA_LIST_EMPTY       0x01
#define NVME_ZNS_ZRA_LIST_IMP_OPEN    0x02
#define NVME_ZNS_ZRA_LIST_EXP_OPEN    0x03
#define NVME_ZNS_ZRA_LIST_CLOSED      0x04
#define NVME_ZNS_ZRA_LIST_FULL        0x05
#define NVME_ZNS_ZRA_LIST_OFFLINE     0x06
#define NVME_ZNS_ZRA_LIST_RDONLY      0x07
#define NVME_ZNS_ZRA_EXT_DESC         0x80

typedef struct __packed {
    u8  report_option;
    u8  rsvd1[3];
    u64 start_lba;
    u32 num_zones;      /* 0-based number of zones to return */
    u8  rsvd2[4];
} stor_nvme_zns_report_req_t;

/* ========================================================================== */
/* SECTION 9: NVMe 2.0 — KV (Key-Value)                                       */
/* ========================================================================== */

#define NVME_KV_STORE_OPT_INSERT      0x0
#define NVME_KV_STORE_OPT_UPDATE      0x1
#define NVME_KV_STORE_OPT_UPSERT      0x2

typedef struct __packed {
    u16 kv_key_len;
    u16 rsvd;
    u32 value_len;
    u32 kv_store_option;
} stor_nvme_kv_store_cdw_t;

/* ========================================================================== */
/* SECTION 10: NVMe 2.0 — Reservations                                        */
/* ========================================================================== */

#define NVME_RESV_TYPE_WRITE_EXCL     0x1
#define NVME_RESV_TYPE_EXCL_ACCESS    0x2
#define NVME_RESV_TYPE_WRITE_EXCL_REG 0x3
#define NVME_RESV_TYPE_EXCL_ACCESS_REG 0x4
#define NVME_RESV_TYPE_WRITE_EXCL_ALL 0x5
#define NVME_RESV_TYPE_EXCL_ACCESS_ALL 0x6

#define NVME_RESV_REG_ACTION_REGISTER      0x0
#define NVME_RESV_REG_ACTION_UNREGISTER    0x1
#define NVME_RESV_REG_ACTION_REPLACE       0x2

#define NVME_RESV_ACQ_ACTION_ACQUIRE       0x0
#define NVME_RESV_ACQ_ACTION_PREEMPT       0x1
#define NVME_RESV_ACQ_ACTION_ACQUIRE_REL   0x7

typedef struct __packed {
    u64 crkey;
    u64 nrkey;
} stor_nvme_resv_keys_t;

typedef struct __packed {
    u16 num_dwords;     /* 0-based number of dwords to return */
    u8  rsvd1[2];
} stor_nvme_resv_report_req_t;

/* ========================================================================== */
/* SECTION 11: NVMe 2.0 — Directives                                          */
/* ========================================================================== */

#define NVME_DIR_IDENTIFY            0x00
#define NVME_DIR_STREAMS             0x01

#define NVME_DIR_SEND_OP_DIRECTIVE   0x01
#define NVME_DIR_RECV_OP_DIRECTIVE   0x02
#define NVME_DIR_OP_STREAMS_RSRC_REL 0x01
#define NVME_DIR_OP_STREAMS_RSRC_REG 0x02

/* ========================================================================== */
/* SECTION 12: NVMe — Controller & Runtime Structures                         */
/* ========================================================================== */

typedef struct stor_nvme_queue {
    u16 qid;
    u16 depth;
    u16 sq_head;
    u16 sq_tail;
    u16 cq_head;
    u16 cq_tail;
    u16 phase;
    stor_nvme_sqe_t *sq;
    stor_nvme_cqe_t *cq;
    dma_addr_t       sq_phys;
    dma_addr_t       cq_phys;
    volatile u32    *sq_doorbell;
    volatile u32    *cq_doorbell;
} stor_nvme_queue_t;

typedef struct stor_nvme_ctrl {
    stor_pci_device_t pci;
    vaddr_t           bar0;
    u64               bar0_size;

    u64               cap;
    u32               vs;
    u32               dstrd;
    u32               mqes;
    u32               timeout_ms;

    stor_nvme_queue_t admin_q;
    stor_nvme_queue_t io_q[NVME_MAX_QUEUES];
    u16               io_q_count;

    stor_nvme_id_ctrl_t id_ctrl;

    stor_nvme_queue_t *next_cq_completion[NVME_MAX_QUEUES];

    u32               next_cid;
    stor_spinlock_t   lock;
    bool              ready;
    bool              initialized;
    void             *namespace_map[NVME_MAX_NAMESPACES];
    u32               namespace_count;
} stor_nvme_ctrl_t;

typedef struct stor_nvme_ns {
    stor_nvme_ctrl_t   *ctrl;
    u32                 nsid;
    stor_nvme_id_ns_t   id_ns;
    stor_nvme_lbaf_t    lbaf;
    u32                 sector_size;
    u64                 capacity_sectors;
    u32                 format_index;
    bool                zoned;
    u64                 zone_size_sectors;
    u64                 zone_count;

    stor_blk_dev_t      blk;
} stor_nvme_ns_t;

/* ========================================================================== */
/* SECTION 13: AHCI / SATA — Registers                                        */
/* ========================================================================== */

/* HBA register offsets (BAR5 = ABAR) */
#define AHCI_REG_CAP         0x0000
#define AHCI_REG_GHC         0x0004
#define AHCI_REG_IS          0x0008
#define AHCI_REG_PI          0x000C
#define AHCI_REG_VS          0x0010
#define AHCI_REG_CCC_CTL     0x0014
#define AHCI_REG_CCC_PORTS   0x0018
#define AHCI_REG_EM_LOC      0x001C
#define AHCI_REG_EM_CTL      0x0020
#define AHCI_REG_CAP2        0x0024
#define AHCI_REG_BOHC        0x0028

/* Port register block base: 0x100 + port * 0x80 */
#define AHCI_PORT_BASE(p)    (0x100u + ((p) * 0x80u))

#define AHCI_PX_CLB          0x00
#define AHCI_PX_CLBU         0x04
#define AHCI_PX_FB           0x08
#define AHCI_PX_FBU          0x0C
#define AHCI_PX_IS           0x10
#define AHCI_PX_IE           0x14
#define AHCI_PX_CMD          0x18
#define AHCI_PX_TFD          0x20
#define AHCI_PX_SIG          0x24
#define AHCI_PX_SSTS         0x28
#define AHCI_PX_SCTL         0x2C
#define AHCI_PX_SERR         0x30
#define AHCI_PX_SACT         0x34
#define AHCI_PX_CI           0x38
#define AHCI_PX_SNTF         0x3C
#define AHCI_PX_FBS          0x40
#define AHCI_PX_DEVSLP       0x44
#define AHCI_PX_VS           0x70

/* GHC bits */
#define AHCI_GHC_HR          STOR_BIT(0)
#define AHCI_GHC_IE          STOR_BIT(1)
#define AHCI_GHC_AE          STOR_BIT(31)

/* CAP bits */
#define AHCI_CAP_S64A        STOR_BIT(31)
#define AHCI_CAP_SNCQ        STOR_BIT(30)
#define AHCI_CAP_SSNTF       STOR_BIT(29)
#define AHCI_CAP_SMPS        STOR_BIT(28)
#define AHCI_CAP_SSS         STOR_BIT(27)
#define AHCI_CAP_SALP        STOR_BIT(26)
#define AHCI_CAP_SAL         STOR_BIT(25)
#define AHCI_CAP_SCLO        STOR_BIT(24)
#define AHCI_CAP_ISS_SHIFT   20
#define AHCI_CAP_ISS_MASK    STOR_GENMASK(23, 20)
#define AHCI_CAP_SAM         STOR_BIT(18)
#define AHCI_CAP_SPM         STOR_BIT(17)
#define AHCI_CAP_FBSS        STOR_BIT(16)
#define AHCI_CAP_PMD         STOR_BIT(15)
#define AHCI_CAP_SSC         STOR_BIT(14)
#define AHCI_CAP_PSC         STOR_BIT(13)
#define AHCI_CAP_NCS_SHIFT   8
#define AHCI_CAP_NCS_MASK    STOR_GENMASK(12, 8)
#define AHCI_CAP_CCCS        STOR_BIT(7)
#define AHCI_CAP_NP_SHIFT    0
#define AHCI_CAP_NP_MASK     STOR_GENMASK(4, 0)

/* PxCMD bits */
#define AHCI_PX_CMD_ST       STOR_BIT(0)
#define AHCI_PX_CMD_SUD      STOR_BIT(1)
#define AHCI_PX_CMD_POD      STOR_BIT(2)
#define AHCI_PX_CMD_CLO      STOR_BIT(3)
#define AHCI_PX_CMD_FRE      STOR_BIT(4)
#define AHCI_PX_CMD_CCS_SHIFT 8
#define AHCI_PX_CMD_CCS_MASK STOR_GENMASK(12, 8)
#define AHCI_PX_CMD_MPSS     STOR_BIT(13)
#define AHCI_PX_CMD_FR       STOR_BIT(14)
#define AHCI_PX_CMD_CR       STOR_BIT(15)
#define AHCI_PX_CMD_CPS      STOR_BIT(16)
#define AHCI_PX_CMD_PMA      STOR_BIT(17)
#define AHCI_PX_CMD_HPCP     STOR_BIT(18)
#define AHCI_PX_CMD_MPSP     STOR_BIT(19)
#define AHCI_PX_CMD_CPD      STOR_BIT(20)
#define AHCI_PX_CMD_ESP      STOR_BIT(21)
#define AHCI_PX_CMD_FBSCP    STOR_BIT(22)
#define AHCI_PX_CMD_APSTE    STOR_BIT(23)
#define AHCI_PX_CMD_ATAPI    STOR_BIT(24)
#define AHCI_PX_CMD_DLAE     STOR_BIT(25)
#define AHCI_PX_CMD_ALPE     STOR_BIT(26)
#define AHCI_PX_CMD_ASP      STOR_BIT(27)
#define AHCI_PX_CMD_ICC_SHIFT 28
#define AHCI_PX_CMD_ICC_MASK STOR_GENMASK(31, 28)

/* PxIS / PxIE bits */
#define AHCI_PX_IS_DHRS      STOR_BIT(0)
#define AHCI_PX_IS_PSS       STOR_BIT(1)
#define AHCI_PX_IS_DSS       STOR_BIT(2)
#define AHCI_PX_IS_SDBS      STOR_BIT(3)
#define AHCI_PX_IS_UFS       STOR_BIT(4)
#define AHCI_PX_IS_DPS       STOR_BIT(5)
#define AHCI_PX_IS_PCS       STOR_BIT(6)
#define AHCI_PX_IS_DMPS      STOR_BIT(7)
#define AHCI_PX_IS_PRCS      STOR_BIT(22)
#define AHCI_PX_IS_IPMS      STOR_BIT(23)
#define AHCI_PX_IS_OFS       STOR_BIT(24)
#define AHCI_PX_IS_INFS      STOR_BIT(26)
#define AHCI_PX_IS_IFS       STOR_BIT(27)
#define AHCI_PX_IS_HBDS      STOR_BIT(28)
#define AHCI_PX_IS_HBFS      STOR_BIT(29)
#define AHCI_PX_IS_TFES      STOR_BIT(30)
#define AHCI_PX_IS_CPDS      STOR_BIT(31)

/* PxSSTS */
#define AHCI_PX_SSTS_DET_MASK     0x0F
#define AHCI_PX_SSTS_DET_NONE     0x0
#define AHCI_PX_SSTS_DET_PRESENT  0x1
#define AHCI_PX_SSTS_DET_PHYOFF   0x3
#define AHCI_PX_SSTS_DET_OFFLINE  0x4
#define AHCI_PX_SSTS_IPM_MASK     0xF0
#define AHCI_PX_SSTS_IPM_ACTIVE   0x10
#define AHCI_PX_SSTS_SPD_MASK     0xF00
#define AHCI_PX_SSTS_SPD_GEN1     0x100
#define AHCI_PX_SSTS_SPD_GEN2     0x200
#define AHCI_PX_SSTS_SPD_GEN3     0x300

/* PxSIG */
#define AHCI_PX_SIG_ATA           0x00000101
#define AHCI_PX_SIG_ATAPI         0xEB140101
#define AHCI_PX_SIG_SEMB          0xC33C0101
#define AHCI_PX_SIG_PM            0x96690101

/* PxTFD */
#define AHCI_PX_TFD_STS_ERR       STOR_BIT(0)
#define AHCI_PX_TFD_STS_DRQ       STOR_BIT(3)
#define AHCI_PX_TFD_STS_BSY       STOR_BIT(7)

#define AHCI_PORT_CMD_LIST_SIZE   1024
#define AHCI_PORT_FIS_SIZE        256
#define AHCI_CMD_TABLE_MAX        0x1000
#define AHCI_MAX_PORTS            32
#define AHCI_NCQ_MAX_TAGS         32

/* ========================================================================== */
/* SECTION 14: AHCI — Command structures                                      */
/* ========================================================================== */

typedef struct __packed {
    u8  fis[64];
    u8  acmd[32];
    u8  rsvd[32];
    /* PRDT[0..N-1] follows */
} stor_ahci_cmd_table_t;

typedef struct __packed {
    u8  cfl;           /* Command FIS length in dwords */
    u8  flags;         /* bit0: ATAPI, bit1: write, bit2: prefetch */
    u16 prdtl;         /* PRDT length */
    u32 prdbc;
    u64 ctba;
} stor_ahci_cmd_header_t;

BUILD_BUG_ON(sizeof(stor_ahci_cmd_header_t) != 32);

typedef struct __packed {
    u64 dba;
    u32 rsvd;
    u32 flags;         /* DBC[21:0] | I[31] */
} stor_ahci_prdt_entry_t;

BUILD_BUG_ON(sizeof(stor_ahci_prdt_entry_t) != 16);

typedef struct __packed {
    u32 flags;
    u32 features_lo;
    u32 features_hi;
    u64 base_phys;
    u32 rsvd;
} stor_ahci_cmd_header_ex_t;

/* FIS types */
#define FIS_TYPE_REG_H2D      0x27
#define FIS_TYPE_REG_D2H      0x34
#define FIS_TYPE_DMA_ACT      0x39
#define FIS_TYPE_DMA_SETUP    0x41
#define FIS_TYPE_DATA         0x46
#define FIS_TYPE_BIST         0x58
#define FIS_TYPE_PIO_SETUP    0x5F
#define FIS_TYPE_DEV_BITS     0xA1

typedef struct __packed {
    u8  fis_type;
    u8  pmport;
    u8  command;
    u8  featurel;
    u8  lba0;
    u8  lba1;
    u8  lba2;
    u8  device;
    u8  lba3;
    u8  lba4;
    u8  lba5;
    u8  featureh;
    u8  countl;
    u8  counth;
    u8  icc;
    u8  control;
    u8  rsvd[4];
} stor_fis_reg_h2d_t;

BUILD_BUG_ON(sizeof(stor_fis_reg_h2d_t) != 20);

typedef struct __packed {
    u8  fis_type;
    u8  pmport;
    u8  status;
    u8  error;
    u8  lba0;
    u8  lba1;
    u8  lba2;
    u8  device;
    u8  lba3;
    u8  lba4;
    u8  lba5;
    u8  rsvd1;
    u8  countl;
    u8  counth;
    u8  rsvd2[2];
    u8  rsvd3[4];
} stor_fis_reg_d2h_t;

typedef struct __packed {
    u8  fis_type;
    u8  pmport;
    u8  rsvd0;
    u8  countl;
    u8  counth;
    u8  rsvd1;
    u8  status;
    u8  error;
    u8  rsvd2[8];
} stor_fis_dev_bits_t;

typedef struct __packed {
    u8  fis_type;
    u8  pmport;
    u8  status;
    u8  error;
    u8  lba0;
    u8  lba1;
    u8  lba2;
    u8  device;
    u8  lba3;
    u8  lba4;
    u8  lba5;
    u8  rsvd;
    u8  countl;
    u8  counth;
    u8  rsvd2;
    u8  e_status;
    u16 transfer_count;
    u16 rsvd3;
} stor_fis_pio_setup_t;

typedef struct __packed {
    u8  fis_type;
    u8  pmport;
    u8  rsvd[18];
} stor_fis_dma_setup_t;

typedef struct __packed {
    u8  fis_type;
    u8  rsvd0;
    u8  status;
    u8  error;
    u32 rsvd1[2];
} stor_fis_dma_activate_t;

/* ========================================================================== */
/* SECTION 15: ATA — commands & IDENTIFY words                                */
/* ========================================================================== */

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_READ_FPDMA_QUEUED 0x60
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_WRITE_FPDMA_QUEUED 0x61
#define ATA_CMD_READ_VERIFY       0x40
#define ATA_CMD_READ_VERIFY_EXT   0x42
#define ATA_CMD_WRITE_UNCORR_EXT  0x45
#define ATA_CMD_READ_LOG_EXT      0x2F
#define ATA_CMD_WRITE_LOG_EXT     0x3F
#define ATA_CMD_IDENTIFY          0xEC
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY_DMA      0xEE
#define ATA_CMD_SET_FEATURES      0xEF
#define ATA_CMD_FLUSH_CACHE       0xE7
#define ATA_CMD_FLUSH_CACHE_EXT   0xEA
#define ATA_CMD_DATA_SET_MGMT     0x06
#define ATA_CMD_TRUSTED_SEND      0x5B
#define ATA_CMD_TRUSTED_RECV      0x5C
#define ATA_CMD_SECURITY_SET_PASS 0xF1
#define ATA_CMD_SECURITY_UNLOCK   0xF2
#define ATA_CMD_SECURITY_ERASE_PREP 0xF3
#define ATA_CMD_SECURITY_ERASE_UNIT 0xF4
#define ATA_CMD_SECURITY_FREEZE   0xF5
#define ATA_CMD_SECURITY_DISABLE  0xF6
#define ATA_CMD_SANITIZE          0xB4
#define ATA_CMD_SMART             0xB0
#define ATA_CMD_NCQ_NON_DATA      0x63
#define ATA_CMD_NCQ_QUEUE_MGMT    0x63

/* IDENTIFY DEVICE word offsets */
#define ATA_ID_WORDS              256
#define ATA_ID_SERIAL             10    /* 20 bytes, big-endian word order */
#define ATA_ID_FW_REV             23
#define ATA_ID_MODEL              27
#define ATA_ID_MULTI_SECTOR       47
#define ATA_ID_CAPABILITIES       49
#define ATA_ID_FIELD_VALID        53
#define ATA_ID_LBA_CAPACITY_LO    60    /* if word49 bit9 == 1 */
#define ATA_ID_LBA_CAPACITY_HI    61
#define ATA_ID_LBA_CAPACITY_EXT_LO 100
#define ATA_ID_LBA_CAPACITY_EXT_HI 101
#define ATA_ID_LOGICAL_SECTOR_SIZE 106  /* words 106-107 */
#define ATA_ID_PHYSICAL_SECTOR_SIZE 106 /* top 4 bits if bit14 of word106 */
#define ATA_ID_ROTATION_RATE      217   /* word 217: 1 = SSD, 0x0401-0xFFFE = RPM */
#define ATA_ID_SATA_CAPABILITIES  76
#define ATA_ID_MAJOR_VER          80
#define ATA_ID_MINOR_VER          81
#define ATA_ID_COMMAND_SET_SUPP   82
#define ATA_ID_COMMAND_SET_ACTIVE 83
#define ATA_ID_NCQ                76    /* bit 8: NCQ supported (SATA FIS-based) */
#define ATA_ID_QUEUE_DEPTH        75
#define ATA_ID_TRIM_SUPPORT       169   /* bit 0: DATA SET MANAGEMENT TRIM supported */

/* ========================================================================== */
/* SECTION 16: AHCI — Controller & port runtime                               */
/* ========================================================================== */

typedef struct stor_ahci_port {
    u32 port_num;
    vaddr_t port_base;
    u32 signature;
    u32 ssts;

    stor_ahci_cmd_header_t *cmd_list;
    dma_addr_t               cmd_list_phys;
    u8                      *fis_rx;
    dma_addr_t               fis_rx_phys;
    stor_ahci_cmd_table_t   *cmd_tables[AHCI_NCQ_MAX_TAGS];
    dma_addr_t               cmd_tables_phys[AHCI_NCQ_MAX_TAGS];

    u32  slot_in_use;         /* bitmask of used command slots */
    u32  next_tag;
    stor_spinlock_t lock;

    bool ready;
    bool is_atapi;

    stor_blk_dev_t blk;
    void *identify_buf;
    dma_addr_t identify_phys;
} stor_ahci_port_t;

typedef struct stor_ahci_hba {
    stor_pci_device_t  pci;
    vaddr_t            abar;
    u64                abar_size;

    u32 cap;
    u32 cap2;
    u32 pi;
    u32 ncs;
    u32 n_ports;

    stor_ahci_port_t ports[AHCI_MAX_PORTS];
    u32 n_ports_present;

    stor_spinlock_t lock;
    bool ready;
    bool initialized;
} stor_ahci_hba_t;

/* ========================================================================== */
/* SECTION 17: AAC / AACRAID                                                  */
/* ========================================================================== */

/* FIB header (per aacraid spec, host-side) */
#define AAC_FIB_SIZE              512
#define AAC_MAX_FIB_SIZE          4096
#define AAC_SENSE_BUFFERSIZE      96

#define AAC_FIB_FLAG_ASYNC        STOR_BIT(0)
#define AAC_FIB_FLAG_DATA_DIR     STOR_GENMASK(1, 2)

#define AAC_CMD_READ              0x02
#define AAC_CMD_WRITE             0x03
#define AAC_CMD_READ_64           0x04
#define AAC_CMD_WRITE_64          0x05
#define AAC_CMD_SCSI_EXEC         0x06
#define AAC_CMD_FIB_CONT          0x07
#define AAC_CMD_READ_64_BLK       0x0A
#define AAC_CMD_WRITE_64_BLK      0x0B

#define AAC_ADAPTER_FLAGS_MSI     STOR_BIT(0)
#define AAC_ADAPTER_FLAGS_SAFTE   STOR_BIT(1)

#define AAC_SMART_READ            0x2A
#define AAC_SMART_WRITE           0x2B

/* AAC mailbox commands (subset) */
#define AAC_MB_INIT_STRUCT        0x01
#define AAC_MB_ADAPTER_INFO       0x04
#define AAC_MB_READ_HOST_TIME     0x05
#define AAC_MB_QUERY_DEV          0x08
#define AAC_MB_GET_CTLR_INFO      0x0A
#define AAC_MB_LOOKUP_LD          0x09
#define AAC_MB_GET_DEV_INFO       0x0C
#define AAC_MB_RESET_ADAPTER      0x0E
#define AAC_MB_FLUSH_ADAPTER      0x10
#define AAC_MB_SEND_FIB           0x11
#define AAC_MB_MANAGE_SMART       0x13
#define AAC_MB_ABORT_FIB          0x18

typedef struct __packed {
    u32 cmd;                /* AAC_CMD_xxx */
    u32 flags;              /* bit 0=async, bits 1-2=data dir */
    u32 size;               /* Data size in bytes */
    u32 sgl_number;         /* Number of SGL entries */
    u32 lba;                /* Starting LBA */
    u32 handle;             /* Device handle (LD/PD) */
    u32 count;              /* Sector count */
    u32 rsvd[8];
} stor_aac_fib_header_t;

typedef struct __packed {
    u64 sgl_addr;
    u32 sgl_length;
    u32 sgl_flags;
} stor_aac_sgl_t;

typedef struct stor_aac_fib {
    stor_aac_fib_header_t hdr;
    stor_aac_sgl_t        sgl[16];
    u8                    sense[16];
    u8                    rsvd[16];
    u8                    body[AAC_FIB_SIZE - sizeof(stor_aac_fib_header_t)
                                          - (16 * sizeof(stor_aac_sgl_t))
                                          - 32];
} stor_aac_fib_t;

BUILD_BUG_ON(sizeof(stor_aac_fib_t) != AAC_FIB_SIZE);

/* Adapter info (from AAC_MB_ADAPTER_INFO) */
typedef struct __packed {
    u32    bsize;
    u32    fw_version;
    u32    feature_flags;
    u32    max_fibs;
    u32    max_aif;
    u32    max_io_size;
    u32    max_devices;
    u8     model[32];
    u8     serial[32];
    u32    adapter_flags;
} stor_aac_adapter_info_t;

typedef struct stor_aac_hba {
    stor_pci_device_t   pci;
    vaddr_t             bar;
    u64                 bar_size;

    stor_aac_adapter_info_t info;
    u32                 max_fibs;
    u32                 max_sg;

    /* FIB queue (host-side) */
    stor_aac_fib_t     *fibs;
    dma_addr_t          fibs_phys;
    u32                 next_fib;
    u32                 inflight;
    stor_spinlock_t     lock;

    stor_blk_dev_t      lds[16];
    u32                 ld_count;

    bool ready;
    bool initialized;
} stor_aac_hba_t;

/* ========================================================================== */
/* SECTION 18: PUBLIC API — NVMe                                              */
/* ========================================================================== */

int  stor_nvme_probe(stor_pci_device_t *pci, stor_nvme_ctrl_t *ctrl);
int  stor_nvme_init(stor_nvme_ctrl_t *ctrl);
int  stor_nvme_reset(stor_nvme_ctrl_t *ctrl);
int  stor_nvme_shutdown(stor_nvme_ctrl_t *ctrl, bool notify);
int  stor_nvme_disable(stor_nvme_ctrl_t *ctrl);

int  stor_nvme_create_io_queues(stor_nvme_ctrl_t *ctrl, u16 n_queues,
                                 u16 depth);
int  stor_nvme_delete_io_queues(stor_nvme_ctrl_t *ctrl);
int  stor_nvme_admin_cmd(stor_nvme_ctrl_t *ctrl, const stor_nvme_sqe_t *sqe,
                          stor_nvme_cqe_t *cqe, u32 timeout_ms);

int  stor_nvme_identify_ctrl(stor_nvme_ctrl_t *ctrl,
                             stor_nvme_id_ctrl_t *out);
int  stor_nvme_identify_ns(stor_nvme_ctrl_t *ctrl, u32 nsid,
                           stor_nvme_id_ns_t *out);
int  stor_nvme_scan_namespaces(stor_nvme_ctrl_t *ctrl);
int  stor_nvme_get_smart(stor_nvme_ctrl_t *ctrl, stor_nvme_smart_log_t *out);
int  stor_nvme_get_error_log(stor_nvme_ctrl_t *ctrl, u32 entries,
                             stor_nvme_error_log_entry_t *out);
int  stor_nvme_get_fw_slots(stor_nvme_ctrl_t *ctrl,
                            stor_nvme_fw_slot_log_t *out);
int  stor_nvme_fw_commit(stor_nvme_ctrl_t *ctrl, u8 slot, u8 action);

int  stor_nvme_submit_io(stor_nvme_ctrl_t *ctrl, u16 qid,
                          const stor_nvme_sqe_t *sqe);
int  stor_nvme_poll_completions(stor_nvme_ctrl_t *ctrl, u16 qid, u32 budget);

/* ZNS */
int  stor_nvme_zns_report_zones(stor_nvme_ns_t *ns, u64 start_lba,
                                 u32 num_zones, u8 report_opt,
                                 stor_nvme_zns_zone_desc_t *out, u32 *got);
int  stor_nvme_zns_zone_mgmt(stor_nvme_ns_t *ns, u64 zone_id, u8 action,
                              bool all_zones);
int  stor_nvme_zns_append(stor_nvme_ns_t *ns, u64 zone_id,
                           const void *buf, u32 bytes,
                           u64 *result_lba);

/* Reservations */
int  stor_nvme_resv_register(stor_nvme_ctrl_t *ctrl, u32 nsid,
                              u64 crkey, u64 nrkey, u8 action);
int  stor_nvme_resv_acquire(stor_nvme_ctrl_t *ctrl, u32 nsid,
                             u64 crkey, u8 type, u8 action);
int  stor_nvme_resv_release(stor_nvme_ctrl_t *ctrl, u32 nsid,
                             u64 crkey, u8 type, u8 action);
int  stor_nvme_resv_report(stor_nvme_ctrl_t *ctrl, u32 nsid,
                            void *out, u32 num_dw);

/* SR-IOV */
int  stor_nvme_sriov_enable(stor_nvme_ctrl_t *ctrl, u16 num_vfs);
int  stor_nvme_sriov_disable(stor_nvme_ctrl_t *ctrl);
int  stor_nvme_sriov_configure_vf(stor_nvme_ctrl_t *ctrl, u16 vf_idx);

/* ========================================================================== */
/* SECTION 19: PUBLIC API — AHCI/SATA                                         */
/* ========================================================================== */

int  stor_ahci_probe(stor_pci_device_t *pci, stor_ahci_hba_t *hba);
int  stor_ahci_init(stor_ahci_hba_t *hba);
int  stor_ahci_reset(stor_ahci_hba_t *hba);
int  stor_ahci_shutdown(stor_ahci_hba_t *hba);

int  stor_ahci_port_init(stor_ahci_hba_t *hba, u32 port);
int  stor_ahci_port_reset(stor_ahci_hba_t *hba, u32 port);
int  stor_ahci_port_identify(stor_ahci_hba_t *hba, u32 port,
                              u16 identify[ATA_ID_WORDS]);
int  stor_ahci_port_read_dma(stor_ahci_hba_t *hba, u32 port,
                              u64 lba, u32 count, void *buf,
                              dma_addr_t buf_phys, u32 timeout_ms);
int  stor_ahci_port_write_dma(stor_ahci_hba_t *hba, u32 port,
                               u64 lba, u32 count, const void *buf,
                               dma_addr_t buf_phys, u32 timeout_ms);
int  stor_ahci_port_ncq_read(stor_ahci_hba_t *hba, u32 port,
                              u64 lba, u32 count, void *buf,
                              dma_addr_t buf_phys);
int  stor_ahci_port_ncq_write(stor_ahci_hba_t *hba, u32 port,
                               u64 lba, u32 count, const void *buf,
                               dma_addr_t buf_phys);
int  stor_ahci_port_flush(stor_ahci_hba_t *hba, u32 port);
int  stor_ahci_port_trim(stor_ahci_hba_t *hba, u32 port,
                          const void *ranges, u32 range_bytes,
                          dma_addr_t ranges_phys);
int  stor_ahci_port_smart(stor_ahci_hba_t *hba, u32 port, u8 *out, u32 len);
int  stor_ahci_port_secure_erase(stor_ahci_hba_t *hba, u32 port,
                                  const u8 password[32], bool enhanced);
int  stor_ahci_port_sanitize(stor_ahci_hba_t *hba, u32 port, u8 feature);

int  stor_ahci_scan_ports(stor_ahci_hba_t *hba);
int  stor_ahci_handle_hotplug(stor_ahci_hba_t *hba, u32 port);
int  stor_ahci_poll_port(stor_ahci_hba_t *hba, u32 port);

/* ========================================================================== */
/* SECTION 20: PUBLIC API — AAC                                               */
/* ========================================================================== */

int  stor_aac_probe(stor_pci_device_t *pci, stor_aac_hba_t *hba);
int  stor_aac_init(stor_aac_hba_t *hba);
int  stor_aac_reset(stor_aac_hba_t *hba);
int  stor_aac_get_info(stor_aac_hba_t *hba);
int  stor_aac_query_devices(stor_aac_hba_t *hba);

int  stor_aac_ld_read(stor_aac_hba_t *hba, u32 ld_handle,
                       u64 lba, u32 count, void *buf, dma_addr_t buf_phys);
int  stor_aac_ld_write(stor_aac_hba_t *hba, u32 ld_handle,
                        u64 lba, u32 count, const void *buf, dma_addr_t buf_phys);
int  stor_aac_ld_flush(stor_aac_hba_t *hba, u32 ld_handle);
int  stor_aac_pd_smart(stor_aac_hba_t *hba, u32 pd_handle, u8 *out, u32 len);
int  stor_aac_pd_passthrough(stor_aac_hba_t *hba, u32 pd_handle,
                              const u8 cdb[16], void *buf, dma_addr_t buf_phys,
                              u32 buf_len, bool write);

int  stor_aac_poll(stor_aac_hba_t *hba, u32 budget);

/* ========================================================================== */
/* SECTION 21: PUBLIC API — Unified block layer                               */
/* ========================================================================== */

int  stor_blk_submit(stor_blk_dev_t *dev, stor_blk_req_t *req);
int  stor_blk_poll(stor_blk_dev_t *dev, u32 budget);
int  stor_blk_flush(stor_blk_dev_t *dev);
int  stor_blk_quiesce(stor_blk_dev_t *dev);
int  stor_blk_resume(stor_blk_dev_t *dev);
int  stor_blk_health(stor_blk_dev_t *dev, stor_blk_health_t *out);
int  stor_blk_geometry(stor_blk_dev_t *dev, stor_blk_geometry_t *out);

/* Synchronous one-shot wrappers (convenience; internally use async + spin) */
int  stor_blk_read_sync(stor_blk_dev_t *dev, u64 lba, u32 nsect, void *buf);
int  stor_blk_write_sync(stor_blk_dev_t *dev, u64 lba, u32 nsect, const void *buf);
int  stor_blk_flush_sync(stor_blk_dev_t *dev);
int  stor_blk_discard_sync(stor_blk_dev_t *dev, u64 lba, u32 nsect);

/* ========================================================================== */
/* SECTION 22: SELFTEST ENTRY                                                 */
/* ========================================================================== */

typedef struct {
    stor_nvme_ctrl_t *nvme;
    stor_ahci_hba_t  *ahci;
    stor_aac_hba_t   *aac;
} stor_hw_context_t;

void stor_selftest_set_hw_context(stor_hw_context_t *ctx);
int  stor_selftest_run_all(void);
int  stor_selftest_run_by_name(const char *name);
void stor_selftest_list(void);

#ifdef __cplusplus
}
#endif

#endif /* STORAGE_CORE_H */
