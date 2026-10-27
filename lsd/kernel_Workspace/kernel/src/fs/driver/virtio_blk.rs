//! `virtio_blk` — the virtio block device, ported from the virtio 0.9 (legacy
//! PCI) transport already used by `nic/virtio/pci_legacy.rs`.
//!
//! # Why this driver exists
//!
//! The kernel's only other disk driver is `ata_pio`, which talks to IDE in PIO
//! mode and tops out at LBA28. QEMU's default disk is a virtio-blk device, so
//! without this driver a QEMU guest boots with no writable disk behind the
//! "virtio" PCI device it can already see. This is the block counterpart of the
//! virtio-net driver: same vendor, same legacy register layout, one queue
//! instead of two.
//!
//! # The request shape
//!
//! A virtio-blk request is a three-descriptor chain:
//!
//! ```text
//!   desc 0  header (16 bytes, device reads): kind | reserved | sector(LE)
//!   desc 1  data   (512 bytes): device writes on read, reads on write
//!   desc 2  status (1 byte, device writes): 0 = OK, non-zero = error
//! ```
//!
//! The sector field is always in 512-byte units, matching [`BlockDevice`]'s
//! single-block contract, so one `read_block` is exactly one request.

use core::ffi::c_void;
use core::ptr::{read_volatile, write_bytes, write_volatile};
use core::sync::atomic::{compiler_fence, AtomicBool, AtomicU16, AtomicU64, Ordering};

use x86_64::instructions::port::Port;

use crate::mm::ffi::contig_alloc;
use crate::nic::virtio::queue::{Descriptor, VIRTQ_DESC_F_NEXT, VIRTQ_DESC_F_WRITE};
use crate::pci::{self, VIRTIO_BLK_LEGACY_DEVICE, VIRTIO_BLK_MODERN_DEVICE, VIRTIO_VENDOR};

use super::block::{BlockDevice, DriverError};
use super::lock::IrqGuard;

const STATUS_ACKNOWLEDGE: u8 = 1;
const STATUS_DRIVER: u8 = 2;
const STATUS_DRIVER_OK: u8 = 4;
const STATUS_FAILED: u8 = 128;

const REQUEST_QUEUE_INDEX: u16 = 0;
const QUEUE_SIZE: u16 = 4;
const PAGE_SIZE: usize = 4096;

const REG_GUEST_FEATURES: u16 = 4;
const REG_QUEUE_ADDRESS: u16 = 8;
const REG_QUEUE_SIZE: u16 = 12;
const REG_QUEUE_SELECT: u16 = 14;
const REG_QUEUE_NOTIFY: u16 = 16;
const REG_DEVICE_STATUS: u16 = 18;
const REG_DEVICE_CONFIG: u16 = 20;

const BLK_CONFIG_CAPACITY: u16 = 0;
const BLK_CONFIG_BLK_SIZE: u16 = 20;

const REQ_READ: u32 = 0;
const REQ_WRITE: u32 = 1;
const SECTOR_BYTES: usize = 512;
const HEADER_BYTES: usize = 16;
const STATUS_BYTES: usize = 1;
const REQ_BYTES: usize = HEADER_BYTES + SECTOR_BYTES + STATUS_BYTES;

#[repr(C)]
#[derive(Clone, Copy)]
struct BlkRequestHeader {
    kind: u32,
    reserved: u32,
    sector: u64,
}

#[repr(C)]
#[derive(Clone, Copy)]
struct UsedElement {
    id: u32,
    len: u32,
}

fn align_up(value: usize, to: usize) -> usize {
    (value + to - 1) & !(to - 1)
}

fn queue_bytes(size: u16) -> usize {
    let size = size as usize;
    let descriptors = core::mem::size_of::<Descriptor>() * size;
    let avail = 4 + size * 2;
    let used = align_up(4 + size * core::mem::size_of::<UsedElement>(), 2);
    align_up(descriptors + avail, PAGE_SIZE) + align_up(used, PAGE_SIZE)
}


/// One virtqueue, laid out as `descriptors | avail ring | used ring`.
///
/// Same layout as `nic/virtio/pci_legacy.rs`'s `LegacyQueue`, but descriptors
/// support `next` chaining — a blk request is three descriptors, a net frame is
/// one.
#[derive(Clone, Copy)]
struct BlkQueue {
    virt: *mut u8,
    phys: u64,
    size: u16,
    last_used: u16,
}

