#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DriverError {
    Timeout,
    Io,
    Unsupported,
    InvalidBlock,
    InvalidLength,
    NoDevice,
}

pub trait BlockDevice {
    fn name(&self) -> &'static str;
    fn block_size(&self) -> usize;
    fn block_count(&self) -> u64;

    fn read_block(&self, block: u64, buf: &mut [u8]) -> Result<(), DriverError>;
    fn write_block(&self, block: u64, buf: &[u8]) -> Result<(), DriverError>;

    fn read_blocks(&self, start: u64, count: u64, buf: &mut [u8]) -> Result<(), DriverError> {
        let bs = self.block_size();

        let expected = (count as usize)
            .checked_mul(bs)
            .ok_or(DriverError::InvalidLength)?;

        if buf.len() < expected {
            return Err(DriverError::InvalidLength);
        }

        for i in 0..count {
            let off = i as usize * bs;
            self.read_block(start + i, &mut buf[off..off + bs])?;
        }

        Ok(())
    }

    fn write_blocks(&self, start: u64, count: u64, buf: &[u8]) -> Result<(), DriverError> {
        let bs = self.block_size();

        let expected = (count as usize)
            .checked_mul(bs)
            .ok_or(DriverError::InvalidLength)?;

        if buf.len() < expected {
            return Err(DriverError::InvalidLength);
        }

        for i in 0..count {
            let off = i as usize * bs;
            self.write_block(start + i, &buf[off..off + bs])?;
        }

        Ok(())
    }
}
