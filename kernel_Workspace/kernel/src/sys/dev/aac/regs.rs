// regs.rs — Adaptec AAC RAID register offsets and doorbell bit flags.

/// Inbound Doorbell Register — host rings this to tell the adapter about new commands.
pub const IDBR: u32 = 0x00;
/// Outbound Doorbell Register — adapter rings this to tell the host about responses.
pub const ODBR: u32 = 0x04;
/// Firmware Status Register.
pub const FWSTATUS: u32 = 0x08;
/// Mailbox area for sync FIB submission.
pub const MAILBOX: u32 = 0x50;
/// Outbound Interrupt Mask Register.
pub const OIMR: u32 = 0x3C;

// Legacy aliases used elsewhere in the driver.
pub const FW_STATUS: u32 = FWSTATUS;
pub const STS_UP_AND_RUNNING: u32 = AAC_UP_AND_RUNNING;
pub const BASE_PTR_LO: u32 = 0x10;
pub const BASE_PTR_HI: u32 = 0x14;
pub const INBOUND_DOORBELL: u32 = IDBR;
pub const DB_INIT: u32 = DB_SYNC_COMMAND;
pub const OUTBOUND_DOORBELL: u32 = ODBR;
pub const DB_INIT_ACK: u32 = DB_SYNC_COMMAND;
pub const OUT_INTR_MASK: u32 = OIMR;
pub const DB_COMMAND_READY_ALIAS: u32 = DB_COMMAND_READY;

// Device register block (Bar0) sub-registers.
pub mod rx {
    pub use super::{IDBR, ODBR, FWSTATUS, MAILBOX, OIMR};
    pub use super::{FW_STATUS as FW_STATUS, STS_UP_AND_RUNNING as STS_UP_AND_RUNNING};
    pub use super::{INBOUND_DOORBELL as INBOUND_DOORBELL, DB_INIT as DB_INIT};
    pub use super::{OUTBOUND_DOORBELL as OUTBOUND_DOORBELL, DB_INIT_ACK as DB_INIT_ACK};
    pub use super::{OUT_INTR_MASK as OUT_INTR_MASK, DB_COMMAND_READY_ALIAS as DB_COMMAND_READY};
}

// Doorbell / status bits (transcribed from aacreg.h / FreeBSD aac driver).
pub const AAC_SELF_TEST_FAILED: u32 = 0x00000001;
pub const AAC_UP_AND_RUNNING: u32 = 0x00000002;

pub const DB_SYNC_COMMAND: u32 = 0x00000001;
pub const DB_RESPONSE_READY: u32 = 0x00000002;
pub const DB_PRINTF: u32 = 0x00000004;
pub const DB_COMMAND_READY: u32 = 0x00000008;
