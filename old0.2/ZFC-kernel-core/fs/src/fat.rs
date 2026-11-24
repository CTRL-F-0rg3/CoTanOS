//! FAT32 file system using the `fatfs` crate
//! Wraps a BlockDevice (virtio-blk) to provide standard FAT operations

use crate::{FileSystem, FileHandle};
use cotan_kapi::{KResult, Error, device::BlockDevice};
use cotan_storage::VIRTIO_BLK;
use fatfs::{FileSystem as FatFs, StdIoWrapper, LossySlice};
use spin::Mutex;
use alloc::string::String;
use alloc::vec::Vec;
use alloc::sync::Arc;
use core::cell::RefCell;

/// Wrapper to adapt our BlockDevice to fatfs::Io trait
struct BlockDeviceIo {
    device: &'static Mutex<dyn BlockDevice>,
}

impl fatfs::IoBase for BlockDeviceIo {
    type Error = ();
}

impl fatfs::Read for BlockDeviceIo {
    fn read(&mut self, buf: &mut [u8]) -> Result<usize, Self::Error> {
        // fatfs reads sequentially, we track position via block boundaries
        // For simplicity, read one block at a time
        let mut tmp = [0u8; 512];
        let lba = 0; // TODO: track position
        let mut dev = self.device.lock();
        if dev.read_blocks(lba, &mut tmp).is_err() {
            return Err(());
        }
        let len = buf.len().min(512);
        buf[..len].copy_from_slice(&tmp[..len]);
        Ok(len)
    }
}

impl fatfs::Write for BlockDeviceIo {
    fn write(&mut self, buf: &[u8]) -> Result<usize, Self::Error> {
        let mut tmp = [0u8; 512];
        let len = buf.len().min(512);
        tmp[..len].copy_from_slice(&buf[..len]);
        let lba = 0; // TODO: track position
        let mut dev = self.device.lock();
        if dev.write_blocks(lba, &tmp).is_err() {
            return Err(());
        }
        Ok(len)
    }

    fn flush(&mut self) -> Result<(), Self::Error> {
        Ok(())
    }
}

impl fatfs::Seek for BlockDeviceIo {
    fn seek(&mut self, _pos: fatfs::SeekFrom) -> Result<u64, Self::Error> {
        // TODO: implement proper seeking
        Ok(0)
    }
}

impl fatfs::ReadSeek for BlockDeviceIo {}
impl fatfs::WriteSeek for BlockDeviceIo {}

/// FAT32 file system instance
pub struct FatFileSystem {
    // We'll store the mounted filesystem here
    // For now, just a placeholder
    mounted: bool,
}

static FAT_INSTANCE: Mutex<Option<FatFileSystem>> = Mutex::new(None);

impl FatFileSystem {
    pub fn init() -> KResult<()> {
        let mut blk = VIRTIO_BLK.lock();
        if blk.block_count() == 0 {
            log::warn!("fat: no block device available");
            return Err(Error::NotImplemented);
        }
        
        log::info!("fat: block device has {} blocks", blk.block_count());
        
        // TODO: Mount the filesystem
        // For now, just mark as initialized
        let mut inst = FAT_INSTANCE.lock();
        *inst = Some(FatFileSystem { mounted: false });
        
        Ok(())
    }
    
    pub fn instance() -> Option<&'static dyn FileSystem> {
        // Return a reference to the singleton
        None // TODO: implement
    }
}

impl FileSystem for FatFileSystem {
    fn open(&self, _path: &str) -> KResult<FileHandle> {
        Err(Error::NotImplemented)
    }
    
    fn read(&self, _handle: &mut FileHandle, _buf: &mut [u8]) -> KResult<usize> {
        Err(Error::NotImplemented)
    }
    
    fn write(&self, _handle: &mut FileHandle, _buf: &[u8]) -> KResult<usize> {
        Err(Error::NotImplemented)
    }
    
    fn close(&self, _handle: FileHandle) -> KResult<()> {
        Err(Error::NotImplemented)
    }
    
    fn list_dir(&self, _path: &str) -> KResult<Vec<String>> {
        Err(Error::NotImplemented)
    }
}
