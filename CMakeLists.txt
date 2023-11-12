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

    # include external libraries (git submodules)
    include_cexception()
    add_subdirectory(extern/unity)
    add_subdirectory(extern/elastic-ai.runtime.c)
    include(CTest)

    # include required standard libs for testing
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

    # include unit tests
    add_subdirectory(test/unit)
else ()
    message(NOTICE "INFO: Building for enV5 hardware")

    # include external libraries (git submodules)
    include(extern/pico-sdk/pico_sdk_init.cmake)
    add_subdirectory(extern/freeRTOS/Kernel/portable/ThirdParty/GCC/RP2040 FREERTOS_KERNEL)
    add_subdirectory(extern/elastic-ai.runtime.c)

    project(enV5 C CXX ASM)

    pico_sdk_init()

    if (TARGET tinyusb_device)
        # include our own libraries
        include_cexception()
        add_subdirectory(src/common)
        add_subdirectory(src/hal)
        add_subdirectory(src/rtos)
        add_subdirectory(src/network)
        add_subdirectory(src/flash)
        add_subdirectory(src/fpga)
        add_subdirectory(src/sensor)

        # load hardware tests
        add_subdirectory(test/hardware)
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building Project because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()
