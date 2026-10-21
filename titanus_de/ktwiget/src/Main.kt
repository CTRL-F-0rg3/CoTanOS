import system.math.Vector2
import system.ui.CustomButton

fun main() {
    println("[UI] Inicjalizacja konfigurowalnych widgetów w Kotlinie...")

    // 1. Tworzymy przycisk za pomocą naszych wektorów 2D
    val submitButton = CustomButton(
        position = Vector2(100f, 50f),
        size = Vector2(200f, 45f),
        text = "Zatwierdź system"
    ).apply {
        // --- CUSTOMIZACJA WYGLĄDU ---
        backgroundColor = 0x0055CC // Niebieski tła
        hoverColor = 0x0077FF      // Jasny niebieski po najechaniu
        pressedColor = 0x003388    // Ciemny niebieski po kliknięciu
        
        // Własna akcja po kliknięciu przycisku
        onClick = {
            println("-> Sukces! Wykonano akcję z przycisku konfigurowalnego.")
        }
    }

    // 2. Symulujemy ruch myszy i kliknięcie nad przyciskiem
    println("\n[Test] Najazd myszką i kliknięcie w przycisk...")
    submitButton.updateMouseState(120f, 60f, true) // Pozycja X=120, Y=60 mieści się w przycisku

    // 3. Renderowanie przycisku na ekranie
    submitButton.draw()
}