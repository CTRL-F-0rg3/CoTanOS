// crates/loomcos-core/src/shm.rs
use std::os::unix::io::RawFd;
use std::ffi::CString;
use std::ptr;

pub struct ShmPool {
    pub fd: RawFd,
    pub ptr: *mut u32,
    pub pixel_count: usize,
    name: CString,
}

impl ShmPool {
    // Creates a new POSIX shared memory segment and maps it
    pub fn create(name: &str, pixel_count: usize) -> Result<Self, String> {
        let c_name = CString::new(name).map_err(|_| "Invalid name")?;
        let byte_size = pixel_count * 4;

        unsafe {
            // Create SHM object
            let fd = libc::shm_open(
                c_name.as_ptr(),
                libc::O_CREAT | libc::O_EXCL | libc::O_RDWR,
                0600,
            );
            if fd < 0 {
                return Err(format!("shm_open failed: {}", std::io::Error::last_os_error()));
            }

            // Set size
            if libc::ftruncate(fd, byte_size as libc::off_t) < 0 {
                libc::close(fd);
                libc::shm_unlink(c_name.as_ptr());
                return Err("ftruncate failed".into());
            }

            // Map to memory
            let ptr = libc::mmap(
                ptr::null_mut(),
                byte_size,
                libc::PROT_READ | libc::PROT_WRITE,
                libc::MAP_SHARED,
                fd,
                0,
            );

            if ptr == libc::MAP_FAILED {
                libc::close(fd);
                libc::shm_unlink(c_name.as_ptr());
                return Err("mmap failed".into());
            }

            Ok(Self {
                fd,
                ptr: ptr as *mut u32,
                pixel_count,
                name: c_name,
            })
        }
    }

    // Returns the raw pointer for C++ FFI
    pub fn as_mut_ptr(&mut self) -> *mut u32 {
        self.ptr
    }
    
    pub fn as_ptr(&self) -> *const u32 {
        self.ptr
    }
}

impl Drop for ShmPool {
    fn drop(&mut self) {
        unsafe {
            let byte_size = self.pixel_count * 4;
            libc::munmap(self.ptr as *mut libc::c_void, byte_size);
            libc::close(self.fd);
            libc::shm_unlink(self.name.as_ptr());
        }
    }
}