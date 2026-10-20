package system.framework;

import java.util.HashMap;
import java.util.Map;

public class ServiceContainer {
    private static final Map<String, Object> services = new HashMap<>();

    public static void register(String name, Object service) {
        services.put(name, service);
    }

    @SuppressWarnings("unchecked")
    public static <T> T get(String serviceName) {
        Object service = services.get(serviceName);
        if (service == null) {
            throw new RuntimeException("[ServiceContainer] Nie znaleziono usługi: " + serviceName);
        }
        return (T) service;
    }
}