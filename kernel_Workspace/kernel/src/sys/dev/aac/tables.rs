// tables.rs

pub const AAC_HWIF_I960RX: u32 = 1;
pub const AAC_HWIF_STRONGARM: u32 = 2;
pub const AAC_HWIF_RKT: u32 = 3;
pub const AAC_HWIF_NARK: u32 = 4;
pub const AAC_HWIF_UNKNOWN: u32 = 0;

pub const AAC_FLAGS_NO4GB: u32 = 0x0001;
pub const AAC_FLAGS_256FIBS: u32 = 0x0002;
pub const AAC_FLAGS_BROKEN_MEMMAP: u32 = 0x0004;
pub const AAC_FLAGS_PERC2QC: u32 = 0x0008;
pub const AAC_FLAGS_NOMSI: u32 = 0x0010;
pub const AAC_FLAGS_LBA_64BIT: u32 = 0x0020;

#[repr(C)]
#[derive(Clone, Copy)]
pub struct PciId {
    pub vendor: u16,
    pub device: u16,
    pub subvendor: u16,
    pub subdevice: u16,
    pub hwif: u32,
    pub quirks: u32,
    pub desc: &'static str,
}

pub static AAC_IDENTIFIERS: &[PciId] = &[
    PciId { vendor: 0x1028, device: 0x0001, subvendor: 0x1028, subdevice: 0x0001, hwif: AAC_HWIF_I960RX, quirks: 0, desc: "Dell PERC 2/Si" },
    PciId { vendor: 0x1028, device: 0x0002, subvendor: 0x1028, subdevice: 0x0002, hwif: AAC_HWIF_I960RX, quirks: 0, desc: "Dell PERC 3/Di" },
    PciId { vendor: 0x1028, device: 0x0003, subvendor: 0x1028, subdevice: 0x0003, hwif: AAC_HWIF_I960RX, quirks: 0, desc: "Dell PERC 3/Si" },
    PciId { vendor: 0x1028, device: 0x0004, subvendor: 0x1028, subdevice: 0x00d0, hwif: AAC_HWIF_I960RX, quirks: 0, desc: "Dell PERC 3/Si" },
    PciId { vendor: 0x9005, device: 0x0285, subvendor: 0x9005, subdevice: 0x0285, hwif: AAC_HWIF_I960RX, quirks: AAC_FLAGS_NO4GB | AAC_FLAGS_256FIBS, desc: "Adaptec SCSI RAID 2200S" },
    PciId { vendor: 0x9005, device: 0x0286, subvendor: 0x9005, subdevice: 0x028c, hwif: AAC_HWIF_RKT, quirks: AAC_FLAGS_NOMSI, desc: "Adaptec SCSI RAID 2230S" },
    // ... (Wklej tutaj resztę z aac_pci.c, jeśli masz te konkretne karty, ale na start te najważniejsze wystarczą)
];

pub static AAC_FAMILY_IDENTIFIERS: &[PciId] = &[
    PciId { vendor: 0x9005, device: 0x0285, subvendor: 0, subdevice: 0, hwif: AAC_HWIF_I960RX, quirks: 0, desc: "Adaptec RAID Controller" },
    PciId { vendor: 0x9005, device: 0x0286, subvendor: 0, subdevice: 0, hwif: AAC_HWIF_RKT, quirks: 0, desc: "Adaptec RAID Controller" },
];

pub fn find_pci_id(vendor: u16, device: u16, subvendor: u16, subdevice: u16) -> Option<&'static PciId> {
    for id in AAC_IDENTIFIERS {
        if id.vendor == vendor && id.device == device && id.subvendor == subvendor && id.subdevice == subdevice {
            return Some(id);
        }
    }
    for id in AAC_FAMILY_IDENTIFIERS {
        if id.vendor == vendor && id.device == device {
            return Some(id);
        }
    }
    None
}

pub fn container_type_str(ct: u32) -> &'static str {
    match ct {
        0 => "None", 1 => "Volume", 2 => "RAID 1", 3 => "RAID 0", 4 => "RAID 5",
        9 => "RAID 4", 10 => "RAID 0/1", 13 => "RAID 0/5", 16 => "RAID 6",
        _ => "Unknown",
    }
}