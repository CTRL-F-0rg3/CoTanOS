#![no_std]
extern crate alloc;

pub mod cotanfs;
pub mod scheme;

use cotan_kapi::{KResult, Error, device::BlockDevice, kprintln};
use cotan_storage::virtio::VIRTIO_BLK;
use cotanfs::{CoTanFS, BLOCK_SIZE, MAGIC};

pub fn init() -> KResult<()> {
    kprintln!("[fs] initializing CoTanFS...");
    
    let mut blk = VIRTIO_BLK.lock();
    let block_count = blk.block_count();
    
    if block_count == 0 {
        kprintln!("[fs] no block device available, skipping");
        return Ok(());
    }

    let mut sb_buf = [0u8; BLOCK_SIZE];
    if blk.read_blocks(0, &mut sb_buf).is_err() {
        kprintln!("[fs] failed to read block 0");
        return Err(Error::Io);
    }

    let magic = u32::from_le_bytes([sb_buf[0], sb_buf[1], sb_buf[2], sb_buf[3]]);
    
    if magic != MAGIC {
        kprintln!("[fs] disk is unformatted (magic={:#x}), formatting now...", magic);
        let fs = CoTanFS::init(block_count as u32)?;
        fs.format_device(&mut *blk)?;
    } else {
        kprintln!("[fs] disk is already formatted, mounting...");
        let _fs = CoTanFS::init(block_count as u32)?;
        _fs.test_read_root(&mut *blk)?;
    }

    kprintln!("[fs] CoTanFS ready. Disk has {} blocks", block_count);
    Ok(())
}
