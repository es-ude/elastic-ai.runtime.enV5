cmake_minimum_required(VERSION 3.13)
include(helpers.cmake)

OPTION(BUILD_TESTS "Generate Build to run tests" 1)

if (BUILD_TESTS)
    project(ENv5_unittests)
    enable_language(C CXX ASM)
    fetch_git_submodules()
    add_subdirectory(test/unit)
    add_subdirectory(extern/unity)

    # Vim autocomplete plugin (ycm) likes to have
    # a compilation database
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
else ()
    # initialize pico-sdk from GIT
    # (note this can come from environment, CMake cache etc)
    set(PICO_SDK_FETCH_FROM_GIT 1)

    # Vim autocomplete plugin (ycm) likes to have
    # a compilation database
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    #set(CMAKE_BUILD_TYPE Debug)

    # pico_sdk_import.cmake is a single file copied from this SDK
    # note: this must happen before project()
    include(extern/pico-sdk/pico_sdk_import.cmake)
    project(ENv5)
    enable_language(C CXX ASM)
    fetch_git_submodules()
    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()

    if (TARGET tinyusb_device)
        ##############################################
        ## ADD LIBRARIES
        # add freeRTOS libraries
        add_subdirectory(extern/freeRTOS/)
        add_subdirectory(src/FreeRTOSUtils)
        # add elastic-ai.runtime.c
        SET(TARGET_GROUP extern)
        add_subdirectory(extern/elastic-ai.runtime.c)
        # adds esp Broker
        add_subdirectory(src/espBroker)
        # adds uart library
        add_subdirectory(src/uart)
        # adds network libraries
        add_subdirectory(src/esp)
        add_subdirectory(src/network)
        # add i²c interface
        add_subdirectory(src/i2c)
        # add gpio interface
        add_subdirectory(src/gpio)
        ##############################################
        ## ADD EXECUTABLE: main
        # creates executable
        add_executable(main src/main.c)
        # add libraries
        target_include_directories(main PUBLIC
                ${CMAKE_SOURCE_DIR}/src)
        target_link_libraries(main
                pico_stdlib
                FreeRTOSUtils
                network_lib
                espBroker_lib
                )
        # enable usb output, disable uart output
        pico_enable_stdio_usb(main 1)
        pico_enable_stdio_uart(main 0)
        # create map/bin/hex/uf2 file etc.
        #pico_add_extra_outputs(main)
        pico_add_uf2_output(main)

        ##############################################
        ## Include Hardware tests
        add_subdirectory(test/hardware)
    elseif (PICO_ON_DEVICE)
        message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
    endif ()
endif ()