impl BlkQueue {
    fn allocate(size: u16) -> Result<Self, DriverError> {
        let bytes = queue_bytes(size);
        let mut phys = 0u64;
        let mut virt = core::ptr::null_mut::<c_void>();
        let ok = unsafe {
            contig_alloc(
                bytes,
                PAGE_SIZE,
                &mut phys as *mut u64,
                &mut virt as *mut *mut c_void,
            )
        };
        if !ok || virt.is_null() || phys == 0 || phys as usize % PAGE_SIZE != 0 {
            return Err(DriverError::Io);
        }
        unsafe { write_bytes(virt as *mut u8, 0, bytes) };
        Ok(Self { virt: virt as *mut u8, phys, size, last_used: 0 })
    }

    fn descriptor_offset(&self, index: u16) -> usize {
        index as usize * core::mem::size_of::<Descriptor>()
    }

    fn avail_offset(&self) -> usize {
        self.size as usize * core::mem::size_of::<Descriptor>()
    }

    fn used_offset(&self) -> usize {
        let descriptor_bytes = self.size as usize * core::mem::size_of::<Descriptor>();
        let avail_bytes = 4 + self.size as usize * 2;
        align_up(descriptor_bytes + avail_bytes, PAGE_SIZE)
    }

    unsafe fn write_descriptor(&mut self, index: u16, addr: u64, len: u32, flags: u16, next: u16) {
        let descriptor = self.virt.add(self.descriptor_offset(index)) as *mut Descriptor;
        write_volatile(descriptor, Descriptor { addr, len, flags, next });
    }

    unsafe fn submit(&mut self, descriptor: u16) {
        let avail = self.virt.add(self.avail_offset());
        let idx_ptr = avail.add(2) as *mut u16;
        let idx = read_volatile(idx_ptr);
        let ring = avail.add(4) as *mut u16;
        write_volatile(ring.add(idx as usize % self.size as usize), descriptor);
        compiler_fence(Ordering::Release);
        write_volatile(idx_ptr, idx.wrapping_add(1));
    }

    unsafe fn used_index(&self) -> u16 {
        compiler_fence(Ordering::Acquire);
        read_volatile(self.virt.add(self.used_offset() + 2) as *const u16)
    }

    unsafe fn take_used(&mut self) -> Option<(u16, u32)> {
        let used_index = self.used_index();
        if used_index == self.last_used {
            return None;
        }
        let ring = self.virt.add(self.used_offset() + 4) as *const UsedElement;
        let element = read_volatile(ring.add(self.last_used as usize % self.size as usize));
        self.last_used = self.last_used.wrapping_add(1);
        if element.id >= self.size as u32 {
            return None;
        }
        Some((element.id as u16, element.len))
    }
}

/// The single virtio-blk device, probed at boot like `ata_pio::ATA0`.
///
/// Every field is an atomic so `BlockDevice`'s `&self` methods can drive it.
/// The DMA buffers and queue rings are touched only under [`IrqGuard`] — the
/// same serialization `ata_pio` relies on for its port I/O.
pub struct VirtioBlk {
    io_base: AtomicU16,
    ready: AtomicBool,
    block_size: AtomicU64,
    sectors: AtomicU64,
    queue_virt: AtomicU64,
    queue_phys: AtomicU64,
    queue_size: AtomicU16,
    last_used: AtomicU16,
    req_virt: AtomicU64,
    req_phys: AtomicU64,
}

pub static VBLK: VirtioBlk = VirtioBlk::uninit();

impl VirtioBlk {
    pub const fn uninit() -> Self {
        Self {
            io_base: AtomicU16::new(0),
            ready: AtomicBool::new(false),
            block_size: AtomicU64::new(0),
            sectors: AtomicU64::new(0),
            queue_virt: AtomicU64::new(0),
            queue_phys: AtomicU64::new(0),
            queue_size: AtomicU16::new(0),
            last_used: AtomicU16::new(0),
            req_virt: AtomicU64::new(0),
            req_phys: AtomicU64::new(0),
        }
    }

    pub fn is_present(&self) -> bool {
        self.ready.load(Ordering::Relaxed)
    }

