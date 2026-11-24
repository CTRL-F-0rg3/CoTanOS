use crate::hal::CotanHal;
use cotan_kapi::{KResult, Error, device::NetDevice};
use virtio_drivers::transport::mmio::{MmioTransport, VirtIOHeader};
use virtio_drivers::device::net::{VirtIONet, TxBuffer};
use core::ptr::NonNull;
use spin::Mutex;

pub struct VirtioNetDriver {
    inner: Option<VirtIONet<CotanHal, MmioTransport<'static>, 64>>,
    ready: bool,
}

impl VirtioNetDriver {
    pub const fn new() -> Self { Self { inner: None, ready: false } }

    pub fn init(&mut self, mmio_base: usize) -> KResult<()> {
        if mmio_base == 0 { return Err(Error::NotImplemented); }
        log::info!("net: probing virtio-drivers @ {:#x}", mmio_base);

        let header_ptr = mmio_base as *mut VirtIOHeader;
        let header = unsafe { NonNull::new_unchecked(header_ptr) };
        
        let transport = unsafe {
            MmioTransport::new(header, 0x200).map_err(|e| {
                log::debug!("net: MmioTransport failed @ {:#x}: {:?}", mmio_base, e);
                Error::Invalid
            })?
        };

        let net = VirtIONet::<CotanHal, MmioTransport<'static>, 64>::new(transport, 1536).map_err(|e| {
            log::debug!("net: VirtIONet::new failed @ {:#x}: {:?}", mmio_base, e);
            Error::Invalid
        })?;

        let mac = net.mac_address();
        
        // Validate MAC address
        if mac == [0; 6] || mac[0] == 0 && mac[1] == 0 && mac[2] == 0 {
            log::warn!("net: suspicious MAC {:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        } else {
            log::info!("net: SUCCESS! MAC address: {:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }

        self.inner = Some(net);
        self.ready = true;
        Ok(())
    }
}

impl NetDevice for VirtioNetDriver {
    fn mac(&self) -> [u8; 6] { 
        self.inner.as_ref().map(|n| n.mac_address()).unwrap_or([0; 6]) 
    }
    
    fn send(&mut self, frame: &[u8]) -> KResult<()> {
        let net = self.inner.as_mut().ok_or(Error::Invalid)?;
        let tx_buf = TxBuffer::from(frame);
        net.send(tx_buf).map_err(|e| {
            log::error!("net: send failed: {:?}", e);
            Error::Io
        })
    }
    
    fn recv(&mut self, out: &mut [u8]) -> KResult<usize> {
        let net = self.inner.as_mut().ok_or(Error::Invalid)?;
        match net.receive() {
            Ok(rx_buf) => {
                let packet = rx_buf.packet();
                let len = packet.len().min(out.len());
                out[..len].copy_from_slice(&packet[..len]);
                net.recycle_rx_buffer(rx_buf).map_err(|_| Error::Io)?;
                Ok(len)
            }
            Err(e) => {
                if matches!(e, virtio_drivers::Error::NotReady) {
                    Ok(0) // No packet available, not an error
                } else {
                    log::error!("net: receive failed: {:?}", e);
                    Err(Error::Io)
                }
            }
        }
    }
}

pub static VIRTIO_NET: Mutex<VirtioNetDriver> = Mutex::new(VirtioNetDriver::new());
