//! Proste stuby dla ax-sync, wymagane przez crab-usb.
//! W pełnym kernelu tutaj byłyby prawdziwe operacje na schedulerze i przerwaniami.
#![allow(dead_code, unused_variables, clippy::missing_safety_doc)]

use core::sync::atomic::{AtomicU8, AtomicUsize, Ordering};

// --- Context Ops ---
#[no_mangle] pub extern "C" fn __ContextOps_enter() {}
#[no_mangle] pub extern "C" fn __ContextOps_exit() {}
#[no_mangle] pub extern "C" fn __ContextOps_hardirq_enter() {}
#[no_mangle] pub extern "C" fn __ContextOps_hardirq_exit() {}
#[no_mangle] pub extern "C" fn __ContextOps_irq_return_preempt_enter() {}
#[no_mangle] pub extern "C" fn __ContextOps_irq_return_preempt_exit() {}

// --- Spin Ops (używamy AtomicU8 jako opaque lock) ---
#[no_mangle] pub extern "C" fn __SpinOps_acquire(lock: *mut u8) {
    let atomic = unsafe { &*(lock as *const AtomicU8) };
    while atomic.swap(1, Ordering::Acquire) != 0 {
        core::hint::spin_loop();
    }
}
#[no_mangle] pub extern "C" fn __SpinOps_release(lock: *mut u8) {
    let atomic = unsafe { &*(lock as *const AtomicU8) };
    atomic.store(0, Ordering::Release);
}
#[no_mangle] pub extern "C" fn __SpinOps_try_acquire(lock: *mut u8) -> bool {
    let atomic = unsafe { &*(lock as *const AtomicU8) };
    atomic.compare_exchange(0, 1, Ordering::Acquire, Ordering::Relaxed).is_ok()
}
#[no_mangle] pub extern "C" fn __SpinOps_is_locked(lock: *mut u8) -> bool {
    let atomic = unsafe { &*(lock as *const AtomicU8) };
    atomic.load(Ordering::Relaxed) != 0
}
#[no_mangle] pub extern "C" fn __SpinOps_force_release(lock: *mut u8) {
    let atomic = unsafe { &*(lock as *const AtomicU8) };
    atomic.store(0, Ordering::Release);
}

// --- RwLock Ops (uproszczone do mutexa dla stuba) ---
#[no_mangle] pub extern "C" fn __RwLockOps_acquire(lock: *mut u8) {
    let atomic = unsafe { &*(lock as *const AtomicUsize) };
    while atomic.swap(1, Ordering::Acquire) != 0 {
        core::hint::spin_loop();
    }
}
#[no_mangle] pub extern "C" fn __RwLockOps_release(lock: *mut u8) {
    let atomic = unsafe { &*(lock as *const AtomicUsize) };
    atomic.store(0, Ordering::Release);
}
#[no_mangle] pub extern "C" fn __RwLockOps_try_acquire(lock: *mut u8) -> bool {
    let atomic = unsafe { &*(lock as *const AtomicUsize) };
    atomic.compare_exchange(0, 1, Ordering::Acquire, Ordering::Relaxed).is_ok()
}
#[no_mangle] pub extern "C" fn __RwLockOps_force_read_decrement(lock: *mut u8) {
    let atomic = unsafe { &*(lock as *const AtomicUsize) };
    atomic.fetch_sub(1, Ordering::Release);
}

// --- Lockdep Ops ---
#[no_mangle] pub extern "C" fn __LockdepOps_dump_trace() {}
#[no_mangle] pub extern "C" fn __LockdepOps_set_trace_enabled(_enabled: bool) {}
