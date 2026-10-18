// queues.rs

use core::ptr::NonNull;
use crate::sys::dev::aac::fib::{Fib, FibHeader};
use crate::sys::dev::aac::regs::*;

pub const PRODUCER: usize = 0;
pub const CONSUMER: usize = 1;

/// Number of entries per hardware queue.
pub const HOST_NORM_CMD_ENTRIES: usize = 256;
pub const HOST_HIGH_CMD_ENTRIES: usize = 64;
pub const ADAP_NORM_CMD_ENTRIES: usize = 256;
pub const ADAP_HIGH_CMD_ENTRIES: usize = 64;
pub const HOST_NORM_RESP_ENTRIES: usize = 256;
pub const HOST_HIGH_RESP_ENTRIES: usize = 64;
pub const ADAP_NORM_RESP_ENTRIES: usize = 256;
pub const ADAP_HIGH_RESP_ENTRIES: usize = 64;

/// Total number of queues.
pub const QUEUE_COUNT: usize = 8;

pub const QUEUE_LAYOUT: [(usize, usize); 8] = [
    (0, HOST_NORM_CMD_ENTRIES),
    (core::mem::size_of::<[QueueEntry; HOST_NORM_CMD_ENTRIES]>(), HOST_HIGH_CMD_ENTRIES),
    (0, ADAP_NORM_CMD_ENTRIES),
    (0, ADAP_HIGH_CMD_ENTRIES),
    (0, HOST_NORM_RESP_ENTRIES),
    (0, HOST_HIGH_RESP_ENTRIES),
    (0, ADAP_NORM_RESP_ENTRIES),
    (0, ADAP_HIGH_RESP_ENTRIES),
];

#[repr(C, packed)]
#[derive(Clone, Copy)]
pub struct SgEntry {
    pub address: u32,
    pub byte_count: u32,
}

#[repr(C, packed)]
#[derive(Clone, Copy)]
pub struct SgTable {
    pub count: u32,
    pub entry: SgEntry,
}

/// Single entry in the hardware queue.
#[repr(C)]
#[derive(Debug, Clone, Copy, Default)]
pub struct QueueEntry {
    pub fib_size: u32,
    pub fib_addr: u32, // Physical address of the FIB
}

/// The shared memory structure mandated by the Adaptec firmware.
/// Must be 16-byte aligned.
#[repr(C)]
pub struct QueueTable {
    pub qindex: [[u32; 2]; QUEUE_COUNT],
    pub host_norm_cmd: [QueueEntry; HOST_NORM_CMD_ENTRIES],
    pub host_high_cmd: [QueueEntry; HOST_HIGH_CMD_ENTRIES],
    pub adap_norm_cmd: [QueueEntry; ADAP_NORM_CMD_ENTRIES],
    pub adap_high_cmd: [QueueEntry; ADAP_HIGH_CMD_ENTRIES],
    pub host_norm_resp: [QueueEntry; HOST_NORM_RESP_ENTRIES],
    pub host_high_resp: [QueueEntry; HOST_HIGH_RESP_ENTRIES],
    pub adap_norm_resp: [QueueEntry; ADAP_NORM_RESP_ENTRIES],
    pub adap_high_resp: [QueueEntry; ADAP_HIGH_RESP_ENTRIES],
}

/// Indices for the queues (matching FreeBSD's definitions)
pub const HOST_NORM_CMD: usize = 0;
pub const HOST_HIGH_CMD: usize = 1;
pub const ADAP_NORM_CMD: usize = 2;
pub const ADAP_HIGH_CMD: usize = 3;
pub const HOST_NORM_RESP: usize = 4;
pub const HOST_HIGH_RESP: usize = 5;
pub const ADAP_NORM_RESP: usize = 6;
pub const ADAP_HIGH_RESP: usize = 7;

/// Manages the shared memory and FIB pool.
pub struct QueueManager {
    pub qt_virt: NonNull<QueueTable>,
    pub qt_phys: u64,
    
    pub fib_pool_virt: NonNull<Fib>,
    pub fib_pool_phys: u64,
    pub fib_count: usize,
    
    // Local cache of producer/consumer indices to avoid reading MMIO/Shared memory constantly
    local_producer: [u32; QUEUE_COUNT],
    local_consumer: [u32; QUEUE_COUNT],
}

impl QueueManager {
    /// Initializes the queue manager. 
    /// WARNING: You must implement the DMA allocation in your OS!
    pub fn new() -> Self {
        // TODO: Allocate contiguous physical memory for QueueTable (size: sizeof(QueueTable), align: 16)
        // let (qt_virt, qt_phys) = crate::mm::dma_alloc_aligned(core::mem::size_of::<QueueTable>(), 16);
        
        // TODO: Allocate contiguous physical memory for FIB pool (e.g., 512 FIBs)
        // let fib_count = 512;
        // let (fib_virt, fib_phys) = crate::mm::dma_alloc_aligned(fib_count * core::mem::size_of::<Fib>(), 16);

        // Placeholder for compilation:
        panic!("TODO: Implement DMA allocation for QueueTable and FIB pool in your OS");
        
        /*
        let mut local_producer = [0; QUEUE_COUNT];
        let mut local_consumer = [0; QUEUE_COUNT];

        // Pre-fill the queue table with FIB physical addresses
        let mut fib_idx = 0;
        let fib_phys_base = fib_phys;
        let fib_size = core::mem::size_of::<Fib>() as u32;

        // Helper macro/closure to fill queues
        // ... (Fill adap_norm_cmd, host_norm_resp, etc. with sequential fib_phys addresses)
        
        Self {
            qt_virt: NonNull::new(qt_virt).unwrap(),
            qt_phys,
            fib_pool_virt: NonNull::new(fib_virt).unwrap(),
            fib_pool_phys: fib_phys,
            fib_count,
            local_producer,
            local_consumer,
        }
        */
    }

