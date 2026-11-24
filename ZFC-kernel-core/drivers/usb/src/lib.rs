#![no_std]

use cotan_kapi::{ffi::pmm_alloc_page, ffi::pmm_free_page, KResult, Error};
use crab_usb::{KernelOp, USBHost};
use dma_api::{DmaAllocHandle, DmaCoherency, DmaConstraints, DmaDeviceInfo, DmaDirection, DmaDomainId, DmaMapHandle, DmaOp};
use spin::Mutex;
use core::alloc::Layout;
use core::num::NonZeroUsize;
use core::ptr::NonNull;
use core::time::Duration;

// --- KernelOp Stub ---
struct CotanKernelOp;

impl KernelOp for CotanKernelOp {
    fn delay(&self, duration: Duration) {
        // Proste opóźnienie busy-wait (w produkcji użyj timera sprzętowego)
        let loops = duration.as_micros() as u64 * 100;
        for _ in 0..loops {
            core::hint::spin_loop();
        }
    }
}

static KERNEL_OP: CotanKernelOp = CotanKernelOp;

// --- DmaOp Stub ---
struct CotanDmaOp;

impl DmaOp for CotanDmaOp {
    fn page_size(&self) -> usize {
        4096
    }

    unsafe fn alloc_contiguous(
        &self,
        _constraints: DmaConstraints,
        layout: Layout,
    ) -> Option<DmaAllocHandle> {
        let ptr = pmm_alloc_page() as *mut u8;
        if ptr.is_null() {
            return None;
        }
        core::ptr::write_bytes(ptr, 0, layout.size().max(4096));
        
        let cpu_addr = NonNull::new(ptr).unwrap();
        let dma_addr = dma_api::DmaAddr::from(ptr as usize as u64);
        
        Some(unsafe { DmaAllocHandle::new(cpu_addr, cpu_addr, dma_addr, layout) })
    }

    unsafe fn dealloc_contiguous(&self, handle: DmaAllocHandle) {
        pmm_free_page(handle.as_ptr().as_ptr());
    }

    unsafe fn alloc_coherent(
        &self,
        constraints: DmaConstraints,
        layout: Layout,
    ) -> Option<DmaAllocHandle> {
        self.alloc_contiguous(constraints, layout)
    }

    unsafe fn dealloc_coherent(&self, handle: DmaAllocHandle) -> Result<(), dma_api::DmaError> {
        self.dealloc_contiguous(handle);
        Ok(())
    }

    unsafe fn map_streaming(
        &self,
        _constraints: DmaConstraints,
        addr: NonNull<u8>,
        size: NonZeroUsize,
        _direction: DmaDirection,
    ) -> Result<DmaMapHandle, dma_api::DmaError> {
        let dma_addr = dma_api::DmaAddr::from(addr.as_ptr() as usize as u64);
        let layout = Layout::from_size_align(size.get(), 1).unwrap();
        Ok(unsafe { DmaMapHandle::new(addr, dma_addr, layout, None) })
    }

    unsafe fn unmap_streaming(&self, _handle: DmaMapHandle) {
        // QEMU jest coherent, więc nic nie robimy
    }
}

static DMA_OP: CotanDmaOp = CotanDmaOp;

// --- Główny kontroler ---
pub struct UsbController {
    ready: bool,
}

impl UsbController {
    pub const fn new() -> Self {
        Self { ready: false }
    }

    pub fn init(&mut self, mmio_base: usize) -> KResult<()> {
        if mmio_base == 0 {
            log::warn!("usb: no controller base provided, skipping");
            return Err(Error::NotImplemented);
        }

        log::info!("usb: initializing crab-usb @ {:#x}", mmio_base);

        let mmio_ptr = mmio_base as *mut u8;
        let mmio = NonNull::new(mmio_ptr).ok_or(Error::Invalid)?;
        
        let device_info = DmaDeviceInfo::new(
            DmaDomainId::Direct, // Mapowanie tożsamościowe (bez IOMMU)
            DmaCoherency::Coherent,
            DmaConstraints::new(!0), // Brak ograniczeń adresowych
        );
        let dma = dma_api::DeviceDma::new(device_info, &DMA_OP);

        match USBHost::new_xhci(mmio, dma, &KERNEL_OP) {
            Ok(_host) => {
                log::info!("usb: xHCI host created successfully");
                self.ready = true;
                Ok(())
            }
            Err(e) => {
                log::error!("usb: failed to create xHCI host: {:?}", e);
                Err(Error::Io)
            }
        }
    }

    pub fn is_ready(&self) -> bool {
        self.ready
    }
}

pub static USB: Mutex<UsbController> = Mutex::new(UsbController::new());

pub fn init_usb(mmio_base: usize) -> KResult<()> {
    USB.lock().init(mmio_base)
}
