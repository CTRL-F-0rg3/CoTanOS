
pub const AAC_BLOCK_SIZE: u32 = 512;

pub const AAC_MAX_CONTAINERS: usize = 64;

pub mod rx {
    pub const OMR0: u32 = 0x18; // Outbound Message Register 0
    pub const OMR1: u32 = 0x1c; // Outbound Message Register 1
    pub const IDBR: u32 = 0x20; // Inbound Doorbell Register (Host -> Adapter)
    pub const IISR: u32 = 0x24; // Inbound Interrupt Status
    pub const IIMR: u32 = 0x28; // Inbound Interrupt Mask
    pub const ODBR: u32 = 0x2c; // Outbound Doorbell Register (Adapter -> Host)
    pub const OISR: u32 = 0x30; // Outbound Interrupt Status
    pub const OIMR: u32 = 0x34; // Outbound Interrupt Mask
    pub const IQUE: u32 = 0x40; // Inbound Queue (tu "wrzucamy" FIB-y)
    pub const OQUE: u32 = 0x44; // Outbound Queue (tu "wyjmujemy" odpowiedzi)
    pub const MAILBOX: u32 = 0x50; // Mailbox (20 bytes)
    pub const FWSTATUS: u32 = 0x6c; // Firmware Status

    // Queue table base pointer (64-bit address, written during adapter init).
    // TODO: verify offsets against the Adaptec AAC register spec (aacraid aacreg.h).
    pub const BASE_PTR_LO: u32 = 0x60;
    pub const BASE_PTR_HI: u32 = 0x64;
}

pub const DB_SYNC_COMMAND: u32 = 1 << 0;
pub const DB_COMMAND_READY: u32 = 1 << 1;
pub const DB_RESPONSE_READY: u32 = 1 << 2;
pub const DB_COMMAND_NOT_FULL: u32 = 1 << 3;
pub const DB_RESPONSE_NOT_FULL: u32 = 1 << 4;
pub const DB_PRINTF: u32 = 1 << 5;

// Init handshake doorbell bits (used by the adapter init sequence).
// TODO: verify bit numbers against the Adaptec AAC register spec.
pub const DB_INIT: u32 = 1 << 6;     // inbound: host asks the adapter to init
pub const DB_INIT_ACK: u32 = 1 << 6; // outbound: adapter acknowledges init

// Firmware status bits (read from the FWSTATUS register).
pub const STS_UP_AND_RUNNING: u32 = 0x01; // firmware finished booting

pub const DB_INTERRUPTS: u32 = DB_COMMAND_READY | DB_RESPONSE_READY | DB_PRINTF;



pub const HOST_NORM_CMD_ENTRIES: usize = 8;
pub const HOST_HIGH_CMD_ENTRIES: usize = 4;
pub const ADAP_NORM_CMD_ENTRIES: usize = 512;
pub const ADAP_HIGH_CMD_ENTRIES: usize = 4;

pub const HOST_NORM_RESP_ENTRIES: usize = 512;
pub const HOST_HIGH_RESP_ENTRIES: usize = 4;
pub const ADAP_NORM_RESP_ENTRIES: usize = 8;
pub const ADAP_HIGH_RESP_ENTRIES: usize = 4;

pub const QUEUE_COUNT: usize = 8;
pub const QUEUE_ALIGN: usize = 16;