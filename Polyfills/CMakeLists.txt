if(JSRUNTIMEHOST_POLYFILL_ABORTCONTROLLER)
    add_subdirectory(AbortController)
endif()

if(JSRUNTIMEHOST_POLYFILL_ABORTSIGNAL)
    add_subdirectory(AbortSignal)
endif()

if(JSRUNTIMEHOST_POLYFILL_CONSOLE)
    add_subdirectory(Console)
endif()

if(JSRUNTIMEHOST_POLYFILL_CRYPTO)
    add_subdirectory(Crypto)
endif()

if(JSRUNTIMEHOST_POLYFILL_PERFORMANCE)
    add_subdirectory(Performance)
endif()

if(JSRUNTIMEHOST_POLYFILL_SCHEDULING)
    add_subdirectory(Scheduling)
endif()

if(JSRUNTIMEHOST_POLYFILL_URL)
    add_subdirectory(URL)
endif()

if(JSRUNTIMEHOST_POLYFILL_URLSEARCHPARAMS)
    add_subdirectory(URLSearchParams)
endif()

if(JSRUNTIMEHOST_POLYFILL_WEBSOCKET)
    add_subdirectory(WebSocket)
endif()

if(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    add_subdirectory(XMLHttpRequest)
endif()

add_subdirectory(Crypto)
add_subdirectory(AbortController)
add_subdirectory(AbortSignal)
add_subdirectory(Performance)
add_subdirectory(URL)
add_subdirectory(URLSearchParams)