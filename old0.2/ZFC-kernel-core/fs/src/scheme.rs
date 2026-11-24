use spin::Mutex;
use alloc::collections::BTreeMap;
use alloc::string::String;

pub struct SchemeRegistry { schemes: BTreeMap<String, &'static str> }
static REGISTRY: Mutex<SchemeRegistry> = Mutex::new(SchemeRegistry { schemes: BTreeMap::new() });

pub fn register_scheme(name: &str, _fs: &'static str) {
    let mut reg = REGISTRY.lock();
    reg.schemes.insert(String::from(name), _fs);
    log::info!("scheme: registered '{}'", name);
}
