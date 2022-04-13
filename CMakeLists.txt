cmake_minimum_required(VERSION 3.13)
include(helpers.cmake)

OPTION(DEVELOP "For best use developing with an IDE" OFF)
OPTION(UNIT_TEST "Builds Unit tests" OFF)
OPTION(HARDWARE_TEST "Builds hardware tests" OFF)

if (DEVELOP)
    if (UNIT_TEST OR HARDWARE_TEST)
        message(WARNING "MULTIPLE TARGETS DEFINED")
    endif ()
    message(NOTICE "INFO: Including everything for development")

    set(PICO_SDK_FETCH_FROM_GIT 1)
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    include(extern/pico-sdk/pico_sdk_import.cmake)
    project(ENv5)
    enable_language(C CXX ASM)
    fetch_git_submodules()
    pico_sdk_init()

    add_subdirectory(extern/freeRTOS/)
    SET(TARGET_GROUP extern)
    add_subdirectory(extern/elastic-ai.runtime.c)
    add_subdirectory(extern/unity)

    add_subdirectory(src/broker)
    add_subdirectory(src/esp)
    add_subdirectory(src/FreeRTOSUtils)
    add_subdirectory(src/gpio)
    add_subdirectory(src/i2c)
    add_subdirectory(src/network)
    add_subdirectory(src/uart)

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
            )

    pico_enable_stdio_usb(main 1)
    pico_enable_stdio_uart(main 0)

    add_subdirectory(test/hardware)
    add_subdirectory(test/unit)

elseif (UNIT_TEST)
    if (HARDWARE_TEST)
        message(WARNING "MULTIPLE TARGETS DEFINED")
    endif ()
    message(NOTICE "INFO: Building Unit tests")

    project(ENv5_unittests)
    enable_language(C CXX ASM)
    fetch_git_submodules()
    add_subdirectory(test/unit)
    add_subdirectory(extern/unity)
    # Vim autocomplete plugin (ycm) likes to have  a compilation database
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
else ()
    # initialize pico-sdk from GIT (note this can come from environment, CMake cache etc)
    set(PICO_SDK_FETCH_FROM_GIT 1)
    # Vim autocomplete plugin (ycm) likes to have a compilation database
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    # pico_sdk_import.cmake is a single file copied from this SDK
    # note: this must happen before project()
    include(extern/pico-sdk/pico_sdk_import.cmake)

    project(ENv5)
    enable_language(C CXX ASM)
    fetch_git_submodules()
    pico_sdk_init() # initialize the Raspberry Pi Pico SDK

    if (TARGET tinyusb_device)
        SET(TARGET_GROUP extern)
        add_subdirectory(extern/freeRTOS/)
        add_subdirectory(extern/elastic-ai.runtime.c)

        add_subdirectory(src/broker)
        add_subdirectory(src/esp)
        add_subdirectory(src/FreeRTOSUtils)
        add_subdirectory(src/gpio)
        add_subdirectory(src/i2c)
        add_subdirectory(src/network)
        add_subdirectory(src/uart)

        if (HARDWARE_TEST)
            message(NOTICE "INFO: Building hardware tests")
            add_subdirectory(test/hardware) # Include Hardware tests
        else ()
            message(NOTICE "INFO: Building main executable")
            add_executable(main src/main.c) # creates executable
            target_include_directories(main PUBLIC
                    ${CMAKE_SOURCE_DIR}/src)
            target_link_libraries(main
                    pico_stdlib
                    FreeRTOSUtils
                    network_lib
                    espBroker_lib)
            # enable usb output, disable uart output
            pico_enable_stdio_usb(main 1)
            pico_enable_stdio_uart(main 0)
            # create map/bin/hex/uf2 file etc.
            pico_add_uf2_output(main)
        endif ()
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building Project because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()

# Reset command line options
unset(HARDWARE_TEST CACHE)
unset(UNIT_TEST CACHE)
