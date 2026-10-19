//! C ABI — „interfejs” DE, którym może sterować dowolny język (C#, C, ...).
//!
//! To jest granica, na której siedzi warstwa interfejsu: C# (albo C, Odin,
//! Ada...) woła te funkcje i steruje środowiskiem. Funkcje są `extern "C"` i
//! `#[no_mangle]`, a na granicy są tylko wskaźniki i liczby — żadnych
//! `Vec`/`String` w sygnaturach, żeby ABI było banalne do związania.

use std::ffi::CStr;
use std::os::raw::c_char;

use crate::de::Titanus;
use crate::geometry::{Rect, Size};
use crate::theme::Theme;

/// Nieprzezroczysty uchwyt DE: `Titanus` plus własny framebuffer, do którego
/// renderuje `titanus_render` i który odczytuje `titanus_framebuffer_ptr`.
pub struct FfiDe {
    de: Titanus,
    fb: Vec<u32>,
}

/// Tworzy DE o wymiarach `w` × `h`. Zwraca `null` przy nieudanych wymiarach.
#[no_mangle]
pub extern "C" fn titanus_new(w: u32, h: u32) -> *mut FfiDe {
    if w == 0 || h == 0 {
        return std::ptr::null_mut();
    }
    let de = Titanus::new(Size::new(w, h));
    let fb = vec![0u32; (w as usize) * (h as usize)];
    Box::into_raw(Box::new(FfiDe { de, fb }))
}

/// Zwalnia DE.
#[no_mangle]
pub extern "C" fn titanus_free(de: *mut FfiDe) {
    if de.is_null() {
        return;
    }
    unsafe {
        drop(Box::from_raw(de));
    }
}

/// Tworzy okno kafelkowe; zwraca id (0 = błąd).
#[no_mangle]
pub extern "C" fn titanus_spawn_tiled(de: *mut FfiDe, title: *const c_char) -> u32 {
    unsafe {
        let title = cstr_to_string(title);
        (*de).de.spawn_tiled(title)
    }
}

/// Tworzy okno pływające; zwraca id.
#[no_mangle]
pub extern "C" fn titanus_spawn_floating(
    de: *mut FfiDe,
    title: *const c_char,
    x: i32,
    y: i32,
    w: u32,
    h: u32,
) -> u32 {
    unsafe {
        let title = cstr_to_string(title);
        (*de).de.spawn_floating(title, Rect::new(x, y, w, h))
    }
}

/// Skupia kolejne okno (cyklicznie).
#[no_mangle]
pub extern "C" fn titanus_focus_next(de: *mut FfiDe) {
    unsafe {
        (*de).de.focus_next();
    }
}

/// Ustawia motyw z JSON (format `theme_cs`). Zwraca 0 (ok) lub -1 (błąd).
#[no_mangle]
pub extern "C" fn titanus_set_theme_json(de: *mut FfiDe, json: *const c_char) -> i32 {
    unsafe {
        match Theme::from_json(&cstr_to_string(json)) {
            Ok(t) => {
                (*de).de.theme = t;
                0
            }
            Err(_) => -1,
        }
    }
}

/// Renderuje ramkę do wewnętrznego framebuffera.
#[no_mangle]
pub extern "C" fn titanus_render(de: *mut FfiDe) {
    unsafe {
        let de = &mut *de;
        de.de.render(&mut de.fb);
    }
}

/// Wskaźnik do framebuffera (XRGB8888, `width * height` pikseli).
#[no_mangle]
pub extern "C" fn titanus_framebuffer_ptr(de: *const FfiDe) -> *const u32 {
    unsafe { (*de).fb.as_ptr() }
}

#[no_mangle]
pub extern "C" fn titanus_width(de: *const FfiDe) -> u32 {
    unsafe { (*de).de.screen.w }
}

#[no_mangle]
pub extern "C" fn titanus_height(de: *const FfiDe) -> u32 {
    unsafe { (*de).de.screen.h }
}

// ── usługi (daemony) i autostart ────────────────────────────────────────────
//
// Dostęp po indeksie (lista). Mutacje zwracają 0 (ok) lub -1 (błąd/brak).

#[no_mangle]
pub extern "C" fn titanus_service_count(de: *const FfiDe) -> u32 {
    unsafe { (*de).de.services.list().len() as u32 }
}

#[no_mangle]
pub extern "C" fn titanus_service_name(de: *const FfiDe, index: u32, buf: *mut c_char, buf_len: u32) -> u32 {
    unsafe {
        let Some(svc) = (*de).de.services.list().get(index as usize) else {
            if !buf.is_null() && buf_len > 0 {
                *buf = 0;
            }
            return 0;
        };
        let name = svc.name.as_bytes();
        if !buf.is_null() && buf_len > 0 {
            let n = name.len().min((buf_len - 1) as usize);
            std::ptr::copy_nonoverlapping(name.as_ptr(), buf as *mut u8, n);
            *buf.add(n) = 0;
        }
        name.len() as u32
    }
}

#[no_mangle]
pub extern "C" fn titanus_service_state(de: *const FfiDe, index: u32) -> i32 {
    unsafe {
        match (*de).de.services.list().get(index as usize) {
            Some(s) => match s.state {
                crate::service::ServiceState::Stopped => 0,
                crate::service::ServiceState::Running => 1,
                crate::service::ServiceState::Failed => 2,
            },
            None => -1,
        }
    }
}

#[no_mangle]
pub extern "C" fn titanus_service_autostart(de: *const FfiDe, index: u32) -> i32 {
    unsafe {
        match (*de).de.services.list().get(index as usize) {
            Some(s) => i32::from(s.autostart),
            None => -1,
        }
    }
}

#[no_mangle]
pub extern "C" fn titanus_service_start(de: *mut FfiDe, index: u32) -> i32 {
    unsafe {
        match (*de).de.services.list().get(index as usize).map(|s| s.id) {
            Some(id) if (*de).de.services.start(id) => 0,
            _ => -1,
        }
    }
}

#[no_mangle]
pub extern "C" fn titanus_service_stop(de: *mut FfiDe, index: u32) -> i32 {
    unsafe {
        match (*de).de.services.list().get(index as usize).map(|s| s.id) {
            Some(id) if (*de).de.services.stop(id) => 0,
            _ => -1,
        }
    }
}

#[no_mangle]
pub extern "C" fn titanus_service_set_autostart(de: *mut FfiDe, index: u32, enabled: i32) -> i32 {
    unsafe {
        match (*de).de.services.list().get(index as usize).map(|s| s.id) {
            Some(id) if (*de).de.services.set_autostart(id, enabled != 0) => 0,
            _ => -1,
        }
    }
}

/// Zamienia wskaźnik C na `String`; `null` daje pusty napis.
fn cstr_to_string(p: *const c_char) -> String {
    if p.is_null() {
        return String::new();
    }
    unsafe { CStr::from_ptr(p) }.to_string_lossy().into_owned()
}
