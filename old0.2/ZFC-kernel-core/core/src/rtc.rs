//! Real-Time Clock (RTC) driver abstraction and implementation.

use cotan_kapi::kprintln;
use crate::peripherals::i2c::I2cDevice;

/// Represents an error during RTC operations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum RtcError {
    I2cError,
    InvalidTime,
    DeviceNotResponding,
}

/// A standard time representation.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct RtcTime {
    pub year: u16,
    pub month: u8,
    pub day: u8,
    pub hour: u8,
    pub minute: u8,
    pub second: u8,
}

/// Trait defining RTC operations.
pub trait Rtc {
    /// Reads the current time from the RTC.
    fn get_time(&mut self) -> Result<RtcTime, RtcError>;
    
    /// Sets the RTC to the specified time.
    fn set_time(&mut self, time: &RtcTime) -> Result<(), RtcError>;
}

/// Driver for the PCF8563 RTC chip (common I2C RTC).
/// Datasheet: https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf
pub struct Pcf8563Rtc<I2C: I2cDevice> {
    i2c: I2C,
    i2c_addr: u8,
}

impl<I2C: I2cDevice> Pcf8563Rtc<I2C> {
    /// Creates a new PCF8563 RTC driver instance.
    /// Default I2C address for PCF8563 is 0x51.
    pub fn new(i2c: I2C) -> Self {
        Self {
            i2c,
            i2c_addr: 0x51,
        }
    }

    /// Converts Binary Coded Decimal (BCD) to binary.
    fn bcd_to_bin(&self, bcd: u8) -> u8 {
        (bcd & 0x0F) + ((bcd / 16) * 10)
    }

    /// Converts binary to Binary Coded Decimal (BCD).
    fn bin_to_bcd(&self, bin: u8) -> u8 {
        ((bin / 10) << 4) | (bin % 10)
    }
}

impl<I2C: I2cDevice> Rtc for Pcf8563Rtc<I2C> {
    fn get_time(&mut self) -> Result<RtcTime, RtcError> {
        // PCF8563 time registers start at offset 0x02 (seconds)
        let reg_addr = [0x02];
        let mut buf = [0u8; 7]; // seconds, minutes, hours, days, weekdays, months, years
        
        self.i2c.write_read(self.i2c_addr, &reg_addr, &mut buf)
            .map_err(|_| RtcError::I2cError)?;

        let second = self.bcd_to_bin(buf[0] & 0x7F);
        let minute = self.bcd_to_bin(buf[1] & 0x7F);
        let hour = self.bcd_to_bin(buf[2] & 0x3F);
        let day = self.bcd_to_bin(buf[3] & 0x3F);
        let month = self.bcd_to_bin(buf[5] & 0x1F);
        let year = self.bcd_to_bin(buf[6]) as u16 + 2000; // PCF8563 assumes 20xx

        Ok(RtcTime {
            year,
            month,
            day,
            hour,
            minute,
            second,
        })
    }

    fn set_time(&mut self, time: &RtcTime) -> Result<(), RtcError> {
        let mut buf = [
            0x02, // Start register
            self.bin_to_bcd(time.second),
            self.bin_to_bcd(time.minute),
            self.bin_to_bcd(time.hour),
            self.bin_to_bcd(time.day),
            0x00, // Weekday (ignored for simplicity)
            self.bin_to_bcd(time.month),
            self.bin_to_bcd((time.year - 2000) as u8),
        ];

        self.i2c.write(self.i2c_addr, &buf)
            .map_err(|_| RtcError::I2cError)?;
            
        Ok(())
    }
}
