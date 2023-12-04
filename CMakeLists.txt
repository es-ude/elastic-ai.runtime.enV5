cmake_minimum_required(VERSION 3.13)
include(helpers.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# enable debug prints
if (DEBUG_OUTPUT)
    add_definitions(-DDEBUG_MODE)
endif ()

if (UNIT_TEST)
    message(NOTICE "INFO: Building for local machine")

    project(enV5 C CXX ASM)

    # enable test execution via CMake
    include(CTest)

    # include unit-test framework
    add_unity()

    # include required standard libs for testing
    add_cexception()
    add_subdirectory(extern/elastic-ai.runtime.c)
    add_subdirectory(src/common)
    add_subdirectory(src/network/atCommands)

    # include dummies for local machine
    add_subdirectory(test/unit/dummies)

    # include our own libraries to test
    add_subdirectory(src/sensor/sht3x)
    add_subdirectory(src/sensor/adxl345b)
    add_subdirectory(src/sensor/pac193x)
    add_subdirectory(src/network/wifi)
    add_subdirectory(src/network/http)
    add_subdirectory(src/network/broker)
    add_subdirectory(src/fpga/fpgaConfigurationHandler)

    # include unit-tests
    add_subdirectory(test/unit)
else ()
    message(NOTICE "INFO: Building for enV5 hardware (Type: ${CMAKE_BUILD_TYPE})")

    # include pico-sdk
    include(extern/pico-sdk/pico_sdk_init.cmake)

    project(enV5 C CXX ASM)

    pico_sdk_init()

    if (TARGET tinyusb_device)
        # include required libraries
        add_basic_functionality()
        add_rp2040_hal()
        add_sensor_libraries()

        # include hardware tests
        add_subdirectory(test/hardware)
    elseif (PICO_ON_DEVICE)
        message(WARNING "Not building Project, because TinyUSB submodule is not initialized in the SDK!")
    endif ()

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
            COMMAND dot -T png enV5.dot -o "${CMAKE_SOURCE_DIR}/documentation/pics/dependencies.png"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

endif ()
