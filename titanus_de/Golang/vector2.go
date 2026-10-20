package mathext

import "math"

type Vector2 struct {
	X float32
	Y float32
}

// Konstruktor wektora
func NewVector2(x, y float32) Vector2 {
	return Vector2{X: x, Y: y}
}

// Dodawanie wektorów
func (v Vector2) Add(other Vector2) Vector2 {
	return Vector2{X: v.X + other.X, Y: v.Y + other.Y}
}

// Odejmowanie wektorów
func (v Vector2) Sub(other Vector2) Vector2 {
	return Vector2{X: v.X - other.X, Y: v.Y - other.Y}
}

// Mnożenie przez skalar
func (v Vector2) Mul(scalar float32) Vector2 {
	return Vector2{X: v.X * scalar, Y: v.Y * scalar}
}

// Długość wektora
func (v Vector2) Length() float32 {
	return float32(math.Sqrt(float64(v.X*v.X + v.Y*v.Y)))
}

// Normalizacja wektora
func (v Vector2) Normalize() Vector2 {
	length := v.Length()
	if length == 0 {
		return Vector2{X: 0, Y: 0}
	}
	return Vector2{X: v.X / length, Y: v.Y / length}
}