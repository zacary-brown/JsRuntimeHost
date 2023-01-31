#pragma once

#include <napi/napi.h>
#include <Babylon/JsRuntimeScheduler.h>

namespace Babylon::Polyfills::Internal
{
    class Crypto final : public Napi::ObjectWrap<Crypto>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit Crypto(const Napi::CallbackInfo& info);

    private:
        Napi::Value GetRandomValues(const Napi::CallbackInfo& info);

        JsRuntimeScheduler m_runtimeScheduler;
    };
}
