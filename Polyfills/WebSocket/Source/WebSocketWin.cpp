#include "WebSocket.h"
#include <ppltasks.h>

#include <Windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>

#include <Babylon/JsRuntime.h>
#include <arcana/threading/task.h>
#include <arcana/threading/task_conversions.h>
#include <stringapiset.h>

#include <iostream>

// DATAWRITER
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml::Navigation;

namespace Babylon::Polyfills::Internal
{
    void WebSocket::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        static constexpr auto JS_WEB_SOCKET_CONSTRUCTOR_NAME = "WebSocket";

        Napi::Function func = DefineClass(
            env,
            JS_WEB_SOCKET_CONSTRUCTOR_NAME,
            {
                StaticValue("CONNECTING", Napi::Value::From(env, 0)),
                StaticValue("OPEN", Napi::Value::From(env, 1)),
                StaticValue("CLOSING", Napi::Value::From(env, 2)),
                StaticValue("CLOSED", Napi::Value::From(env, 3)),
                InstanceAccessor("binaryType", &WebSocket::GetBinaryType_, &WebSocket::SetBinaryType),
                InstanceAccessor("bufferedAmount", &WebSocket::GetBufferedAmount, nullptr),
                InstanceAccessor("protocol", &WebSocket::GetProtocol, nullptr),
                InstanceAccessor("readyState", &WebSocket::GetReadyState, nullptr),
                InstanceAccessor("URL", &WebSocket::GetURL, nullptr),
                InstanceAccessor("onopen", nullptr, &WebSocket::SetOnOpen),
                InstanceAccessor("onclose", nullptr, &WebSocket::SetOnClose),
                InstanceAccessor("onmessage", nullptr, &WebSocket::SetOnMessage),
                InstanceAccessor("onerror", nullptr, &WebSocket::SetOnError),
                //InstanceMethod("addEventListener", &WebSocket::AddEventListener),
                //InstanceMethod("removeEventListener", &WebSocket::RemoveEventListener),
                InstanceMethod("close", &WebSocket::Close),
                InstanceMethod("send", &WebSocket::Send),
            });

