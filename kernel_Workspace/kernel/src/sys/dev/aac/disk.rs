// disk.rs

use crate::sys::dev::aac::pci::AacSoftc;

pub const AAC_BLOCK_SIZE: u32 = 512;

// Minimal block device traits so this module compiles standalone.
// The kernel's real block layer can re-implement these and glue them in later.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IoError {
    NotSupported,
    Busy,
    Io,
}

pub struct IoRequest {
    read: bool,
    write: bool,
    byte_count_val: usize,
    residual_val: usize,
}

impl IoRequest {
    pub fn new(read: bool, write: bool, byte_count: usize) -> Self {
        Self { read, write, byte_count_val: byte_count, residual_val: byte_count }
    }
    pub fn is_read(&self) -> bool { self.read }
    pub fn is_write(&self) -> bool { self.write }
    pub fn byte_count(&self) -> usize { self.byte_count_val }
    pub fn set_residual(&mut self, residual: usize) { self.residual_val = residual; }
}

pub trait BlockDevice {
    fn sector_size(&self) -> u32;
    fn total_sectors(&self) -> u64;
    fn strategy(&self, req: &mut IoRequest) -> Result<(), IoError>;
}

pub struct AacDisk {
    pub controller: *mut AacSoftc,
    pub container_id: u32,
    pub capacity_sectors: u64,
    pub unit: u32,
}

impl AacDisk {
    pub fn new(controller: *mut AacSoftc, container_id: u32, capacity: u64, unit: u32) -> Self {
        Self {
            controller,
            container_id,
            capacity_sectors: capacity,
            unit,
        }
    }

    pub fn geometry(&self) -> (u32, u32, u32) {
        let size = self.capacity_sectors;
        let (heads, sectors) = if size >= 2 * 1024 * 1024 {
            (255, 63)
        } else if size >= 1 * 1024 * 1024 {
            (128, 32)
        } else {
            (64, 32)
        };
        let cylinders = (size / (heads as u64 * sectors as u64)) as u32;
        (cylinders, heads, sectors)
    }
}

impl BlockDevice for AacDisk {
    fn sector_size(&self) -> u32 {
        AAC_BLOCK_SIZE
    }

    fn total_sectors(&self) -> u64 {
        self.capacity_sectors
    }

    fn strategy(&self, req: &mut IoRequest) -> Result<(), IoError> {
        if req.byte_count() == 0 {
            req.set_residual(0);
            return Ok(());
        }

        if !req.is_read() && !req.is_write() {
            return Err(IoError::NotSupported);
        }

        // W rzeczywistości tutaj musisz zbudować FIB (ContainerCommand),
        // podpiąć Scatter-Gather listę do bufora z req i wrzucić FIB do kolejki.
        // Na ten moment to tylko placeholder pokazujący strukturę.

        unsafe {
            let sc = &mut *self.controller;
            // TODO: sc.submit_io(self.container_id, req);
        }

        Ok(())
    }
}