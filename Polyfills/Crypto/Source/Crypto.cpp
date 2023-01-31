#define _CRT_RAND_S
#include <stdlib.h>
#include "Crypto.h"
#include <Babylon/Polyfills/Crypto.h>
#include <Babylon/JsRuntime.h>

namespace Babylon::Polyfills::Internal
{
    void Crypto::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};
        static constexpr auto JS_CRYPTO_CONSTRUCTOR_NAME = "crypto";
        Napi::Function func = DefineClass(
            env,
            JS_CRYPTO_CONSTRUCTOR_NAME,
            {
                InstanceMethod("getRandomValues", &Crypto::GetRandomValues),
            });

        auto myCrypto = func.New({});
        if (env.Global().Get(JS_CRYPTO_CONSTRUCTOR_NAME).IsUndefined())
        {
            env.Global().Set(JS_CRYPTO_CONSTRUCTOR_NAME, myCrypto);
        }

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CRYPTO_CONSTRUCTOR_NAME, myCrypto);
    }
    
    uint8_t GetRandomValue() {
        unsigned int number;
        errno_t err;
        err = rand_s(&number);
        if (err != 0)
        {
            printf_s("The rand_s function failed!\n");
        }
        return (uint8_t)((double)number / ((double) UINT_MAX + 1) * 256.0);

    }

    Napi::Value Crypto::GetRandomValues(const Napi::CallbackInfo& info)
    {
        auto env{info.Env()};
        auto myBuf = Napi::Uint8Array::New(env, 16);

        for (int i = 0; i <= 16; i++)
        {
            myBuf[i] = GetRandomValue();
        }

       return Napi::Value::From(Env(), myBuf);
    }

    Crypto::Crypto(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Crypto>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
    {
    }
}


namespace Babylon::Polyfills::Crypto
{
    void Initialize(Napi::Env env)
    {
        Internal::Crypto::Initialize(env);
    }
}