    fn queue(&self) -> BlkQueue {
        BlkQueue {
            virt: self.queue_virt.load(Ordering::Relaxed) as *mut u8,
            phys: self.queue_phys.load(Ordering::Relaxed),
            size: self.queue_size.load(Ordering::Relaxed),
            last_used: self.last_used.load(Ordering::Relaxed),
        }
    }

    fn notify(&self, queue: u16) {
        write_u16(self.io_base.load(Ordering::Relaxed), REG_QUEUE_NOTIFY, queue);
    }

    fn initialize(&self, io_base: u16) -> bool {
        let _g = IrqGuard::lock();

        write_u8(io_base, REG_DEVICE_STATUS, 0);
        write_u8(io_base, REG_DEVICE_STATUS, STATUS_ACKNOWLEDGE);
        write_u8(io_base, REG_DEVICE_STATUS, STATUS_ACKNOWLEDGE | STATUS_DRIVER);

        // Basic read/write needs none of the optional feature bits.
        write_u32(io_base, REG_GUEST_FEATURES, 0);

        let cap_lo = read_u32(io_base, REG_DEVICE_CONFIG + BLK_CONFIG_CAPACITY) as u64;
        let cap_hi = read_u32(io_base, REG_DEVICE_CONFIG + BLK_CONFIG_CAPACITY + 4) as u64;
        let capacity = (cap_hi << 32) | cap_lo;
        let blk_size = read_u32(io_base, REG_DEVICE_CONFIG + BLK_CONFIG_BLK_SIZE);
        let blk_size = if blk_size == 0 { SECTOR_BYTES as u32 } else { blk_size };

        let queue = match BlkQueue::allocate(QUEUE_SIZE) {
            Ok(q) => q,
            Err(_) => {
                write_u8(io_base, REG_DEVICE_STATUS, STATUS_FAILED);
                return false;
            }
        };

        let mut req_phys = 0u64;
        let mut req_virt = core::ptr::null_mut::<c_void>();
        let ok = unsafe {
            contig_alloc(
                REQ_BYTES,
                16,
                &mut req_phys as *mut u64,
                &mut req_virt as *mut *mut c_void,
            )
        };
        if !ok || req_virt.is_null() || req_phys == 0 {
            write_u8(io_base, REG_DEVICE_STATUS, STATUS_FAILED);
            return false;
        }

        write_u16(io_base, REG_QUEUE_SELECT, REQUEST_QUEUE_INDEX);
        let max = read_u16(io_base, REG_QUEUE_SIZE);
        if queue.size == 0 || queue.size > max || !queue.size.is_power_of_two() {
            write_u8(io_base, REG_DEVICE_STATUS, STATUS_FAILED);
            return false;
        }
        write_u16(io_base, REG_QUEUE_SIZE, queue.size);
        write_u32(io_base, REG_QUEUE_ADDRESS, (queue.phys >> 12) as u32);

        self.io_base.store(io_base, Ordering::Relaxed);
        self.block_size.store(blk_size as u64, Ordering::Relaxed);
        self.sectors.store(capacity, Ordering::Relaxed);
        self.queue_virt.store(queue.virt as u64, Ordering::Relaxed);
        self.queue_phys.store(queue.phys, Ordering::Relaxed);
        self.queue_size.store(queue.size, Ordering::Relaxed);
        self.last_used.store(0, Ordering::Relaxed);
        self.req_virt.store(req_virt as u64, Ordering::Relaxed);
        self.req_phys.store(req_phys, Ordering::Relaxed);

        write_u8(
            io_base,
            REG_DEVICE_STATUS,
            STATUS_ACKNOWLEDGE | STATUS_DRIVER | STATUS_DRIVER_OK,
        );
        self.ready.store(true, Ordering::Relaxed);
        true
    }

