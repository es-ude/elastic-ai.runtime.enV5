add_library(enV5_hw_configuration INTERFACE
        revision01/EnV5HwConfiguration.h
        revision02/EnV5HwConfiguration.h)

set(REVISION_FILEPATH ${REVISION} CACHE FILEPATH "Helper variable needed for casting" FORCE)

if(NOT DEFINED REVISION)
    message(FATAL_ERROR "You need to specify a hw configuration by setting REVISION to '1' or '2'")
elseif(${REVISION} STREQUAL "1")
    target_include_directories(enV5_hw_configuration INTERFACE ${CMAKE_CURRENT_LIST_DIR}/revision01)
elseif (${REVISION} STREQUAL "2")
    target_include_directories(enV5_hw_configuration INTERFACE ${CMAKE_CURRENT_LIST_DIR}/revision02)
elseif (IS_ABSOLUTE ${REVISION_FILEPATH})
    if(${REVISION_FILEPATH} MATCHES "\\/EnV5HwConfiguration\\.h$")
        target_include_directories(enV5_hw_configuration INTERFACE REVISION_FILEPATH)
    else ()
        message(FATAL_ERROR "Provided file needs to be named EnV5HwConfiguration.h: ${REVISION_FILEPATH}")
    endif()
else()
    message(FATAL_ERROR "Error in hardware configuration: ${REVISION}")
endif ()
