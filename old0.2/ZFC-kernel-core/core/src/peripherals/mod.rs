//! Hardware abstraction layer (HAL) for low-level peripherals.
//! This module defines hardware-agnostic traits for GPIO, I2C, and SPI.
//! Specific hardware implementations (e.g., StarFive JH7110) will implement these traits.

pub mod gpio;
pub mod i2c;
pub mod spi;

pub use gpio::GpioPin;
pub use i2c::I2cDevice;
pub use spi::SpiDevice;
