include(FetchContent)


# include pico-sdk
macro(add_pico_sdk tag)
    set(PICO_SDK_FETCH_FROM_GIT on)
    set(PICO_SDK_FETCH_FROM_GIT_TAG ${tag})
    set(PICOTOOL_FETCH_FROM_GIT_PATH ${CMAKE_SOURCE_DIR}/picotool)
    include(pico_sdk_import.cmake)
endmacro()


function(add_cexception)
    FetchContent_Declare(
            cexception
            GIT_REPOSITORY https://github.com/ThrowTheSwitch/CException.git
            GIT_TAG v1.3.3
    )
    FetchContent_Populate(cexception)
    add_library(CException ${cexception_SOURCE_DIR}/lib/CException.c)
    add_library(CException__hdrs INTERFACE)
    target_include_directories(CException__hdrs INTERFACE ${cexception_SOURCE_DIR}/lib/)
    target_link_libraries(CException PUBLIC CException__hdrs)
endfunction()

function(add_unity)
FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.5.2
        OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(unity)
    find_package(unity)
endfunction()

function(add_runtime_c)
    FetchContent_Declare(
            elastic_ai_runtime_c
            GIT_REPOSITORY https://github.com/es-ude/elastic-ai.runtime.c.git
            GIT_TAG v2.7.2
    )
    FetchContent_MakeAvailable(elastic_ai_runtime_c)
endfunction()

function(add_freertos)
FetchContent_Declare(
        freertos_kernel
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG V11.0.1
)
FetchContent_Populate(freertos_kernel)
add_subdirectory(${freertos_kernel_SOURCE_DIR}/portable/ThirdParty/GCC/RP2040 FREERTOS_KERNEL)
endfunction()

