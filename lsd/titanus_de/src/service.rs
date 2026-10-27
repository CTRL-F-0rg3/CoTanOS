//! Usługi (daemony) i autostart — rejestr długo żyjących procesów.
//!
//! To jest backend „aplikacji do zarządzania autostartem i daemonami": trzyma
//! listę usług, ich stan (`Stopped`/`Running`/`Failed`) i flagę autostartu.
//! Sama egzekucja procesów (spawn/kill) należy do wyższej warstwy
//! (session_manager); tutaj jest stan i decyzje, które da się testować bez
//! procesów.

pub type ServiceId = u32;

/// Stan usługi.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ServiceState {
    Stopped,
    Running,
    Failed,
}

/// Jedna usługa (daemon).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Service {
    pub id: ServiceId,
    pub name: String,
    pub state: ServiceState,
    /// Czy startuje automatycznie przy starcie systemu.
    pub autostart: bool,
}

/// Rejestr usług i autostartu.
#[derive(Debug, Clone)]
pub struct ServiceManager {
    services: Vec<Service>,
    next_id: ServiceId,
}

impl ServiceManager {
    pub fn new() -> Self {
        Self {
            services: Vec::new(),
            next_id: 1,
        }
    }

    /// Rejestruje usługę i zwraca jej id.
    pub fn add(&mut self, name: impl Into<String>, autostart: bool) -> ServiceId {
        let id = self.next_id;
        self.next_id += 1;
        self.services.push(Service {
            id,
            name: name.into(),
            state: ServiceState::Stopped,
            autostart,
        });
        id
    }

    /// Usuwa usługę; zwraca `true`, gdy istniała.
    pub fn remove(&mut self, id: ServiceId) -> bool {
        let before = self.services.len();
        self.services.retain(|s| s.id != id);
        self.services.len() != before
    }

    pub fn list(&self) -> &[Service] {
        &self.services
    }

    pub fn get(&self, id: ServiceId) -> Option<&Service> {
        self.services.iter().find(|s| s.id == id)
    }

    fn get_mut(&mut self, id: ServiceId) -> Option<&mut Service> {
        self.services.iter_mut().find(|s| s.id == id)
    }

    /// Uruchamia usługę; zwraca `false`, gdy nie istnieje.
    pub fn start(&mut self, id: ServiceId) -> bool {
        match self.get_mut(id) {
            Some(s) => {
                s.state = ServiceState::Running;
                true
            }
            None => false,
        }
    }

    /// Zatrzymuje usługę.
    pub fn stop(&mut self, id: ServiceId) -> bool {
        match self.get_mut(id) {
            Some(s) => {
                s.state = ServiceState::Stopped;
                true
            }
            None => false,
        }
    }

    /// Restartuje usługę (zatrzymaj + uruchom).
    pub fn restart(&mut self, id: ServiceId) -> bool {
        match self.get_mut(id) {
            Some(s) => {
                s.state = ServiceState::Running;
                true
            }
            None => false,
        }
    }

    /// Włącza/wyłącza autostart usługi.
    pub fn set_autostart(&mut self, id: ServiceId, enabled: bool) -> bool {
        match self.get_mut(id) {
            Some(s) => {
                s.autostart = enabled;
                true
            }
            None => false,
        }
    }

    /// Liczba działających usług.
    pub fn running_count(&self) -> usize {
        self.services
            .iter()
            .filter(|s| s.state == ServiceState::Running)
            .count()
    }

    /// Liczba usług z włączonym autostartem.
    pub fn autostart_count(&self) -> usize {
        self.services.iter().filter(|s| s.autostart).count()
    }
}

impl Default for ServiceManager {
    /// Domyślny zestaw daemonów userspace (spójny z `userspace_worspace/services`).
    fn default() -> Self {
        let mut m = Self::new();
        for (name, autostart) in [
            ("network_manager", true),
            ("audio_server", true),
            ("logger_daemon", true),
            ("vfs_server", true),
            ("display_server", true),
        ] {
            m.add(name, autostart);
        }
        m
    }
}
