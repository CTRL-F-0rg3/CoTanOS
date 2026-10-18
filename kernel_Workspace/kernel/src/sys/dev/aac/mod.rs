//! Adaptec AAC RAID host driver.
//! v1: polled, synchronous, 32-bit DMA, bounce-buffered reads.

pub mod regs;
pub mod fib;
pub mod tables;
pub mod pci;
pub mod queues;
pub mod disk;

use core::{fmt, mem, ptr};
use core::sync::atomic::{fence, Ordering};

use fib::{Fib, FibHeader, FibCommand, BlockRead, SgEntry, AdapterInfo, fib_state};
use queues::{QueueTable, QueueEntry, QUEUE_LAYOUT, HOST_NORM_CMD, ADAP_NORM_RESP};
use regs::rx;
use tables::AAC_FLAGS_LBA_64BIT;

pub type Result<T> = core::result::Result<T, AacError>;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AacError {
    Dma,
    Timeout,
    NoFib,
    BadFib(u32),
    Status(u32),
    Range,
    NotReady,
}

// ---------------------------------------------------------------------------
// Kernel boundary. Implement once against your existing mm / console.
// ---------------------------------------------------------------------------

/// Zeroed, physically contiguous, DMA-reachable memory.
/// v1 requires phys < 4 GiB.
#[derive(Debug, Clone, Copy)]
pub struct DmaMem {
    pub virt: usize,
    pub phys: u64,
    pub len: usize,
}

pub trait KernelIo {
    fn dma_alloc(&mut self, len: usize, align: usize) -> Option<DmaMem>;
    fn udelay(&mut self, us: u32);
    fn log(&mut self, args: fmt::Arguments);
}

macro_rules! log {
    ($io:expr, $($arg:tt)*) => { $io.log(core::format_args!($($arg)*)) };
}

#[derive(Clone, Copy)]
pub struct RegWindow(usize);

impl RegWindow {
    #[inline]
    pub fn read(self, off: u32) -> u32 {
        unsafe { ptr::read_volatile((self.0 + off as usize) as *const u32) }
    }

    #[inline]
    pub fn write(self, off: u32, val: u32) {
        unsafe { ptr::write_volatile((self.0 + off as usize) as *mut u32, val) }
    }
}

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const FIB_COUNT: usize = 64;
const FIB_BYTES: usize = 512;
const HEADER_BYTES: usize = mem::size_of::<FibHeader>();
const BOUNCE_BYTES: usize = 64 * 1024;
const SECTOR: usize = 512;

const POLL_ITERS: u32 = 500_000;
const POLL_US: u32 = 10;

const SUBMIT: usize = HOST_NORM_CMD;
const REAP: usize = ADAP_NORM_RESP;

const VM_CT_BLOCK_READ: u32 = 8;

const _: () = assert!(mem::size_of::<Fib>() == FIB_BYTES);
const _: () = assert!(HEADER_BYTES <= 64);

// ---------------------------------------------------------------------------
// FIB pool
// ---------------------------------------------------------------------------

#[derive(Clone, Copy, PartialEq, Eq)]
enum Slot {
    Free,
    Sync,
    Async,
}

#[derive(Clone, Copy)]
struct PoolSlot {
    state: Slot,
    tag: u32,
}

impl PoolSlot {
    const FREE: Self = Self { state: Slot::Free, tag: 0 };
}

struct FibPool {
    mem: DmaMem,
    slots: [PoolSlot; FIB_COUNT],
}

impl FibPool {
    fn new(mem: DmaMem) -> Self {
        Self { mem, slots: [PoolSlot::FREE; FIB_COUNT] }
    }

    #[inline]
    fn ptr(&self, i: usize) -> *mut Fib {
        debug_assert!(i < FIB_COUNT);
        (self.mem.virt + i * FIB_BYTES) as *mut Fib
    }

    #[inline]
    fn phys(&self, i: usize) -> u32 {
        (self.mem.phys + (i * FIB_BYTES) as u64) as u32
    }

