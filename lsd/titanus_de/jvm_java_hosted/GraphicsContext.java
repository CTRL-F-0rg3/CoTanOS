package system.framework;

public class GraphicsContext {

    // Te metody w przyszłości mogą przekazywać bajty do bufora ramki (framebuffer) w C++
    public static void drawRect(int x, int y, int width, int height, int color) {
        System.out.println("[GraphicsEngine] Rysuję prostokąt: [" + x + ", " + y + ", " + width + "x" + height + "] kolorem 0x" + Integer.toHexString(color));
        // Tutaj wpięlibyśmy wywołanie natywne do C++ (np. nativeDrawRect)
    }

    public static void drawString(String text, int x, int y, int color) {
        System.out.println("[GraphicsEngine] Tekst \"" + text + "\" na pozycji (" + x + ", " + y + ")");
    }

    public static void clearScreen(int color) {
        System.out.println("[GraphicsEngine] Czyszczenie ekranu kolorem: 0x" + Integer.toHexString(color));
    }
}