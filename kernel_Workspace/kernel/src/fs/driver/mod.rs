pub mod ata_pio;
pub mod block;
pub mod lock;
pub mod port;
pub mod registry;
pub mod virtio_blk;

// Re-export the block vocabulary at the `driver` level. Callers used to reach
// for `crate::fs::driver::BlockDevice` (the `tangfs` next generation does), so
// the shorter path is provided here rather than fixing every use site.
pub use block::{BlockDevice, DriverError};

pub fn init() {
    ata_pio::probe();
    if ata_pio::ATA0.is_present() {
        registry::register(&ata_pio::ATA0);
    }
    if ata_pio::ATA1.is_present() {
        registry::register(&ata_pio::ATA1);
    }

    // The virtio-blk disk is the default block device QEMU presents, so probe
    // it alongside the ATA bus. It registers after ATA, which keeps the data
    // disk (usually the second device) reachable through `root_device`.
    if virtio_blk::probe() && virtio_blk::VBLK.is_present() {
        registry::register(&virtio_blk::VBLK);
    }
}

pub fn self_test() -> bool {
    let dev = match registry::first() {
        Some(d) => d,
        None => return false,
    };

    let mut buf = [0u8; 512];

    if dev.read_block(0, &mut buf).is_err() {
        return false;
    }

    buf[510] == 0x55 && buf[511] == 0xAA
}
