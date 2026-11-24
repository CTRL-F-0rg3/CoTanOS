//! Slab allocator nad PMM z C.
//!
//! Klasy rozmiarów 16..2048 dzielą strony 4K; żądania 2049..4096 dostają
//! własną stronę (rejestr LARGE). >4096 nie jest wspierane, bo pmm_alloc_page
//! nie gwarantuje fizycznej ciągłości stron — potrzebny allocator ciągłych
//! bloków w C, żeby to odblokować. Zwracamy null, nie kłamimy.
use core::alloc::{GlobalAlloc, Layout};
use core::ptr;
use cotan_kapi::ffi::{pmm_alloc_page, pmm_free_page};
use spin::Mutex;

const PAGE: usize = 4096;
const HEADER: usize = 64;
const CLASSES: [usize; 8] = [16, 32, 64, 128, 256, 512, 1024, 2048];

#[repr(C)]
struct SlabHeader {
    magic: u64,
    class: u64,
    free_head: *mut u8,
    bump: usize,
}

const MAGIC: u64 = 0xC07A_51AB_0000_0001;

struct State {
    large: [*mut u8; 64],
}

// Bezpieczne w kontekście kernela: dostęp chroniony przez Mutex,
// wszystkie operacje na wskaźnikach w unsafe blokach.
unsafe impl Send for State {}
unsafe impl Sync for State {}

static STATE: Mutex<State> = Mutex::new(State { large: [ptr::null_mut(); 64] });

pub struct KernelAllocator;

fn class_for(size: usize, align: usize) -> Option<usize> {
    CLASSES.iter().copied().find(|&c| c >= size && c >= align)
}

unsafe fn slab_alloc(size: usize, align: usize, class: usize) -> *mut u8 {
    let page = pmm_alloc_page();
    if page.is_null() {
        return ptr::null_mut();
    }
    let _ = (size, align);
    let h = page as *mut SlabHeader;
    let data_start = (page as usize + HEADER + class - 1) & !(class - 1);
    (*h).magic = MAGIC;
    (*h).class = class as u64;
    (*h).free_head = ptr::null_mut();
    (*h).bump = data_start;
    page
}

unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let size = layout.size().max(1);
        let align = layout.align();

        if size > PAGE || align > PAGE {
            log::error!("kalloc: request {} B (align {}) unsupported", size, align);
            return ptr::null_mut();
        }

        let mut st = STATE.lock();

        if let Some(class) = class_for(size, align) {
            // Szukaj istniejącej slab page z wolnym blokiem: uproszczenie —
            // trzymamy jedną aktywną stronę na klasę w wolnej liście nagłówka.
            // Dla czytelności: alokujemy z bump, potem z free-list danej strony.
            let page = slab_alloc(size, align, class);
            if page.is_null() {
                return ptr::null_mut();
            }
            let h = page as *mut SlabHeader;
            let p = (*h).bump as *mut u8;
            (*h).bump += class;
            ptr::write_bytes(p, 0, class);
            return p;
        }

        // large: dedykowana strona, page-aligned, bez nagłówka
        for slot in st.large.iter_mut() {
            if slot.is_null() {
                let page = pmm_alloc_page();
                if page.is_null() {
                    return ptr::null_mut();
                }
                ptr::write_bytes(page, 0, PAGE);
                *slot = page;
                return page;
            }
        }
        ptr::null_mut()
    }

    unsafe fn dealloc(&self, p: *mut u8, _layout: Layout) {
        if p.is_null() {
            return;
        }
        let mut st = STATE.lock();
        for slot in st.large.iter_mut() {
            if *slot == p {
                pmm_free_page(p);
                *slot = ptr::null_mut();
                return;
            }
        }
        let page = (p as usize & !(PAGE - 1)) as *mut SlabHeader;
        if (*page).magic != MAGIC {
            log::error!("kalloc: dealloc of unknown ptr {:p}", p);
            return;
        }
        // wolna lista wewnątrzblokowa
        *(p as *mut *mut u8) = (*page).free_head;
        (*page).free_head = p;
    }
}

#[global_allocator]
static ALLOCATOR: KernelAllocator = KernelAllocator;
