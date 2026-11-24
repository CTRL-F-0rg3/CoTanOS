use crate::hal::CotanHal;
use cotan_kapi::{KResult, Error, device::BlockDevice, kprintln};
use virtio_drivers::transport::mmio::{MmioTransport, VirtIOHeader};
use virtio_drivers::device::blk::VirtIOBlk;
use core::ptr::NonNull;
use spin::Mutex;

pub struct VirtioBlkDriver {
    inner: Option<VirtIOBlk<CotanHal, MmioTransport<'static>>>,
    ready: bool,
}

impl VirtioBlkDriver {
    pub const fn new() -> Self { Self { inner: None, ready: false } }

    pub fn init(&mut self, mmio_base: usize) -> KResult<()> {
        if mmio_base == 0 { return Err(Error::NotImplemented); }
        kprintln!("[rust/cotan_storage::virtio] storage: probing virtio-blk @ {:#x}", mmio_base);

        let header_ptr = mmio_base as *mut VirtIOHeader;
        let header = unsafe { NonNull::new_unchecked(header_ptr) };

        let transport = unsafe {
            MmioTransport::new(header, 0x200).map_err(|e| {
                kprintln!("[rust/cotan_storage::virtio] storage: MmioTransport failed @ {:#x}: {:?}", mmio_base, e);
                Error::Invalid
            })?
        };

        let blk = VirtIOBlk::<CotanHal, MmioTransport<'static>>::new(transport).map_err(|e| {
            kprintln!("[rust/cotan_storage::virtio] storage: VirtIOBlk::new failed @ {:#x}: {:?}", mmio_base, e);
            Error::Invalid
        })?;

        let capacity = blk.capacity();
        kprintln!("[rust/cotan_storage::virtio] storage: raw capacity from device: {} blocks", capacity);
        
        if capacity > 2_000_000_000 {
            kprintln!("[rust/cotan_storage::virtio] storage: ERROR: suspicious capacity {} blocks", capacity);
            return Err(Error::Io);
        }
        
        kprintln!("[rust/cotan_storage::virtio] storage: SUCCESS! disk capacity: {} blocks ({} MB)", 
                   capacity, capacity * 512 / 1024 / 1024);

        self.inner = Some(blk);
        self.ready = true;
        Ok(())
    }
}

impl BlockDevice for VirtioBlkDriver {
    fn block_size(&self) -> usize { 512 }
    fn block_count(&self) -> u64 { 
        self.inner.as_ref().map(|b| b.capacity() as u64).unwrap_or(0) 
    }
    
    fn read_blocks(&mut self, lba: u64, out: &mut [u8]) -> KResult<usize> {
        let blk = self.inner.as_mut().ok_or(Error::Invalid)?;
        blk.read_blocks(lba as usize, out).map_err(|_| Error::Io)?;
        Ok(out.len() / 512)
    }
    
    fn write_blocks(&mut self, lba: u64, data: &[u8]) -> KResult<usize> {
        let blk = self.inner.as_mut().ok_or(Error::Invalid)?;
        blk.write_blocks(lba as usize, data).map_err(|_| Error::Io)?;
        Ok(data.len() / 512)
    }
}

pub static VIRTIO_BLK: Mutex<VirtioBlkDriver> = Mutex::new(VirtioBlkDriver::new());
