cmake_minimum_required(VERSION 3.13)
include(helpers.cmake)

OPTION(DEVELOP "For best use developing with an IDE" OFF)
OPTION(UNIT_TEST "Builds Unit tests" OFF)
OPTION(HARDWARE_TEST "Builds hardware tests" OFF)

if ((UNIT_TEST AND DEVELOP) OR (UNIT_TEST AND DEVELOP) OR (DEVELOP AND HARDWARE_TEST))
    message(SEND_ERROR "MULTIPLE TARGETS DEFINED")
endif ()

enable_language(C CXX ASM)
fetch_git_submodules()

if (DEVELOP)
    message(NOTICE "INFO: Including everything for development")

    include_pico()

    project(ENv5)

    add_subdirectory(extern/freeRTOS/)
    SET(TARGET_GROUP extern)
    add_subdirectory(extern/elastic-ai.runtime.c)
    add_subdirectory(extern/unity)

    include_src()

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
endif ()

if (UNIT_TEST)
    message(NOTICE "INFO: Building Unit tests")

    project(ENv5_unittests)

    add_subdirectory(test/unit)
    add_subdirectory(extern/unity)
endif ()

if (HARDWARE_TEST)
    message(NOTICE "INFO: Building hardware tests")

    include_pico()

    project(ENv5-Hardware_Test)

    if (TARGET tinyusb_device)
        SET(TARGET_GROUP extern)
        add_subdirectory(extern/freeRTOS/)
        add_subdirectory(extern/elastic-ai.runtime.c)

        include_src()

        add_subdirectory(test/hardware)
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building Project because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()

if (NOT DEVELOP AND NOT UNIT_TEST AND NOT HARDWARE_TEST)
    message(NOTICE "INFO: Building main executable")

    include_pico()

    project(ENv5)

    if (TARGET tinyusb_device)
        add_subdirectory(extern/freeRTOS/)
        SET(TARGET_GROUP extern)
        add_subdirectory(extern/elastic-ai.runtime.c)

        include_src()

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
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building Project because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()

# Vim autocomplete plugin (ycm) likes to have a compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Reset command line options
unset(HARDWARE_TEST CACHE)
unset(UNIT_TEST CACHE)
