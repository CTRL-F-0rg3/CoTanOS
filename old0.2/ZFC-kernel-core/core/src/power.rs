//! Power management module.
//! Handles system shutdown and reboot via SBI SRST extension.

use cotan_kapi::kprintln;

/// SBI System Reset (SRST) extension ID ('SRST' in ASCII)
const SBI_EXT_SRST: usize = 0x53525354;
/// SBI SRST Reset Type: System Shutdown
const SRST_TYPE_SHUTDOWN: usize = 0;
/// SBI SRST Reset Type: Cold Reboot
const SRST_TYPE_COLD_REBOOT: usize = 1;

/// Minimal SBI call wrapper for SRST.
#[inline(always)]
fn sbi_srst_call(reset_type: usize, reset_reason: usize) {
    let mut a0: usize;
    let _a1: usize; // Prefixed with _ to indicate it is intentionally unused
    unsafe {
        core::arch::asm!(
            "ecall",
            in("a7") SBI_EXT_SRST,
            in("a6") 0, // function ID 0 for srst_reset
            in("a0") reset_type,
            in("a1") reset_reason,
            lateout("a0") a0,
            lateout("a1") _a1,
            options(nomem, nostack)
        );
    }
    if a0 != 0 {
        kprintln!("[power] SBI SRST call failed with error code: {}", a0);
    }
}

/// Initiates a clean system shutdown.
pub fn shutdown() -> ! {
    kprintln!("[power] Initiating system shutdown via SBI...");
    sbi_srst_call(SRST_TYPE_SHUTDOWN, 0);
    
    kprintln!("[power] SBI shutdown failed or not supported. Halting CPU.");
    loop {
        unsafe { core::arch::asm!("wfi") };
    }
}

/// Initiates a system cold reboot.
pub fn reboot() -> ! {
    kprintln!("[power] Initiating system cold reboot via SBI...");
    sbi_srst_call(SRST_TYPE_COLD_REBOOT, 0);
    
    kprintln!("[power] SBI reboot failed or not supported. Halting CPU.");
    loop {
        unsafe { core::arch::asm!("wfi") };
    }
}
