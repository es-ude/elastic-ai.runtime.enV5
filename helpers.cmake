include(FetchContent)

function(add_cexception)
    FetchContent_Declare(
            cexception
            GIT_REPOSITORY https://github.com/ThrowTheSwitch/CException.git
            GIT_TAG v1.3.3
    )
    FetchContent_Populate(cexception)

    add_library(CException INTERFACE)
    target_sources(CException INTERFACE ${cexception_SOURCE_DIR}/lib/CException.c)
    target_include_directories(CException INTERFACE ${cexception_SOURCE_DIR}/lib)
endfunction()

function(add_unity)
    FetchContent_Declare(
            unity
            GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
            GIT_TAG v2.5.2
    )
    FetchContent_Populate(unity)

    add_subdirectory(${unity_SOURCE_DIR})
endfunction()

function(add_runtime_c)
    FetchContent_Declare(
            elastic_ai_runtime_c
            GIT_REPOSITORY https://github.com/es-ude/elastic-ai.runtime.c.git
            GIT_TAG v2.5.0
    )
    FetchContent_Populate(elastic_ai_runtime_c)

    add_subdirectory(${elastic_ai_runtime_c_SOURCE_DIR})
endfunction()

function(add_freertos_kernel)
    FetchContent_Declare(
            freertos_kernel
            GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
            GIT_TAG V11.0.1
    )
    FetchContent_Populate(freertos_kernel)

    add_subdirectory(${freertos_kernel_SOURCE_DIR}/portable/ThirdParty/GCC/RP2040 FREERTOS_KERNEL)
endfunction()

function(add_rp2040_hal)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/hal)
endfunction()

function(add_basic_functionality)
    add_cexception()
    add_runtime_c()
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/common)
    add_freertos_kernel()
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/rtos)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/network)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/flash)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/fpga)
endfunction()

function(add_sensor_libraries)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/sensor)
endfunction()

function(create_enV5_executable target)
    # enable usb output
    pico_enable_stdio_usb(${target} 1)
    # disable uart output
    pico_enable_stdio_uart(${target} 0)
    # create map/bin/hex/uf2 file etc.
    pico_add_uf2_output(${target})
    # copy u2f files after build to out directory
    file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/${relative_path}/${target}.uf2
            ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/${relative_path}/${target}.uf2)
endfunction()

function(add_dependency_graph)
    file(WRITE "${CMAKE_BINARY_DIR}/CMakeGraphVizOptions.cmake" "set(GRAPHVIZ_GENERATE_PER_TARGET FALSE)
            set(GRAPHVIZ_GENERATE_DEPENDERS FALSE)
            set(GRAPHVIZ_IGNORE_TARGETS
            \"Pioasm;ELF2UF2;cyw43.*;m;stub_*;cybt.*;cmsis.*;boot_.*;bs2_default.*;.*marker.*;hardware-test_*\")")
    file(WRITE "${CMAKE_BINARY_DIR}/test.sh" "
            perl -i -0pe 's/digraph[^}]*/START_REPLACEMENT/' enV5.dot
            perl -pi -e 's/.*\"[^\\\/\\\/\\n]*//g' enV5.dot
            perl -pi -e 's/\\\/\\\/ //g' enV5.dot
            perl -pi -e 's/[^ ]*(pico|hardware_|tinyusb)[^ \\n]*/pico_stdlib/g' enV5.dot
            perl -pi -e 's/FreeRTOS-Kernel[^ \\n]*/FreeRTOS_Kernel/g' enV5.dot
            perl -pi -e 's/\\\./_/g' enV5.dot
            perl -pi -e 's/->/ARROW_REPLECEMNT/g' enV5.dot
            perl -pi -e 's/-/_/g' enV5.dot
            perl -pi -e 's/ARROW_REPLECEMNT/->/g' enV5.dot
            perl -pi -e 's/node \\\[/digraph \"enV5\" {node [/g' enV5.dot
            perl -pi -e 's/pico_stdlib -> pico_stdlib//' enV5.dot
            perl -pi -e 's/FreeRTOS_Kernel -> FreeRTOS_Kernel//' enV5.dot
            perl -pi -e 's/START_REPLACEMENT}/strict digraph \"enV5\" {/' enV5.dot"
    )
    add_custom_target(graphviz ALL
            COMMAND ${CMAKE_COMMAND} "--graphviz=enV5.dot" .
            COMMAND bash test.sh
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/documentation/pics"
            COMMAND dot -T png enV5.dot -o "${CMAKE_SOURCE_DIR}/documentation/pics/dependencies.png"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endfunction()
