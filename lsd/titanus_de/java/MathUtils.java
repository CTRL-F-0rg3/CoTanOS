package system.math;

public class MathUtils {

    // Ograniczenie wartości w przedziale [min, max] (Clamp)
    public static float clamp(float val, float min, float max) {
        if (val < min) return min;
        if (val > max) return max;
        return val;
    }

    public static int clamp(int val, int min, int max) {
        if (val < min) return min;
        if (val > max) return max;
        return val;
    }

    // Liniowa interpolacja (Lerp) – niezastąpiona przy płynnych animacjach okien w GUI
    public static float lerp(float a, float b, float t) {
        return a + (b - a) * clamp(t, 0.0f, 1.0f);
    }

    // Sprawdzenie czy punkt mieści się w prostokącie (np. kliknięcie myszką w przycisk okna)
    public static boolean pointInRect(float px, float py, float rx, float ry, float rw, float rh) {
        return px >= rx && px <= (rx + rw) && py >= ry && py <= (ry + rh);
    }

    // Szybka funkcja potęgowa / absolutna
    public static float abs(float val) {
        return val < 0 ? -val : val;
    }
}