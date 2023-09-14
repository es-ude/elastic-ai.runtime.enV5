cmake_minimum_required(VERSION 3.13)
include(helpers.cmake)

if (DEBUG_OUTPUT)
    add_definitions(-DDEBUG_MODE)
endif ()

if (UNIT_TEST)
    message(NOTICE "INFO: Building Unit tests")

    project(enV5 C CXX ASM)

    include(CTest)

    add_subdirectory(extern/unity)
    add_subdirectory(test/unit)
else ()
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

    # add submodule libraries
    include(extern/pico-sdk/pico_sdk_init.cmake)
    add_subdirectory(extern/freeRTOS/Kernel/portable/ThirdParty/GCC/RP2040 FREERTOS_KERNEL)
    add_subdirectory(extern/elastic-ai.runtime.c)

    project(enV5 C CXX ASM)

    pico_sdk_init()

    if (TARGET tinyusb_device)

        include_src()

        add_subdirectory(test/hardware)

        add_executable(wrist src/Wrist.c) # creates executable
        target_include_directories(wrist PUBLIC ${CMAKE_SOURCE_DIR}/src)
        target_link_libraries(wrist
                pico_stdlib
                http_lib
                freeRtosUtils
                network_lib
                espBroker_lib
                fpga_config_lib
                flash_lib
                middleware_lib
                spi_lib
                sensor_lib_adxl345b
                env5_lib)
        make_to_output_file(wrist)
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building Project because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()
