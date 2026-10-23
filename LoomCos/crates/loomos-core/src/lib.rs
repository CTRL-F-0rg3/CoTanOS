// crates/loomcos-core/src/lib.rs

pub mod ffi;
pub mod scenegraph;
pub mod session;
pub mod surface;

use scenegraph::SceneGraph;
use session::{ProcessId, ProcessType, WindowSession};
use surface::{Surface, SurfaceId, SurfaceRole};

pub struct CompositorCore {
    width: i32,
    height: i32,
    framebuffer: Vec<u32>,
    pub scene: SceneGraph,
    sessions: Vec<WindowSession>,
    next_id: u64,
}

impl CompositorCore {
    pub fn new(width: i32, height: i32) -> Self {
        let size = (width * height) as usize;
        Self {
            width,
            height,
            framebuffer: vec![0; size],
            scene: SceneGraph::new(),
            sessions: Vec::new(),
            next_id: 1,
        }
    }

    /// Creates a tracked surface and registers its execution session
    pub fn register_window(
        &mut self,
        pid: u32,
        process_type: ProcessType,
        role: SurfaceRole,
        x: i32,
        y: i32,
        width: i32,
        height: i32,
        title: &str,
    ) -> SurfaceId {
        let id = SurfaceId(self.next_id);
        self.next_id += 1;

        let surface = Surface::new(id, role, x, y, width, height);
        self.scene.insert(surface);

        let session = WindowSession::new(ProcessId(pid), process_type, id, title);
        self.sessions.push(session);

        id
    }

    pub fn get_session_by_surface(&self, surface_id: SurfaceId) -> Option<&WindowSession> {
        self.sessions.iter().find(|s| s.surface_id == surface_id)
    }
}