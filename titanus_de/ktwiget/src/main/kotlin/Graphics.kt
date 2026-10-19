package main.kotlin
package dev.nativegui.graphics

class GraphicsBackend {

    companion object {
        init {
            System.loadLibrary("nativegui")
        }
    }

    fun beginFrame(): Boolean =
        nativeBeginFrame() == 0

    fun drawRect(
        x: Int,
        y: Int,
        width: Int,
        height: Int,
        argb: Int
    ): Boolean {
        require(width > 0 && height > 0) {
            "Rectangle dimensions must be positive"
        }

        return nativeDrawRect(
            x, y, width, height, argb
        ) == 0
    }

    fun endFrame(): Boolean =
        nativeEndFrame() == 0

    private external fun nativeBeginFrame(): Int

    private external fun nativeDrawRect(
        x: Int,
        y: Int,
        width: Int,
        height: Int,
        argb: Int
    ): Int

    private external fun nativeEndFrame(): Int
}