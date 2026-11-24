//! Zarządzanie procesami i wątkami

use spin::Mutex;
use alloc::vec::Vec;

/// Stan procesu
#[derive(Debug, Clone, Copy, PartialEq)]
pub enum ProcessState {
    Ready,
    Running,
    Blocked,
    Zombie,
}

/// Struktura procesu
pub struct Process {
    pub pid: u64,
    pub state: ProcessState,
    pub name: [u8; 64],
    // TODO: Context, page table, etc.
}

impl Process {
    pub fn new(pid: u64, name: &str) -> Self {
        let mut name_arr = [0u8; 64];
        let bytes = name.as_bytes();
        let len = bytes.len().min(63);
        name_arr[..len].copy_from_slice(&bytes[..len]);
        
        Self {
            pid,
            state: ProcessState::Ready,
            name: name_arr,
        }
    }
}

/// Scheduler procesów
pub struct Scheduler {
    processes: Vec<Process>,
    #[allow(dead_code)]
    current_pid: Option<u64>,
}

impl Scheduler {
    pub const fn new() -> Self {
        Self {
            processes: Vec::new(),
            current_pid: None,
        }
    }

    /// Dodaj proces do schedulera
    pub fn add_process(&mut self, process: Process) {
        self.processes.push(process);
    }

    /// Przełącz na następny proces
    pub fn schedule(&mut self) -> Option<u64> {
        // TODO: Implementacja round-robin lub priorytetowego
        None
    }
}

pub static SCHEDULER: Mutex<Scheduler> = Mutex::new(Scheduler::new());
