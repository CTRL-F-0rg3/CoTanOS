#![no_std]
//! Kernel API/HAL: FFI do warstwy C (init), konsola, logger, traits urządzeń.
//! Od tego crate'a zależą driver-y i FS. Nigdy odwrotnie.

pub mod console;
pub mod device;
pub mod ffi;
pub mod logging;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Error {
    NoMem,
    Invalid,
    Io,
    NotImplemented,
}

pub type KResult<T> = Result<T, Error>;
