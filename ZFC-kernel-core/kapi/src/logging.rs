//! Backend crate'a `log` -> UART. Bez tego info!/warn! są no-op.
use log::{Level, LevelFilter, Metadata, Record};

struct KernelLogger;

impl log::Log for KernelLogger {
    fn enabled(&self, m: &Metadata) -> bool {
        m.level() <= Level::Debug
    }
    fn log(&self, r: &Record) {
        if self.enabled(r.metadata()) {
            crate::kprintln!("[rust/{}] {}", r.target(), r.args());
        }
    }
    fn flush(&self) {}
}

static LOGGER: KernelLogger = KernelLogger;

pub fn init() {
    let _ = log::set_logger(&LOGGER);
    log::set_max_level(LevelFilter::Debug);
}
