cmake_minimum_required(VERSION 3.20...3.24)
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
    add_runtime_c()
    add_subdirectory(src/common)
    add_subdirectory(src/network/atCommands)
    add_subdirectory(src/network/config)

    # include dummies for local machine
    add_subdirectory(test/unit/dummies)

    # include our own libraries to test
    add_subdirectory(src/hal/enV5HwConfiguration)
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
    set(PICO_SDK_FETCH_FROM_GIT on)
    set(PICO_SDK_FETCH_FROM_GIT_TAG "2.0.0")
    set(PICO_BOARD none)
    set(PICO_PLATFORM rp2040)
    set(PICOTOOL_FETCH_FROM_GIT_PATH ${CMAKE_SOURCE_DIR}/picotool)
    include(pico_sdk_import.cmake)

    project(enV5 C CXX ASM)

    pico_sdk_init()

    if (TARGET tinyusb_device)
        # include required libraries
        add_basic_functionality()
        add_rp2040_hal()
        add_sensor_libraries()

        # include hardware tests
        add_subdirectory(test/hardware)

        add_dependency_graph()
    elseif (PICO_ON_DEVICE)
        message(WARNING "Not building Project, because TinyUSB submodule is not initialized in the SDK!")
    endif ()
endif ()
