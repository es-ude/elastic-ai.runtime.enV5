function(add_cexception)
    add_library(CException INTERFACE)
    target_sources(CException INTERFACE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/extern/cexception/lib/CException.c)
    target_include_directories(CException INTERFACE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/extern/cexception/lib)
endfunction()

function(add_unity)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/extern/unity)
endfunction()

function(add_rp2040_hal)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/hal)
endfunction()

function(add_basic_functionality)
    add_cexception()
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/extern/elastic-ai.runtime.c)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/common)
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
    file(WRITE "${CMAKE_BINARY_DIR}/CMakeGraphVizOptions.cmake" "set(GRAPHVIZ_IGNORE_TARGETS \"Pioasm;ELF2UF2;cyw43.*;m;cybt.*;cmsis.*;boot_.*;bs2_default.*;.*marker.*;hardware-test_*\")")
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
