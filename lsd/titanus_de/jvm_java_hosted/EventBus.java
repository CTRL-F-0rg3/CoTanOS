package system.framework;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class EventBus {

    public static class SystemEvent {
        public String type;
        public int x, y;
        public int keyCode;

        public SystemEvent(String type, int x, int y) {
            this.type = type;
            this.x = x;
            this.y = y;
        }

        public SystemEvent(String type, int keyCode) {
            this.type = type;
            this.keyCode = keyCode;
        }
    }

    public interface Listener {
        void onEvent(SystemEvent event);
    }

    private static final Map<String, List<Listener>> subscribers = new HashMap<>();

    public static void subscribe(String eventType, Listener listener) {
        subscribers.computeIfAbsent(eventType, k -> new ArrayList<>()).add(listener);
    }

    public static void publish(SystemEvent event) {
        List<Listener> listeners = subscribers.get(event.type);
        if (listeners != null) {
            for (Listener l : listeners) {
                l.onEvent(event);
            }
        }
    }
}