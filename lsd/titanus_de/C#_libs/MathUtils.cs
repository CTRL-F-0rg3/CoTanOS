namespace System.MathExt
{
    public static class MathUtils
    {
        // Ograniczenie wartości w przedziale (Clamp)
        public static float Clamp(float val, float min, float max)
        {
            if (val < min) return min;
            if (val > max) return max;
            return val;
        }

        public static int Clamp(int val, int min, int max)
        {
            if (val < min) return min;
            if (val > max) return max;
            return val;
        }

        // Liniowa interpolacja (Lerp) do płynnych animacji okien i UI
        public static float Lerp(float a, float b, float t)
        {
            return a + (b - a) * Clamp(t, 0.0f, 1.0f);
        }

        // Sprawdzanie kolizji punktu z prostokątem (np. kliknięcie myszką w element UI)
        public static bool PointInRect(float px, float py, float rx, float ry, float rw, float rh)
        {
            return px >= rx && px <= (rx + rw) && py >= ry && py <= (ry + rh);
        }

        public static float Abs(float val) => val < 0 ? -val : val;
    }
}