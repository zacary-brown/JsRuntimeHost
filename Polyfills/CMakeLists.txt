if(JSRUNTIMEHOST_POLYFILL_CONSOLE)
    add_subdirectory(Console)
endif()

if(JSRUNTIMEHOST_POLYFILL_SCHEDULING)
    add_subdirectory(Scheduling)
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