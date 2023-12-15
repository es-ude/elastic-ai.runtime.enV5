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
    add_subdirectory(test)

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
        add_subdirectory(test)
        add_subdirectory(test/hardware)
    elseif (PICO_ON_DEVICE)
        message(WARNING "Not building Project, because TinyUSB submodule is not initialized in the SDK!")
    endif ()
endif ()
