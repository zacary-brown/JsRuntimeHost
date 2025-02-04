#include <jni.h>
#include <Android/log.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

#include <Shared/Shared.h>

extern "C" JNIEXPORT jint JNICALL
Java_com_jsruntimehost_unittests_Native_javaScriptTests(JNIEnv* env, jclass clazz, jobject context) {
    JavaVM* javaVM{};
    if (env->GetJavaVM(&javaVM) != JNI_OK)
    {
        throw std::runtime_error{"Failed to get Java VM"};
    }

    jclass webSocketClass{env->FindClass("com/jsruntimehost/unittests/WebSocket")};
    java::websocket::WebSocketClient::InitializeJavaWebSocketClass(webSocketClass, env);
    android::global::Initialize(javaVM, context);

    auto consoleCallback = [](const char* message, Babylon::Polyfills::Console::LogLevel level)
    {
        switch (level)
        {
        case Babylon::Polyfills::Console::LogLevel::Log:
            __android_log_write(ANDROID_LOG_INFO, "JsRuntimeHost", message);
            break;
        case Babylon::Polyfills::Console::LogLevel::Warn:
            __android_log_write(ANDROID_LOG_WARN, "JsRuntimeHost", message);
            break;
        case Babylon::Polyfills::Console::LogLevel::Error:
            __android_log_write(ANDROID_LOG_ERROR, "JsRuntimeHost", message);
            break;
        }
    };

    auto testResult = RunTests(consoleCallback);
    java::websocket::WebSocketClient::DestructJavaWebSocketClass(env);
    return testResult;
}