    /// Get a pointer to a specific FIB in the pool by its index.
    pub fn get_fib(&self, index: usize) -> &mut Fib {
        assert!(index < self.fib_count);
        unsafe {
            &mut *self.fib_pool_virt.as_ptr().add(index)
        }
    }

    /// Get the physical address of a FIB by its index.
    pub fn get_fib_phys(&self, index: usize) -> u32 {
        (self.fib_pool_phys + (index * core::mem::size_of::<Fib>()) as u64) as u32
    }

    /// Push a FIB to an inbound queue (Host -> Adapter).
    pub fn enqueue(&mut self, queue_id: usize, fib_index: usize) {
        let fib_phys = self.get_fib_phys(fib_index);
        let fib_size = core::mem::size_of::<Fib>() as u32;
        
        let prod_idx = self.local_producer[queue_id] as usize;
        
        // Get the specific queue array based on queue_id
        let queue_ptr = self.get_queue_ptr(queue_id);
        
        unsafe {
            let entry = &mut *queue_ptr.add(prod_idx);
            entry.fib_addr = fib_phys;
            entry.fib_size = fib_size;
        }

        // Update local and shared producer index
        self.local_producer[queue_id] = (prod_idx + 1) % self.get_queue_size(queue_id);
        unsafe {
            let qt = self.qt_virt.as_ptr();
            (*qt).qindex[queue_id][PRODUCER] = self.local_producer[queue_id];
        }
    }

    /// Pop a FIB from an outbound queue (Adapter -> Host). Returns FIB index or None.
    pub fn dequeue(&mut self, queue_id: usize) -> Option<usize> {
        unsafe {
            let qt = self.qt_virt.as_ptr();
            let shared_cons = (*qt).qindex[queue_id][CONSUMER];
            let shared_prod = (*qt).qindex[queue_id][PRODUCER];

            if self.local_consumer[queue_id] == shared_prod {
                return None; // Queue empty
            }

            let cons_idx = self.local_consumer[queue_id] as usize;
            let queue_ptr = self.get_queue_ptr(queue_id);
            let entry = &*queue_ptr.add(cons_idx);
            
            // We need to find which FIB index this physical address corresponds to.
            // Since FIBs are sequentially allocated, we can calculate the index.
            let fib_offset = entry.fib_addr as u64 - self.fib_pool_phys;
            let fib_index = (fib_offset / core::mem::size_of::<Fib>() as u64) as usize;

            self.local_consumer[queue_id] = (cons_idx + 1) % self.get_queue_size(queue_id);
            (*qt).qindex[queue_id][CONSUMER] = self.local_consumer[queue_id];

            Some(fib_index)
        }
    }

    // Helper to get the raw pointer to the start of a specific queue's entries
    fn get_queue_ptr(&self, queue_id: usize) -> *mut QueueEntry {
        unsafe {
            let qt = self.qt_virt.as_ptr();
            match queue_id {
                HOST_NORM_CMD => (*qt).host_norm_cmd.as_mut_ptr(),
                HOST_HIGH_CMD => (*qt).host_high_cmd.as_mut_ptr(),
                ADAP_NORM_CMD => (*qt).adap_norm_cmd.as_mut_ptr(),
                ADAP_HIGH_CMD => (*qt).adap_high_cmd.as_mut_ptr(),
                HOST_NORM_RESP => (*qt).host_norm_resp.as_mut_ptr(),
                HOST_HIGH_RESP => (*qt).host_high_resp.as_mut_ptr(),
                ADAP_NORM_RESP => (*qt).adap_norm_resp.as_mut_ptr(),
                ADAP_HIGH_RESP => (*qt).adap_high_resp.as_mut_ptr(),
                _ => panic!("Invalid queue ID"),
            }
        }
    }

    fn get_queue_size(&self, queue_id: usize) -> usize {
        match queue_id {
            HOST_NORM_CMD => HOST_NORM_CMD_ENTRIES,
            HOST_HIGH_CMD => HOST_HIGH_CMD_ENTRIES,
            ADAP_NORM_CMD => ADAP_NORM_CMD_ENTRIES,
            ADAP_HIGH_CMD => ADAP_HIGH_CMD_ENTRIES,
            HOST_NORM_RESP => HOST_NORM_RESP_ENTRIES,
            HOST_HIGH_RESP => HOST_HIGH_RESP_ENTRIES,
            ADAP_NORM_RESP => ADAP_NORM_RESP_ENTRIES,
            ADAP_HIGH_RESP => ADAP_HIGH_RESP_ENTRIES,
            _ => 0,
        }
    }
}