    /// One request. `data` points at a 512-byte buffer; on a write it is read
    /// into the DMA buffer first, on a read it is filled from the DMA buffer
    /// after the device completes.
    fn do_io(&self, block: u64, kind: u32, data: *mut u8) -> Result<(), DriverError> {
        let _g = IrqGuard::lock();

        if !self.is_present() {
            return Err(DriverError::NoDevice);
        }

        let req_virt = self.req_virt.load(Ordering::Relaxed) as *mut u8;
        let req_phys = self.req_phys.load(Ordering::Relaxed);
        let mut queue = self.queue();

        // The sector field is in 512-byte units even when the logical block
        // size is larger.
        let sector = block * self.block_size.load(Ordering::Relaxed) / SECTOR_BYTES as u64;

        unsafe {
            write_volatile(
                req_virt as *mut BlkRequestHeader,
                BlkRequestHeader { kind, reserved: 0, sector },
            );
            if kind == REQ_WRITE {
                core::ptr::copy_nonoverlapping(data, req_virt.add(HEADER_BYTES), SECTOR_BYTES);
            }

            let data_flags = if kind == REQ_READ {
                VIRTQ_DESC_F_WRITE | VIRTQ_DESC_F_NEXT
            } else {
                VIRTQ_DESC_F_NEXT
            };

            queue.write_descriptor(0, req_phys, HEADER_BYTES as u32, VIRTQ_DESC_F_NEXT, 1);
            queue.write_descriptor(
                1,
                req_phys + HEADER_BYTES as u64,
                SECTOR_BYTES as u32,
                data_flags,
                2,
            );
            queue.write_descriptor(
                2,
                req_phys + (HEADER_BYTES + SECTOR_BYTES) as u64,
                STATUS_BYTES as u32,
                VIRTQ_DESC_F_WRITE,
                0,
            );
            queue.submit(0);
        }

        self.notify(REQUEST_QUEUE_INDEX);

        let mut completed = false;
        for _ in 0..1_000_000 {
            if unsafe { queue.take_used() }.is_some() {
                completed = true;
                break;
            }
        }
        self.last_used.store(queue.last_used, Ordering::Relaxed);

        if !completed {
            return Err(DriverError::Timeout);
        }

        let status = unsafe { read_volatile(req_virt.add(HEADER_BYTES + SECTOR_BYTES)) };
        if status != 0 {
            return Err(DriverError::Io);
        }

        if kind == REQ_READ {
            unsafe {
                core::ptr::copy_nonoverlapping(req_virt.add(HEADER_BYTES), data, SECTOR_BYTES);
            }
        }

        Ok(())
    }
}

impl BlockDevice for VirtioBlk {
    fn name(&self) -> &'static str {
        "vblk"
    }

    fn block_size(&self) -> usize {
        self.block_size.load(Ordering::Relaxed) as usize
    }

    fn block_count(&self) -> u64 {
        let bs = self.block_size();
        self.sectors.load(Ordering::Relaxed) * SECTOR_BYTES as u64 / bs as u64
    }

    fn read_block(&self, block: u64, buf: &mut [u8]) -> Result<(), DriverError> {
        if buf.len() < SECTOR_BYTES {
            return Err(DriverError::InvalidLength);
        }
        self.do_io(block, REQ_READ, buf.as_mut_ptr())
    }

    fn write_block(&self, block: u64, buf: &[u8]) -> Result<(), DriverError> {
        if buf.len() < SECTOR_BYTES {
            return Err(DriverError::InvalidLength);
        }
        self.do_io(block, REQ_WRITE, buf.as_ptr() as *mut u8)
    }
}

/// Find the first virtio-blk PCI device and bring it up.
pub fn probe() -> bool {
    let device = {
        let devices = pci::PCI_DEVICES.lock();
        devices.iter().copied().find(|d| {
            d.vendor_id == VIRTIO_VENDOR
                && (d.device_id == VIRTIO_BLK_LEGACY_DEVICE
                    || d.device_id == VIRTIO_BLK_MODERN_DEVICE)
        })
    };

    let Some(dev) = device else { return false };

    let Some(io_base) = pci::io_base_from_bar(pci::bar(dev.address, 0)) else {
        return false;
    };

    pci::enable_bus_mastering(dev.address);
    VBLK.initialize(io_base)
}

fn read_u16(base: u16, offset: u16) -> u16 {
    let mut port = Port::<u16>::new(base + offset);
    unsafe { port.read() }
}

fn read_u32(base: u16, offset: u16) -> u32 {
    let mut port = Port::<u32>::new(base + offset);
    unsafe { port.read() }
}

fn write_u8(base: u16, offset: u16, value: u8) {
    let mut port = Port::<u8>::new(base + offset);
    unsafe { port.write(value) }
}

fn write_u16(base: u16, offset: u16, value: u16) {
    let mut port = Port::<u16>::new(base + offset);
    unsafe { port.write(value) }
}

fn write_u32(base: u16, offset: u16, value: u32) {
    let mut port = Port::<u32>::new(base + offset);
    unsafe { port.write(value) }
}
