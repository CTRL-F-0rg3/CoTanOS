#![no_std]
extern crate alloc;

pub mod hal;
pub mod virtio;

use cotan_kapi::{KResult, kprintln};
use virtio::VIRTIO_BLK;

pub fn init_storage(mmio_base: usize) -> KResult<()> {
    kprintln!("[rust/cotan_storage] storage: initializing storage stack");
    VIRTIO_BLK.lock().init(mmio_base)
}
