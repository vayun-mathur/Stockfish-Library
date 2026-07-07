package com.vayunmathur.stockfish

/**
 * Thin Kotlin wrapper around the native Stockfish UCI engine (`libstockfish.so`).
 *
 * Owns the JNI surface and native library loading. Callers drive the engine via
 * plain UCI commands and receive engine output through the [init] callback.
 */
object Stockfish {

    interface OutputCallback {
        fun onOutput(line: String)
    }

    private external fun nativeInit()
    private external fun nativeSendCommand(command: String)
    private external fun nativeSetOutputCallback(callback: OutputCallback)

    private var loaded = false

    /**
     * Loads the native library (once), registers [onOutput] as the engine's
     * output sink, and initializes the engine.
     */
    fun init(onOutput: (String) -> Unit) {
        if (!loaded) {
            System.loadLibrary("stockfish")
            loaded = true
        }
        nativeSetOutputCallback(object : OutputCallback {
            override fun onOutput(line: String) = onOutput(line)
        })
        nativeInit()
    }

    /** Sends a raw UCI command to the engine. */
    fun sendCommand(command: String) {
        nativeSendCommand(command)
    }
}
