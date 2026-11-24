//! Thermal management module.
//! Monitors system temperature and triggers protective actions.

use cotan_kapi::kprintln;
use crate::power;

/// Temperature threshold for issuing a warning (Celsius)
pub const TEMP_WARNING_C: i32 = 100;
/// Temperature threshold for emergency shutdown (Celsius)
pub const TEMP_CRITICAL_C: i32 = 110;

/// Reads the current system temperature.
/// TODO: On real hardware (e.g., VisionFive 2), implement I2C/ADC read 
/// or query the appropriate SBI/firmware interface here.
pub fn read_temperature() -> i32 {
    // Mock value for QEMU environment
    45
}

/// Checks the current thermal status and takes action if necessary.
pub fn check_thermal_status() {
    let temp = read_temperature();
    
    if temp >= TEMP_CRITICAL_C {
        kprintln!("[thermal] CRITICAL: Temperature {}C exceeds limit ({}C)! Shutting down.", temp, TEMP_CRITICAL_C);
        // Emergency shutdown to protect hardware
        power::shutdown();
    } else if temp >= TEMP_WARNING_C {
        kprintln!("[thermal] WARNING: Temperature {}C is high (limit: {}C).", temp, TEMP_WARNING_C);
        // TODO: In a full implementation, trigger CPU throttling here
    } else {
        kprintln!("[thermal] Temperature is normal: {}C", temp);
    }
}
