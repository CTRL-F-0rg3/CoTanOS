//! Wspólne interfejsy urządzeń. Implementacje w driver-ach in-kernel.

pub trait BlockDevice {
    fn block_size(&self) -> usize;
    fn block_count(&self) -> u64;
    fn read_blocks(&mut self, lba: u64, out: &mut [u8]) -> crate::KResult<usize>;
    fn write_blocks(&mut self, lba: u64, data: &[u8]) -> crate::KResult<usize>;
}

pub trait NetDevice {
    fn mac(&self) -> [u8; 6];
    fn send(&mut self, frame: &[u8]) -> crate::KResult<()>;
    fn recv(&mut self, out: &mut [u8]) -> crate::KResult<usize>;
}
