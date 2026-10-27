package system.ui

import system.math.Vector2
import system.math.MathUtils
import system.framework.GraphicsContext

open class CustomButton(
    var position: Vector2,
    var size: Vector2,
    var text: String
) {
    // --- WŁAŚCIWOŚCI KONFIGURACYJNE (CUSTOMIZACJA) ---
    var backgroundColor: Int = 0x333333       // Domyślny kolor tła (szary)
    var hoverColor: Int = 0x555555          // Kolor po najechaniu myszką
    var pressedColor: Int = 0x222222        // Kolor po kliknięciu
    var textColor: Int = 0xFFFFFF           // Kolor tekstu (biały)
    var borderColor: Int = 0x888888         // Kolor ramki
    var borderWidth: Int = 1

    // Stany wewnętrzne widgetu
    private var isHovered: Boolean = false
    private var isPressed: Boolean = false

    // Obsługa zdarzenia kliknięcia (lambda, którą można podmienić)
    var onClick: () -> Unit = {
        println("Przycisk '$text' został kliknięty!")
    }

    // Sprawdzanie interakcji z myszką (np. wywoływane przez nasz EventBus)
    fun updateMouseState(mouseX: Float, mouseY: Float, isMouseDown: Boolean) {
        isHovered = MathUtils.pointInRect(mouseX, mouseY, position.x, position.y, size.x, size.y)
        
        if (isHovered && isMouseDown) {
            if (!isPressed) {
                isPressed = true
                onClick() // Wywołanie akcji
            }
        } else {
            isPressed = false
        }
    }

    // Metoda odpowiedzialna za renderowanie widgetu
    fun draw() {
        // Dobór koloru tła w zależności od stanu interakcji
        val currentColor = when {
            isPressed -> pressedColor
            isHovered -> hoverColor
            else -> backgroundColor
        }

        // Rysowanie tła przycisku za pomocą naszego GraphicsContext
        GraphicsContext.drawRect(
            position.x.toInt(),
            position.y.toInt(),
            size.x.toInt(),
            size.y.toInt(),
            currentColor
        )

        // Rysowanie tekstu wewnątrz przycisku (z prostym marginesem)
        val textOffsetX = position.x.toInt() + 15
        val textOffsetY = position.y.toInt() + (size.y / 2).toInt() + 5
        GraphicsContext.drawString(text, textOffsetX, textOffsetY, textColor)
    }
}