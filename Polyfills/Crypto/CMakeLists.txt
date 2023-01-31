set(SOURCES 
    "Include/Babylon/Polyfills/Crypto.h"
    "Source/Crypto.cpp"
    "Source/Crypto.h")

add_library(Crypto ${SOURCES})
#warnings_as_errors(Crypto)

target_include_directories(Crypto PUBLIC "Include")

target_link_libraries(Crypto
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE UrlLib)

set_property(TARGET Crypto PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
