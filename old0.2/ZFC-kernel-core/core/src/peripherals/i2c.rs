//! I2C (Inter-Integrated Circuit) abstraction.

use cotan_kapi::kprintln;

/// Represents an error during I2C operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum I2cError {
    BusBusy,
    NoAcknowledge,
    Timeout,
    InvalidAddress,
}

/// Trait defining the behavior of an I2C controller.
pub trait I2cDevice {
    /// Writes a sequence of bytes to a specific I2C device address.
    fn write(&mut self, addr: u8, bytes: &[u8]) -> Result<(), I2cError>;
    
    /// Reads a sequence of bytes from a specific I2C device address.
    fn read(&mut self, addr: u8, buffer: &mut [u8]) -> Result<(), I2cError>;
    
    /// Performs a combined write-then-read transaction (common for sensor register reads).
    fn write_read(&mut self, addr: u8, write_bytes: &[u8], read_buffer: &mut [u8]) -> Result<(), I2cError>;
}

/// A dummy stub implementation for testing in QEMU.
pub struct StubI2cDevice {
    bus_id: u8,
}

impl StubI2cDevice {
    pub const fn new(bus_id: u8) -> Self {
        Self { bus_id }
    }
}

impl I2cDevice for StubI2cDevice {
    fn write(&mut self, addr: u8, bytes: &[u8]) -> Result<(), I2cError> {
        kprintln!("[i2c] Stub: Bus {} wrote {} bytes to addr 0x{:02x}", self.bus_id, bytes.len(), addr);
        Ok(())
    }

    fn read(&mut self, addr: u8, buffer: &mut [u8]) -> Result<(), I2cError> {
        kprintln!("[i2c] Stub: Bus {} read {} bytes from addr 0x{:02x}", self.bus_id, buffer.len(), addr);
        // Fill with dummy data for testing
        for b in buffer.iter_mut() {
            *b = 0xAA;
        }
        Ok(())
    }

    fn write_read(&mut self, addr: u8, write_bytes: &[u8], read_buffer: &mut [u8]) -> Result<(), I2cError> {
        kprintln!("[i2c] Stub: Bus {} write-read: {} bytes out, {} bytes in, addr 0x{:02x}", 
                  self.bus_id, write_bytes.len(), read_buffer.len(), addr);
        self.read(addr, read_buffer)
    }
}