    fn alloc(&mut self, state: Slot, tag: u32) -> Option<usize> {
        for i in 0..FIB_COUNT {
            if self.slots[i].state == Slot::Free {
                self.slots[i] = PoolSlot { state, tag };
                return Some(i);
            }
        }
        None
    }

    fn free(&mut self, i: usize) {
        if i < FIB_COUNT {
            self.slots[i] = PoolSlot::FREE;
        }
    }

    fn index_of_phys(&self, addr: u32) -> Option<usize> {
        let base = self.mem.phys as u32;
        if addr < base {
            return None;
        }
        let delta = addr - base;
        if delta % FIB_BYTES as u32 != 0 {
            return None;
        }
        let idx = (delta / FIB_BYTES as u32) as usize;
        if idx < FIB_COUNT { Some(idx) } else { None }
    }
}

// ---------------------------------------------------------------------------
// Controller
// ---------------------------------------------------------------------------

pub struct AacController {
    pub regs: RegWindow,
    pub quirks: u32,
    pub ready: bool,

    qtab: DmaMem,
    pool: FibPool,
    bounce: DmaMem,

    reap_cons: u32,
    completed: [bool; FIB_COUNT],
    statuses: [u32; FIB_COUNT],

    pub next_tag: u32,
}

// Access is serialized by the kernel driver lock.
unsafe impl Send for AacController {}

impl AacController {
    pub fn attach(io: &mut dyn KernelIo, regs: RegWindow, quirks: u32) -> Result<Self> {
        let qtab = io.dma_alloc(mem::size_of::<QueueTable>(), 64).ok_or(AacError::Dma)?;
        let pool = io.dma_alloc(FIB_COUNT * FIB_BYTES, 64).ok_or(AacError::Dma)?;
        let bounce = io.dma_alloc(BOUNCE_BYTES, 4096).ok_or(AacError::Dma)?;

        // v1 uses 32-bit FIB addresses everywhere.
        if qtab.phys >= (1u64 << 32)
            || pool.phys >= (1u64 << 32)
            || bounce.phys >= (1u64 << 32)
        {
            return Err(AacError::Dma);
        }

        unsafe {
            ptr::write_bytes(qtab.virt as *mut u8, 0, qtab.len);
            ptr::write_bytes(pool.virt as *mut u8, 0, pool.len);
            ptr::write_bytes(bounce.virt as *mut u8, 0, bounce.len);
        }

        let mut sc = Self {
            regs,
            quirks,
            ready: false,
            qtab,
            pool: FibPool::new(pool),
            bounce,
            reap_cons: 0,
            completed: [false; FIB_COUNT],
            statuses: [0; FIB_COUNT],
            next_tag: 1,
        };

        sc.handshake(io)?;
        sc.ready = true;
        Ok(sc)
    }

    #[inline]
    fn qt(&mut self) -> &mut QueueTable {
        unsafe { &mut *(self.qtab.virt as *mut QueueTable) }
    }

    #[inline]
    fn queue_ptr(&self, q: usize) -> *mut QueueEntry {
        let (offset, _) = QUEUE_LAYOUT[q];
        unsafe { (self.qtab.virt + offset) as *mut QueueEntry }
    }

    // -----------------------------------------------------------------------
    // Hardware handshake. Isolated: replace once aac.c is ported.
    // -----------------------------------------------------------------------

    fn wait_bit(&mut self, io: &mut dyn KernelIo, reg: u32, mask: u32) -> Result<()> {
        for _ in 0..POLL_ITERS {
            if self.regs.read(reg) & mask != 0 {
                return Ok(());
            }
            io.udelay(POLL_US);
        }
        Err(AacError::Timeout)
    }

    fn handshake(&mut self, io: &mut dyn KernelIo) -> Result<()> {
        self.wait_bit(io, rx::FW_STATUS, rx::STS_UP_AND_RUNNING)?;

        self.regs.write(rx::BASE_PTR_LO, self.qtab.phys as u32);
        self.regs.write(rx::BASE_PTR_HI, (self.qtab.phys >> 32) as u32);

        self.regs.write(rx::INBOUND_DOORBELL, rx::DB_INIT);
        self.wait_bit(io, rx::OUTBOUND_DOORBELL, rx::DB_INIT_ACK)?;

        self.regs.write(rx::OUT_INTR_MASK, 0);
        Ok(())
    }

