// regs.rs — Adaptec AAC RAID register offsets and doorbell bit flags.

// Device register block (Bar0) sub-registers.
pub mod rx {
    pub const IDBR: u32 = 0x00;
    pub const ODBR: u32 = 0x04;
    pub const FWSTATUS: u32 = 0x08;
    pub const FW_STATUS: u32 = 0x08;
    pub const MAILBOX: u32 = 0x50;
    pub const OIMR: u32 = 0x3C;
    pub const STS_UP_AND_RUNNING: u32 = 0x00000002;
    pub const BASE_PTR_LO: u32 = 0x10;
    pub const BASE_PTR_HI: u32 = 0x14;
    pub const INBOUND_DOORBELL: u32 = 0x00;
    pub const DB_INIT: u32 = 0x00000001;
    pub const OUTBOUND_DOORBELL: u32 = 0x04;
    pub const DB_INIT_ACK: u32 = 0x00000001;
    pub const OUT_INTR_MASK: u32 = 0x3C;
    pub const DB_COMMAND_READY: u32 = 0x00000008;
    pub const DB_RESPONSE_READY: u32 = 0x00000002;
    pub const DB_PRINTF: u32 = 0x00000004;
}

// Top-level doorbell / status bits (transcribed from aacreg.h / FreeBSD aac driver).
pub const IDBR: u32 = 0x00;
pub const ODBR: u32 = 0x04;
pub const FWSTATUS: u32 = 0x08;
pub const FW_STATUS: u32 = FWSTATUS;
pub const MAILBOX: u32 = 0x50;
pub const OIMR: u32 = 0x3C;

pub const AAC_SELF_TEST_FAILED: u32 = 0x00000001;
pub const AAC_UP_AND_RUNNING: u32 = 0x00000002;
pub const STS_UP_AND_RUNNING: u32 = AAC_UP_AND_RUNNING;

pub const DB_SYNC_COMMAND: u32 = 0x00000001;
pub const DB_INIT: u32 = DB_SYNC_COMMAND;
pub const DB_RESPONSE_READY: u32 = 0x00000002;
pub const DB_PRINTF: u32 = 0x00000004;
pub const DB_COMMAND_READY: u32 = 0x00000008;
