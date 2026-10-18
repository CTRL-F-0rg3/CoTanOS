// src/drivers/aac/fib.rs

//! Struktury FIB (Firmware Interface Block).
//! To w nich wysyłamy komendy do firmware'u Adaptec.

use core::fmt;


/// Maksymalny rozmiar danych wewnątrz FIB (512 - nagłówek).
pub const FIB_DATASIZE: usize = 512 - core::mem::size_of::<FibHeader>();

/// Pełny blok FIB.
#[repr(C)]
#[derive(Clone, Copy)]
pub struct Fib {
    pub header: FibHeader,
    pub data: [u8; FIB_DATASIZE],
}

// Implementacja Debug dla Fib, żeby nie spamować 512 bajtów w logach
impl fmt::Debug for Fib {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Fib")
            .field("header", &self.header)
            .field("data", &"[...]")
            .finish()
    }
}

/// Komendy FIB (to, co wpisujemy w `header.command`)
#[repr(u16)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FibCommand {
    // Komendy testowe i komunikacyjne
    TestCommandResponse = 1,
    TestAdapterCommand = 2,
    
    // Komendy kontenerów (Dyski/RAID) - NAJWAŻNIEJSZE DLA NAS
    ContainerCommand = 500,
    ContainerCommand64 = 501,
    RawIo = 502,

    // SCSI Passthrough (do bezpośredniego gadania z dyskami)
    ScsiPortCommand = 600,
    ScsiPortCommandU64 = 601,

    // Zarządzanie kartą
    RequestAdapterInfo = 703,
    SendHostTime = 705,
}

/// Stany transferu FIB (XferState)
#[repr(C, packed)]
#[derive(Clone, Copy, Debug)]
pub struct FibHeader {
    pub xfer_state: u32,
    pub command: u16,
    pub subcommand: u16,
    pub flags: u16,
    pub size: u16,
    pub sender_fib_address: u32,
    pub receiver_fib_address: u32,
    pub context: u32,
    pub spare: u32,
}

pub mod fib_state {
    pub const HOST_OWNED: u32 = 0x0001;
    pub const ADAPTER_OWNED: u32 = 0x0002;
    pub const INITIALISED: u32 = 0x0004;
    pub const EMPTY: u32 = 0x0008;
    pub const FROM_POOL: u32 = 0x0010;
    pub const FROM_HOST: u32 = 0x0020;
    pub const FROM_ADAP: u32 = 0x0040;
    pub const R_EXPECTED: u32 = 0x0080;
    pub const DONE_ADAP: u32 = 0x0200;
    pub const DONE_HOST: u32 = 0x0400;
    pub const BAD: u32 = 0x0800;
    pub const CMD_BAD: u32 = 0x4000;
}
// ==========================================
// Struktury payloadów (wewnątrz `fib.data`)
// ==========================================

/// Wpis w tablicy Scatter-Gather (32-bit)
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct SgEntry {
    pub address: u32,
    pub byte_count: u32,
}

/// Tablica Scatter-Gather (32-bit)
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct SgTable {
    pub count: u32,
    // W C to `SgEntry[0]`, w Rustcie musimy to obsłużyć ostrożnie.
    // Najlepiej alokować odpowiednio duży bufor i rzutować, albo użyć tablicy o max rozmiarze.
    // Na start zrobimy to jako wskaźnik lub po prostu zarezerwujemy miejsce.
}

/// Komenda odczytu bloku (ContainerCommand)
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct BlockRead {
    pub command: u32,      // Zawsze VM_CtBlockRead (ale tu mamy FibCommand)
    pub container_id: u32, // ID woluminu RAID
    pub block_number: u32, // Numer sektora (LBA)
    pub byte_count: u32,   // Ile bajtów czytać
    // Tu wklejamy SgTable
}

/// Komenda zapisu bloku (ContainerCommand)
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct BlockWrite {
    pub command: u32,
    pub container_id: u32,
    pub block_number: u32,
    pub byte_count: u32,
    pub stable: u32,       // Czy zapis ma być "stabilny" (sync)
    // Tu wklejamy SgTable
}

/// Struktura odpowiedzi na zapytanie o info o karcie (RequestAdapterInfo)
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct AdapterInfo {
    pub platform_base: u32,
    pub cpu_arch: u32,
    pub cpu_variant: u32,
    pub clock_speed: u32,
    pub execution_mem: u32,
    pub buffer_mem: u32,
    pub total_mem: u32,
    // ... pomijam resztę na ten moment, bo to dużo pól, dodamy w razie potrzeby
}