#pragma once

#include <Babylon/JsRuntimeScheduler.h>

#include <napi/napi.h>
#include <../../Performance/Source/Performance.h>

namespace Babylon::Polyfills::Internal
{
    class Performance final : public Napi::ObjectWrap<Performance>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit Performance(const Napi::CallbackInfo& info);

    private:
        
        Napi::Value Now(const Napi::CallbackInfo& info);
        void Mark(const Napi::CallbackInfo& info);
        void Measure(const Napi::CallbackInfo& info);
        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);

        JsRuntimeScheduler m_runtimeScheduler;
        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs;

        std::chrono::steady_clock::time_point m_startTime;
    };
}