    // -----------------------------------------------------------------------
    // FIB construction
    // -----------------------------------------------------------------------

    fn take_sync(&mut self) -> Result<usize> {
        let tag = self.next_tag;
        self.next_tag = self.next_tag.wrapping_add(1).max(1);

        let idx = self.pool.alloc(Slot::Sync, tag).ok_or(AacError::NoFib)?;
        self.completed[idx] = false;
        self.statuses[idx] = 0;

        unsafe {
            ptr::write_bytes(self.pool.ptr(idx) as *mut u8, 0, FIB_BYTES);
        }
        Ok(idx)
    }

    unsafe fn header(&mut self, idx: usize, command: u16, sub: u16, size: u16) {
        let h = &mut (*self.pool.ptr(idx)).header;
        h.xfer_state = fib_state::HOST_OWNED
            | fib_state::INITIALISED
            | fib_state::FROM_POOL
            | fib_state::FROM_HOST
            | fib_state::R_EXPECTED;
        h.command = command;
        h.subcommand = sub;
        h.flags = 0;
        h.size = size;
        h.context = self.pool.slots[idx].tag;
    }

    // -----------------------------------------------------------------------
    // Queue producer / consumer
    // -----------------------------------------------------------------------

    fn push(&mut self, idx: usize) {
        let (_, count) = QUEUE_LAYOUT[SUBMIT];
        let prod = (self.qt().q_index[SUBMIT][queues::PRODUCER] as usize) % count;

        unsafe {
            *self.queue_ptr(SUBMIT).add(prod) = QueueEntry {
                size: FIB_BYTES as u32,
                addr: self.pool.phys(idx),
            };
        }

        self.qt().q_index[SUBMIT][queues::PRODUCER] = ((prod + 1) % count) as u32;
        fence(Ordering::SeqCst);
        self.regs.write(rx::INBOUND_DOORBELL, rx::DB_COMMAND_READY);
    }

    fn reap_one(&mut self) -> Option<usize> {
        let (_, count) = QUEUE_LAYOUT[REAP];
        let prod = self.qt().q_index[REAP][queues::PRODUCER] as usize;
        let cons = self.reap_cons as usize;

        if cons == prod {
            return None;
        }

        let entry = unsafe { *self.queue_ptr(REAP).add(cons) };
        let idx = self.pool.index_of_phys(entry.addr)?;

        self.reap_cons = ((cons + 1) % count) as u32;
        self.qt().q_index[REAP][queues::CONSUMER] = self.reap_cons;
        fence(Ordering::SeqCst);

        Some(idx)
    }

    fn complete(&mut self, idx: usize) {
        if self.pool.slots[idx].state == Slot::Free {
            return;
        }

        let state = unsafe { (*self.pool.ptr(idx)).header.xfer_state };
        self.statuses[idx] = state;
        self.completed[idx] = true;

        if self.pool.slots[idx].state == Slot::Async {
            // Async completion ring goes here in v2.
            self.pool.free(idx);
        }
    }

    // -----------------------------------------------------------------------
    // Interrupt / poll path
    // -----------------------------------------------------------------------

    pub fn on_interrupt(&mut self, io: &mut dyn KernelIo) {
        let db = self.regs.read(rx::OUTBOUND_DOORBELL);
        if db == 0 {
            return;
        }

        if db & rx::DB_RESPONSE_READY != 0 {
            while let Some(idx) = self.reap_one() {
                self.complete(idx);
            }
            self.regs.write(rx::OUTBOUND_DOORBELL, rx::DB_RESPONSE_READY);
        }

        if db & rx::DB_PRINTF != 0 {
            self.drain_firmware_printf(io);
            self.regs.write(rx::OUTBOUND_DOORBELL, rx::DB_PRINTF);
        }
    }

    fn drain_firmware_printf(&mut self, io: &mut dyn KernelIo) {
        // Firmware printf buffer lives in the init block. Wire after aac.c port.
        log!(io, "[aac] firmware printf request\n");
    }

