
add_library(EnV5HwConfiguration INTERFACE)
add_library(EnV5HwConfiguration__hdrs INTERFACE)
target_link_libraries(EnV5HwConfiguration INTERFACE EnV5HwConfiguration__hdrs)

set(REVISION_FILEPATH ${REVISION} CACHE FILEPATH "Helper variable needed for casting" FORCE)

if(NOT DEFINED REVISION)
    message(FATAL_ERROR "Missing hardware configuration")
elseif(${REVISION} STREQUAL "1")
    target_include_directories(EnV5HwConfiguration__hdrs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/revision01)
elseif (${REVISION} STREQUAL "2")
    target_include_directories(EnV5HwConfiguration__hdrs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/revision02)
elseif (IS_ABSOLUTE ${REVISION_FILEPATH})
    if(${REVISION_FILEPATH} MATCHES "\\/EnV5HwConfiguration\\.h$")
        target_include_directories(EnV5HwConfiguration__hdrs INTERFACE REVISION_FILEPATH)
    else ()
        message(FATAL_ERROR "Provided file needs to be named EnV5HwConfiguration.h: ${REVISION_FILEPATH}")
    endif()
else()
    message(FATAL_ERROR "Error in hardware configuration: ${REVISION}")
endif ()

