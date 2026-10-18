// controller.rs

use core::ptr;
use crate::drivers::aac::pci::AacSoftc;
use crate::drivers::aac::queues::{QueueManager, ADAP_NORM_CMD, HOST_NORM_RESP, ADAP_NORM_RESP};
use crate::drivers::aac::fib::*;
use crate::drivers::aac::regs::*;

pub struct AacController {
    pub softc: AacSoftc,
    pub queues: QueueManager,
    // TODO: Add a lock/mutex for FIB pool allocation if SMP is used
    // TODO: Add a list of pending I/O requests waiting for completion
}

impl AacController {
    pub fn new(softc: AacSoftc) -> Self {
        Self {
            softc,
            queues: QueueManager::new(),
        }
    }

    // ==========================================
    // Low-level Register Access
    // ==========================================

    fn write_doorbell_in(&self, mask: u32) {
        self.softc.write_reg(rx::IDBR, mask);
    }

    fn read_doorbell_out(&self) -> u32 {
        self.softc.read_reg(rx::ODBR)
    }

    fn clear_doorbell_out(&self, mask: u32) {
        self.softc.write_reg(rx::ODBR, mask);
    }

    fn read_fw_status(&self) -> u32 {
        self.softc.read_reg(rx::FWSTATUS)
    }

    // ==========================================
    // Synchronous Commands (Mailbox)
    // ==========================================

    /// Sends a synchronous FIB via the Mailbox. Used for initialization.
    /// Blocks until the adapter responds.
    pub fn send_sync_fib(&mut self, fib_index: usize) -> Result<(), &'static str> {
        let fib_phys = self.queues.get_fib_phys(fib_index) as u32;
        let fib_size = core::mem::size_of::<Fib>() as u32;

        // Write FIB physical address and size to the Mailbox registers
        // Mailbox is at offset 0x50, 20 bytes long.
        // Layout: [0..4] = Size, [4..8] = Addr, etc. (Check aacreg.h for exact layout if needed, 
        // but usually writing to the first two 32-bit words is enough for Sync FIB).
        let mb_addr = rx::MAILBOX;
        self.softc.write_reg(mb_addr, fib_size);
        self.softc.write_reg(mb_addr + 4, fib_phys);

        // Ring the doorbell to tell the adapter we have a sync command
        self.write_doorbell_in(DB_SYNC_COMMAND);

        // Poll for completion (In a real OS, you'd sleep/yield here)
        let mut timeout = 1000000;
        while timeout > 0 {
            let status = self.read_fw_status();
            if status & AAC_UP_AND_RUNNING != 0 {
                // Check if sync command is done (usually indicated by DB_SYNC_COMMAND clearing or specific FIB state)
                let fib = self.queues.get_fib(fib_index);
                if fib.header.xfer_state & fib_state::DONE_ADAP != 0 {
                    return Ok(());
                }
            }
            timeout -= 1;
            core::hint::spin_loop();
        }

        Err("Sync FIB timeout")
    }

    // ==========================================
    // Initialization Sequence
    // ==========================================

    pub fn init_adapter(&mut self) -> Result<(), &'static str> {
        // 1. Wait for adapter to pass self-test
        let mut timeout = 10000000;
        while self.read_fw_status() & AAC_SELF_TEST_FAILED == 0 {
            if timeout == 0 { return Err("Self-test timeout"); }
            timeout -= 1;
            core::hint::spin_loop();
        }

        if self.read_fw_status() & AAC_SELF_TEST_FAILED != 0 {
            return Err("Adapter self-test failed");
        }

        // 2. Send INIT_STRUCT via Sync FIB
        // TODO: Allocate a FIB, fill it with aac_adapter_init struct (physical addresses of queues),
        // and call self.send_sync_fib(fib_index).
        
        // 3. Wait for UP_AND_RUNNING
        timeout = 10000000;
        while self.read_fw_status() & AAC_UP_AND_RUNNING == 0 {
            if timeout == 0 { return Err("Boot timeout"); }
            timeout -= 1;
            core::hint::spin_loop();
        }

        // 4. Unmask interrupts
        self.softc.write_reg(rx::OIMR, 0); 

        Ok(())
    }

    // ==========================================
    // Interrupt Handler
    // ==========================================

    pub fn interrupt_handler(&mut self) {
        let odbr = self.read_doorbell_out();
        if odbr == 0 { return; } // Not our interrupt

        // Handle Response Ready
        if odbr & DB_RESPONSE_READY != 0 {
            self.process_responses();
            self.clear_doorbell_out(DB_RESPONSE_READY);
        }

        // Handle Printf requests (Adapter wants to print to host console)
        if odbr & DB_PRINTF != 0 {
            // TODO: Read printf buffer from shared memory and print to console
            self.clear_doorbell_out(DB_PRINTF);
        }
    }

    fn process_responses(&mut self) {
        // Dequeue from Host Normal Response Queue
        while let Some(fib_index) = self.queues.dequeue(HOST_NORM_RESP) {
            let fib = self.queues.get_fib(fib_index);
            
            // Check what command this was and complete it
            match fib.header.command {
                c if c == FibCommand::ContainerCommand as u16 || c == FibCommand::ContainerCommand64 as u16 => {
                    // TODO: Complete the Block I/O request associated with this FIB
                    // self.complete_io(fib_index);
                }
                _ => {
                    // Return FIB to the free pool
                }
            }
        }
    }

    // ==========================================
    // Asynchronous I/O Submission
    // ==========================================

    pub fn submit_block_read(&mut self, container_id: u32, lba: u32, byte_count: u32, sg_phys: u32, sg_count: u32) -> Result<(), &'static str> {
        // TODO: Get a free FIB from the pool
        let fib_index = 0; // Placeholder
        let fib = self.queues.get_fib(fib_index);

        fib.header.command = FibCommand::ContainerCommand as u16;
        fib.header.xfer_state = fib_state::HOSTOWNED | fib_state::INITIALISED;
        
        // Fill BlockRead payload
        let payload = fib.data.as_mut_ptr() as *mut BlockRead;
        unsafe {
            (*payload).command = 0; // VM_CtBlockRead (handled by firmware)
            (*payload).container_id = container_id;
            (*payload).block_number = lba;
            (*payload).byte_count = byte_count;
            
            // Fill Scatter-Gather table
            let sg = &mut (*payload).sg_map; // Assuming you add sg_map to BlockRead struct
            // ... fill SG entries ...
        }

        // Enqueue to Adapter Normal Command Queue
        self.queues.enqueue(ADAP_NORM_CMD, fib_index);

        // Ring the doorbell
        self.write_doorbell_in(DB_COMMAND_READY);

        Ok(())
    }
}