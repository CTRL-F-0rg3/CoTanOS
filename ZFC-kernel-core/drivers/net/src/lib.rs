#![no_std]
extern crate alloc;
pub mod hal;
pub mod virtio;
use cotan_kapi::KResult;
use virtio::VIRTIO_NET;

pub fn init_network(mmio_base: usize) -> KResult<()> {
    log::info!("net: initializing network stack");
    VIRTIO_NET.lock().init(mmio_base)
}
