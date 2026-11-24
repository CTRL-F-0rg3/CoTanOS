//! GPIO (General Purpose Input/Output) abstraction.

use cotan_kapi::kprintln;

/// Represents an error during GPIO operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum GpioError {
    InvalidPin,
    OperationNotSupported,
}

/// Trait defining the behavior of a single GPIO pin.
pub trait GpioPin {
    /// Configures the pin as an output.
    fn set_direction_output(&mut self) -> Result<(), GpioError>;
    
    /// Configures the pin as an input.
    fn set_direction_input(&mut self) -> Result<(), GpioError>;
    
    /// Sets the pin state to high (logic 1).
    fn set_high(&mut self) -> Result<(), GpioError>;
    
    /// Sets the pin state to low (logic 0).
    fn set_low(&mut self) -> Result<(), GpioError>;
    
    /// Reads the current state of the pin. Returns true if high.
    fn is_high(&self) -> Result<bool, GpioError>;
}

/// A dummy stub implementation for testing in QEMU.
pub struct StubGpioPin {
    pin_id: u8,
    state: bool,
}

impl StubGpioPin {
    pub const fn new(pin_id: u8) -> Self {
        Self { pin_id, state: false }
    }
}

impl GpioPin for StubGpioPin {
    fn set_direction_output(&mut self) -> Result<(), GpioError> {
        kprintln!("[gpio] Stub: Pin {} set to output", self.pin_id);
        Ok(())
    }

    fn set_direction_input(&mut self) -> Result<(), GpioError> {
        kprintln!("[gpio] Stub: Pin {} set to input", self.pin_id);
        Ok(())
    }

    fn set_high(&mut self) -> Result<(), GpioError> {
        self.state = true;
        kprintln!("[gpio] Stub: Pin {} set HIGH", self.pin_id);
        Ok(())
    }

    fn set_low(&mut self) -> Result<(), GpioError> {
        self.state = false;
        kprintln!("[gpio] Stub: Pin {} set LOW", self.pin_id);
        Ok(())
    }

    fn is_high(&self) -> Result<bool, GpioError> {
        Ok(self.state)
    }
}
