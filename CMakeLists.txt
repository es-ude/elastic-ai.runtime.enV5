cmake_minimum_required(VERSION 3.13)
include(helpers.cmake)

OPTION(DEVELOP "For best use developing with an IDE" OFF)
OPTION(UNIT_TEST "Builds Unit tests" OFF)
OPTION(HARDWARE_TEST "Builds hardware tests" OFF)

if ((UNIT_TEST AND DEVELOP) OR (UNIT_TEST AND DEVELOP) OR (DEVELOP AND HARDWARE_TEST))
    unset(DEVELOP CACHE)
    unset(UNIT_TEST CACHE)
    unset(HARDWARE_TEST CACHE)
    message(SEND_ERROR "MULTIPLE TARGETS DEFINED")
endif ()

if (UNIT_TEST)
    message(NOTICE "INFO: Building Unit tests")

    project(enV5 C CXX ASM)

    fetch_git_submodules()

    add_subdirectory(test/unit)
    add_subdirectory(extern/unity)
endif ()

if (NOT UNIT_TEST)
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    fetch_git_submodules()
    include(extern/pico-sdk/pico_sdk_init.cmake)

    project(enV5 C CXX ASM)

    pico_sdk_init()
    include_src()
    add_subdirectory(extern/freeRTOS/)
    SET(TARGET_GROUP extern)
    add_subdirectory(extern/elastic-ai.runtime.c)
endif ()

if (DEVELOP)
    message(NOTICE "INFO: Including everything for development")

    add_executable(main src/main.c)
    target_include_directories(main PUBLIC
            ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(main
            pico_stdlib
            esp_lib
            espBroker_lib
            FreeRTOSUtils
            gpio_interface
            i2c_interface
            network_lib
            uart_lib
            hardware_spi)
    pico_enable_stdio_usb(main 1)
    pico_enable_stdio_uart(main 0)

    add_subdirectory(extern/unity)
    add_subdirectory(test/hardware)
    add_subdirectory(test/unit)
endif ()

if (HARDWARE_TEST)
    message(NOTICE "INFO: Building hardware tests")

    if (TARGET tinyusb_device)
        add_subdirectory(test/hardware)
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building Project because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()

if (NOT DEVELOP AND NOT UNIT_TEST AND NOT HARDWARE_TEST)
    message(NOTICE "INFO: Building main executable")

    if (TARGET tinyusb_device)
        add_executable(main src/main.c) # creates executable
        target_include_directories(main PUBLIC
                ${CMAKE_SOURCE_DIR}/src)
        target_link_libraries(main
                pico_stdlib
                FreeRTOSUtils
                network_lib
                espBroker_lib
                hardware_spi
                spi)
        # enable usb output, disable uart output
        pico_enable_stdio_usb(main 1)
        pico_enable_stdio_uart(main 0)
        # create map/bin/hex/uf2 file etc.
        pico_add_uf2_output(main)
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building Project because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()

# Reset command line options
unset(DEVELOP CACHE)
unset(UNIT_TEST CACHE)
unset(HARDWARE_TEST CACHE)