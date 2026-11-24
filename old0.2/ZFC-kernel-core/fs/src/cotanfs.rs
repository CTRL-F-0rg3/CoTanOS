//! CoTanFS: Własny, prosty system plików inspirowany RedoxFS
use cotan_kapi::{KResult, Error, device::BlockDevice, kprintln};
use bitflags::bitflags;

pub const BLOCK_SIZE: usize = 4096;
pub const MAGIC: u32 = 0xC07A_51AB;
pub const MAX_NAME_LEN: usize = 255;
pub const DIRECT_BLOCKS: usize = 12;

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct SuperBlock {
    pub magic: u32,
    pub total_blocks: u32,
    pub total_inodes: u32,
    pub bitmap_start_block: u32,
    pub inode_start_block: u32,
    pub data_start_block: u32,
}

impl SuperBlock {
    pub const fn new(total_blocks: u32) -> Self {
        Self { 
            magic: MAGIC, 
            total_blocks, 
            total_inodes: 64, 
            bitmap_start_block: 1, 
            inode_start_block: 2, 
            data_start_block: 3 
        }
    }
    pub fn is_valid(&self) -> bool { self.magic == MAGIC }
}

bitflags! {
    pub struct InodeType: u8 { 
        const FILE = 0x01; 
        const DIR  = 0x02; 
    }
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Inode {
    pub type_flags: u8,
    pub _reserved: [u8; 3],
    pub size: u32,
    pub direct_blocks: [u32; DIRECT_BLOCKS],
    pub indirect_block: u32,
}

impl Inode {
    pub const fn new(type_flags: InodeType) -> Self {
        Self { 
            type_flags: type_flags.bits(), 
            _reserved: [0; 3], 
            size: 0, 
            direct_blocks: [0; DIRECT_BLOCKS], 
            indirect_block: 0 
        }
    }
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct DirEntry {
    pub inode_num: u32,
    pub name_len: u8,
    pub name: [u8; MAX_NAME_LEN],
}

impl DirEntry {
    // Usunięto 'const', aby uniknąć problemów z .min() w kontekście stałym
    pub fn new(inode_num: u32, name: &str) -> Self {
        let mut name_buf = [0u8; MAX_NAME_LEN];
        let bytes = name.as_bytes();
        // Prosty warunek zamiast .min(), aby uniknąć niestabilnych const traitów
        let len = if bytes.len() < MAX_NAME_LEN { bytes.len() } else { MAX_NAME_LEN };
        let mut i = 0;
        while i < len {
            name_buf[i] = bytes[i];
            i += 1;
        }
        Self { inode_num, name_len: len as u8, name: name_buf }
    }

    pub fn name_str(&self) -> &str {
        let end = self.name_len as usize;
        core::str::from_utf8(&self.name[..end]).unwrap_or("?")
    }
}

pub struct CoTanFS { 
    pub superblock: SuperBlock 
}

impl CoTanFS {
    pub fn init(total_blocks: u32) -> KResult<Self> {
        let sb = SuperBlock::new(total_blocks);
        if !sb.is_valid() { return Err(Error::Invalid); }
        Ok(Self { superblock: sb })
    }

    /// Formatuje dysk, tworząc SuperBlock, Bitmapę, Inode Root i wpis "."
    pub fn format_device(&self, device: &mut dyn BlockDevice) -> KResult<()> {
        kprintln!("[cotanfs] formatting device ({} blocks)...", self.superblock.total_blocks);
        
        let mut buf = [0u8; BLOCK_SIZE];

        // 1. Zapisz SuperBlock (blok 0)
        let sb_bytes = unsafe { 
            core::slice::from_raw_parts(&self.superblock as *const _ as *const u8, core::mem::size_of::<SuperBlock>()) 
        };
        buf[..sb_bytes.len()].copy_from_slice(sb_bytes);
        device.write_blocks(0, &buf).map_err(|_| Error::Io)?;

        // 2. Wyczyść i ustaw bitmapę (blok 1) - bloki 0, 1, 2, 3 są zajęte
        buf.fill(0);
        buf[0] = 0x0F; // Bity 0, 1, 2, 3 = 1
        device.write_blocks(self.superblock.bitmap_start_block as u64, &buf).map_err(|_| Error::Io)?;

        // 3. Zainicjalizuj Root Directory Inode (inode 0, w bloku 2)
        buf.fill(0);
        let root_inode = Inode::new(InodeType::DIR);
        let inode_bytes = unsafe {
            core::slice::from_raw_parts(&root_inode as *const _ as *const u8, core::mem::size_of::<Inode>())
        };
        buf[..inode_bytes.len()].copy_from_slice(inode_bytes);
        
        // Ustawiamy, że dane katalogu są w bloku danych 3
        let inode_mut = unsafe { &mut *(buf.as_mut_ptr() as *mut Inode) };
        inode_mut.direct_blocks[0] = self.superblock.data_start_block;
        inode_mut.size = core::mem::size_of::<DirEntry>() as u32;
        
        device.write_blocks(self.superblock.inode_start_block as u64, &buf).map_err(|_| Error::Io)?;

        // 4. Stwórz wpis "." w katalogu root (blok danych 3)
        buf.fill(0);
        let root_entry = DirEntry::new(0, ".");
        let entry_bytes = unsafe {
            core::slice::from_raw_parts(&root_entry as *const _ as *const u8, core::mem::size_of::<DirEntry>())
        };
        buf[..entry_bytes.len()].copy_from_slice(entry_bytes);
        device.write_blocks(self.superblock.data_start_block as u64, &buf).map_err(|_| Error::Io)?;

        kprintln!("[cotanfs] format complete! Disk is ready.");
        Ok(())
    }

    /// Odczytuje i wyświetla zawartość katalogu root (dla testu)
    pub fn test_read_root(&self, device: &mut dyn BlockDevice) -> KResult<()> {
        kprintln!("[cotanfs] testing root directory read...");
        
        let mut buf = [0u8; BLOCK_SIZE];
        device.read_blocks(self.superblock.data_start_block as u64, &mut buf).map_err(|_| Error::Io)?;
        
        let entry = unsafe { &*(buf.as_ptr() as *const DirEntry) };
        kprintln!("[cotanfs] root dir entry: inode={}, name='{}'", entry.inode_num, entry.name_str());
        
        Ok(())
    }
}
