#include <jni.h>
#include <string>
#include "libstockfish.h"

static JavaVM* g_jvm = nullptr;
static jobject g_callback_obj = nullptr;
static jmethodID g_callback_method = nullptr;

static void jni_output_callback(const char* line) {
    if (!g_jvm || !g_callback_obj) return;

    JNIEnv* env = nullptr;
    bool attached = false;
    if (g_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_EDETACHED) {
        g_jvm->AttachCurrentThread(&env, nullptr);
        attached = true;
    }

    jstring jline = env->NewStringUTF(line);
    env->CallVoidMethod(g_callback_obj, g_callback_method, jline);
    env->DeleteLocalRef(jline);

    if (attached)
        g_jvm->DetachCurrentThread();
}

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    g_jvm = vm;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_vayunmathur_stockfish_Stockfish_nativeInit(JNIEnv*, jobject) {
    sf_init();
}

JNIEXPORT void JNICALL
Java_com_vayunmathur_stockfish_Stockfish_nativeSendCommand(JNIEnv* env, jobject, jstring command) {
    const char* cmd = env->GetStringUTFChars(command, nullptr);
    sf_command(cmd);
    env->ReleaseStringUTFChars(command, cmd);
}

JNIEXPORT void JNICALL
Java_com_vayunmathur_stockfish_Stockfish_nativeSetOutputCallback(JNIEnv* env, jobject, jobject callback) {
    if (g_callback_obj)
        env->DeleteGlobalRef(g_callback_obj);

    g_callback_obj = env->NewGlobalRef(callback);
    jclass cls = env->GetObjectClass(callback);
    g_callback_method = env->GetMethodID(cls, "onOutput", "(Ljava/lang/String;)V");

    sf_set_output_callback(jni_output_callback);
}

}
