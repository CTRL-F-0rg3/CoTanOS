//! SPI (Serial Peripheral Interface) abstraction.

use cotan_kapi::kprintln;

/// Represents an error during SPI operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SpiError {
    FrameError,
    Timeout,
    BusBusy,
}

/// Trait defining the behavior of an SPI controller.
pub trait SpiDevice {
    /// Performs a full-duplex transfer. Data is shifted out and in simultaneously.
    /// `write_buffer` contains data to send, `read_buffer` will be filled with received data.
    fn transfer(&mut self, write_buffer: &[u8], read_buffer: &mut [u8]) -> Result<(), SpiError>;
    
    /// Sets the SPI clock speed (in Hz).
    fn set_clock_speed(&mut self, hz: u32) -> Result<(), SpiError>;
}

/// A dummy stub implementation for testing in QEMU.
pub struct StubSpiDevice {
    bus_id: u8,
    speed_hz: u32,
}

impl StubSpiDevice {
    pub const fn new(bus_id: u8) -> Self {
        Self { bus_id, speed_hz: 1_000_000 } // Default 1 MHz
    }
}

impl SpiDevice for StubSpiDevice {
    fn transfer(&mut self, write_buffer: &[u8], read_buffer: &mut [u8]) -> Result<(), SpiError> {
        let len = write_buffer.len().min(read_buffer.len());
        kprintln!("[spi] Stub: Bus {} transferred {} bytes at {} Hz", self.bus_id, len, self.speed_hz);
        
        // Echo back the written data for testing purposes
        read_buffer[..len].copy_from_slice(&write_buffer[..len]);
        Ok(())
    }

    fn set_clock_speed(&mut self, hz: u32) -> Result<(), SpiError> {
        self.speed_hz = hz;
        kprintln!("[spi] Stub: Bus {} clock speed set to {} Hz", self.bus_id, hz);
        Ok(())
    }
}
