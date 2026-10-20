package mathext

// Ograniczenie wartości (Clamp) dla float32
func Clamp(val, min, max float32) float32 {
	if val < min {
		return min
	}
	if val > max {
		return max
	}
	return val
}

// Ograniczenie wartości (Clamp) dla int
func ClampInt(val, min, max int) int {
	if val < min {
		return min
	}
	if val > max {
		return max
	}
	return val
}

// Liniowa interpolacja (Lerp)
func Lerp(a, b, t float32) float32 {
	return a + (b-a)*Clamp(t, 0.0f, 1.0f)
}

// Sprawdzanie kolizji punktu z prostokątem (UI / Mysz)
func PointInRect(px, py, rx, ry, rw, rh float32) bool {
	return px >= rx && px <= (rx+rw) && py >= ry && py <= (ry+rh)
}

// Wartość bezwzględna
func Abs(val float32) float32 {
	if val < 0 {
		return -val
	}
	return val
}