    fn run_sync(&mut self, io: &mut dyn KernelIo, idx: usize) -> Result<()> {
        self.push(idx);

        for _ in 0..POLL_ITERS {
            self.on_interrupt(io);

            if self.completed[idx] {
                let state = self.statuses[idx];
                self.pool.free(idx);

                if state & (fib_state::BAD | fib_state::CMD_BAD) != 0 {
                    return Err(AacError::BadFib(state));
                }
                return Ok(());
            }

            io.udelay(POLL_US);
        }

        self.pool.free(idx);
        Err(AacError::Timeout)
    }

    // -----------------------------------------------------------------------
    // Commands
    // -----------------------------------------------------------------------

    pub fn adapter_info(&mut self, io: &mut dyn KernelIo) -> Result<AdapterInfo> {
        let idx = self.take_sync()?;

        unsafe {
            self.header(idx, FibCommand::RequestAdapterInfo as u16, 0, 0);
        }

        self.run_sync(io, idx)?;

        let info = unsafe {
            let base = (self.pool.ptr(idx) as *mut u8).wrapping_add(HEADER_BYTES);
            ptr::read_unaligned(base.cast::<AdapterInfo>())
        };

        log!(
            io,
            "[aac] firmware arch={} clock={}kHz mem={}KiB\n",
            info.cpu_arch, info.clock_speed, info.total_mem
        );
        Ok(info)
    }

    /// Synchronous read into a caller buffer. Bounce-buffered, <= 64 KiB.
    pub fn read_sectors(
        &mut self,
        io: &mut dyn KernelIo,
        container: u32,
        lba: u64,
        dst: &mut [u8],
    ) -> Result<()> {
        if !self.ready {
            return Err(AacError::NotReady);
        }

        let bytes = dst.len();
        if bytes == 0 || bytes % SECTOR != 0 || bytes > BOUNCE_BYTES {
            return Err(AacError::Range);
        }

        let sectors = (bytes / SECTOR) as u32;

        if (lba + sectors as u64) > u32::MAX as u64 && self.quirks & AAC_FLAGS_LBA_64BIT == 0 {
            return Err(AacError::Range);
        }

        let idx = self.take_sync()?;

        unsafe {
            let payload = (self.pool.ptr(idx) as *mut u8)
                .wrapping_add(HEADER_BYTES) as *mut BlockRead;

            ptr::write(payload, BlockRead {
                command: VM_CT_BLOCK_READ,
                container_id: container,
                block_number: lba as u32,
                sector_count: sectors,
                stamp: 0,
            });

            let sg = payload.cast::<u8>()
                .add(mem::size_of::<BlockRead>())
                .cast::<queues::SgTable>();

            ptr::write(sg, queues::SgTable {
                count: 1,
                entry: queues::SgEntry {
                    address: self.bounce.phys as u32,
                    byte_count: bytes as u32,
                },
            });

            let size = (mem::size_of::<BlockRead>() + mem::size_of::<queues::SgTable>()) as u16;
            self.header(idx, FibCommand::ContainerCommand as u16, 0, size);
        }

        if let Err(e) = self.run_sync(io, idx) {
            return Err(e);
        }

        // TODO: decode container status from the response payload (aac.c).
        unsafe {
            ptr::copy_nonoverlapping(self.bounce.virt as *const u8, dst.as_mut_ptr(), bytes);
        }
        Ok(())
    }

    // -----------------------------------------------------------------------
    // Geometry, transcribed from aac_disk.c
    // -----------------------------------------------------------------------

    pub fn geometry(sectors: u64) -> (u32, u32, u32) {
        let (heads, secs) = if sectors >= 2 * 1024 * 1024 {
            (255, 63)
        } else if sectors >= 1024 * 1024 {
            (128, 32)
        } else {
            (64, 32)
        };

        let cyl = sectors / (heads as u64 * secs as u64);
        (cyl as u32, heads, secs)
    }

    pub fn container_capacity(capacity: u32, capacity_high: u32) -> u64 {
        capacity as u64 | ((capacity_high as u64) << 32)
    }
}