        if (env.Global().Get(JS_WEB_SOCKET_CONSTRUCTOR_NAME).IsUndefined())
        {
            env.Global().Set(JS_WEB_SOCKET_CONSTRUCTOR_NAME, func);
        }

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_WEB_SOCKET_CONSTRUCTOR_NAME, func);
    }

    WebSocket::WebSocket(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<WebSocket>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
    {
        switch (info.Length())
        {
            case 2:
                m_protocol = info[1].As<Napi::String>();
            case 1:
                m_url = info[0].As<Napi::String>();
        }

        m_webSocket.Control().MessageType(Windows::Networking::Sockets::SocketMessageType::Utf8);

        m_messageReceivedEventToken = m_webSocket.MessageReceived({this, &WebSocket::OnMessageReceived});
        m_closedEventToken = m_webSocket.Closed({this, &WebSocket::OnWebSocketClosed});

        try
        {
            // INITIALIZE SOCKET
            winrt::hstring hURL = winrt::to_hstring(m_url);
            m_readyState = ReadyState::Connecting;

            arcana::create_task<std::exception_ptr>(m_webSocket.ConnectAsync(Uri{hURL}))
                .then(m_runtimeScheduler, arcana::cancellation::none(), [this]()
                    {
                        m_readyState = ReadyState::Open;
                        m_onopen.Call({});
                    });
        }
        catch (winrt::hresult_error const& ex)
        {
            Windows::Web::WebErrorStatus webErrorStatus{Windows::Networking::Sockets::WebSocketError::GetStatus(ex.to_abi())};
            printf("%d", webErrorStatus);
            HRESULT result = ex.code();
            printf("%i", result);
            //printf("%s", ex.message().c_str());
            // Add additional code here to handle exceptions.

            m_runtimeScheduler([this]()
                { 
                    Napi::Object errorEvent = Napi::Object::New(Env());

                    m_onerror.Call({errorEvent}); 
                });
        }
    }

    void WebSocket::OnMessageReceived(winrt::Windows::Networking::Sockets::MessageWebSocket const& /* sender */,
        winrt::Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs const& args)
    {
        if (m_readyState == ReadyState::Closed ||
            m_readyState == ReadyState::Closing)
            return;

        try
        {
            DataReader dataReader{args.GetDataReader()};

            dataReader.UnicodeEncoding(Windows::Storage::Streams::UnicodeEncoding::Utf8);
            auto message = winrt::to_string(dataReader.ReadString(dataReader.UnconsumedBufferLength()));
            m_runtimeScheduler([this, message = std::move(message)]()
                {
                    //Napi::Object messageEvent = Napi::Object::New(Env());
                    Napi::Object messageEvent = Env().Global().Get("MessageEvent").As<Napi::Function>().New({});
                    messageEvent.Set("data", message);
                    m_onmessage.Call({messageEvent});
                });
        }
        catch (winrt::hresult_error const& ex)
        {
            ex;

            m_runtimeScheduler([this]()
                {
                    Napi::Object errorEvent = Napi::Object::New(Env());
                    //errorEvent.Set("data", message);

                    m_onerror.Call({errorEvent});
                });
        }

    }

    void WebSocket::Close(const Napi::CallbackInfo&)
    {
        if (m_readyState == ReadyState::Closed ||
            m_readyState == ReadyState::Closing)
            return;

        m_readyState = ReadyState::Closing;

        m_webSocket.Close();

        m_readyState = ReadyState::Closed;

        m_runtimeScheduler([this]()
            {
                Napi::Object closeEvent = Napi::Object::New(Env());
                //errorEvent.Set("data", message);
                //Napi::Object contextStub = Env().Global().Get("CloseEvent").As<Napi::Function>().New({});
                closeEvent.Set("code", 1000);
                closeEvent.Set("reason", "CLOSED_EVENT NATIVE TEST CLOSE");
                closeEvent.Set("wasClean", true);

                m_onclose.Call({closeEvent});
            });
    }

    void WebSocket::OnWebSocketClosed(Windows::Networking::Sockets::IWebSocket const& /* sender */, Windows::Networking::Sockets::WebSocketClosedEventArgs const& args)
    {
        std::cout << L"WebSocket_Closed; Code: " << args.Code() << ", Reason: \"" << args.Reason().c_str() << "\"" << std::endl;
    }

    void WebSocket::Send(const Napi::CallbackInfo& info)
    {
        if (m_readyState == ReadyState::Closed ||
            m_readyState == ReadyState::Closing ||
            m_readyState == ReadyState::Connecting)
            return;

        try
        {
            std::string message = info[0].As<Napi::String>();

            // SEND MESSAGE
            DataWriter dataWriter{m_webSocket.OutputStream()};
            dataWriter.WriteString(winrt::to_hstring(message));

            arcana::create_task<std::exception_ptr>(dataWriter.StoreAsync())
                .then(m_runtimeScheduler, arcana::cancellation::none(), [this, dataWriter](int dwOperation)
                    {
                        // CLEANUP
                        uint32_t result = dwOperation;
                        printf("%d", result);
                        dataWriter.DetachStream();
                    });
        }
        catch (winrt::hresult_error const& ex)
        {
            Windows::Web::WebErrorStatus webErrorStatus{Windows::Networking::Sockets::WebSocketError::GetStatus(ex.to_abi())};
            printf("%d", webErrorStatus);
            HRESULT result = ex.code();
            printf("%i", result);
            //printf("%s", ex.message().c_str());
            // Add additional code here to handle exceptions.

            m_runtimeScheduler([this]()
                {
                    Napi::Object errorEvent = Napi::Object::New(Env());
                    //errorEvent.Set("data", message);

                    m_onerror.Call({errorEvent});
                });
        }
    }
}
