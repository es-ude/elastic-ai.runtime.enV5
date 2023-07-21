cmake_minimum_required(VERSION 3.13)
include(helpers.cmake)

add_library(CException INTERFACE)
target_sources(CException INTERFACE ${CMAKE_SOURCE_DIR}/extern/cexception/lib/CException.c)
target_include_directories(CException INTERFACE ${CMAKE_SOURCE_DIR}/extern/cexception/lib)

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

    include(extern/pico-sdk/pico_sdk_init.cmake)

    project(enV5 C CXX ASM)

    add_subdirectory(extern/elastic-ai.runtime.c)
    pico_sdk_init()
    add_subdirectory(extern/freeRTOS/)

    include_src()

    if (TARGET tinyusb_device)
        add_subdirectory(test/hardware)

        add_executable(demo src/Demo.c) # creates executable
        target_include_directories(demo PUBLIC ${CMAKE_SOURCE_DIR}/src)
        target_link_libraries(demo
                pico_stdlib
                http_lib
                freeRtosUtils
                network_lib
                espBroker_lib
                fpga_config_lib
                flash_lib
                middleware_lib
                hardware_spi
                spi_lib
                sensor_lib_pac193x
                sensor_lib_adxl345b
                env5_lib)
        make_to_output_file(demo)
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building Project because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()
