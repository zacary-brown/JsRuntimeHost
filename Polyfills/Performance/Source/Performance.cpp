#include "Performance.h"
#include <Babylon/Polyfills/Performance.h>

namespace Babylon::Polyfills::Internal
{
    void Performance::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        static constexpr auto JS_PERFORMANCE_CONSTRUCTOR_NAME = "Performance";

        Napi::Function func = DefineClass(
            env,
            JS_PERFORMANCE_CONSTRUCTOR_NAME,
            {
                InstanceMethod("mark", &Performance::Mark),
                InstanceMethod("measure", &Performance::Measure),
                InstanceMethod("now", &Performance::Now),
                InstanceMethod("addEventListener", &Performance::AddEventListener),
                InstanceMethod("removeEventListener", &Performance::RemoveEventListener),
            });

        if (env.Global().Get(JS_PERFORMANCE_CONSTRUCTOR_NAME).IsUndefined())
        {
            env.Global().Set(JS_PERFORMANCE_CONSTRUCTOR_NAME, func);
        }

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_PERFORMANCE_CONSTRUCTOR_NAME, func);
    }

    void Performance::Mark(const Napi::CallbackInfo&) {
        return;
    }

    void Performance::Measure(const Napi::CallbackInfo&)
    {
        return;
    }

    Napi::Value Performance::Now(const Napi::CallbackInfo& )
    {
        return Napi::Value::From(Env(), std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_startTime).count());
    }

    Performance::Performance(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Performance>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
        , m_startTime(std::chrono::high_resolution_clock::now())
    {
        if (!info.Length())
            return;
    }

    void Performance::AddEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function eventHandler = info[1].As<Napi::Function>();

        const auto& eventHandlerRefs = m_eventHandlerRefs[eventType];
        for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
        {
            if (it->Value() == eventHandler)
            {
                throw Napi::Error::New(info.Env(), "Cannot add the same event handler twice");
            }
        }

        m_eventHandlerRefs[eventType].push_back(Napi::Persistent(eventHandler));
    }

    void Performance::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function eventHandler = info[1].As<Napi::Function>();
        auto itType = m_eventHandlerRefs.find(eventType);
        if (itType != m_eventHandlerRefs.end())
        {
            auto& eventHandlerRefs = itType->second;
            for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
            {
                if (it->Value() == eventHandler)
                {
                    eventHandlerRefs.erase(it);
                    break;
                }
            }
        }
    }
}

namespace Babylon::Polyfills::Performance
{
    void Initialize(Napi::Env env)
    {
        Internal::Performance::Initialize(env);
    }
}