// crates/loomcos-core/src/session.rs

use crate::surface::SurfaceId;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ProcessType {
    /// Heavy full application process fetching data and executing logic
    FullApplication,
    /// Lightweight system background helper, decoration renderer, or daemon
    SystemHelper,
    /// Internal compositor core task
    CompositorInternal,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct ProcessId(pub u32);

pub struct WindowSession {
    pub pid: ProcessId,
    pub process_type: ProcessType,
    pub surface_id: SurfaceId,
    pub title: String,
    pub is_responsive: bool,
}

impl WindowSession {
    pub fn new(pid: ProcessId, process_type: ProcessType, surface_id: SurfaceId, title: &str) -> Self {
        Self {
            pid,
            process_type,
            surface_id,
            title: title.to_string(),
            is_responsive: true,
        }
    }
}