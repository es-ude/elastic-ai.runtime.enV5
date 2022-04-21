function(fetch_git_submodules)
    find_package(Git QUIET)
	if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/.git")
        # Update Submodules
        option(GIT_SUBMODULE "Check submodules build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}")
            endif()
        endif()
    endif()
endfunction()

function(include_src)
    add_subdirectory(src/broker)
    add_subdirectory(src/esp)
    add_subdirectory(src/FreeRTOSUtils)
    add_subdirectory(src/gpio)
    add_subdirectory(src/i2c)
    add_subdirectory(src/network)
    add_subdirectory(src/uart)
    add_subdirectory(src/sht3x)
    add_subdirectory(src/adxl345b)
endfunction()

function(make_to_output_file target)
    # enable usb output, disable uart output
    pico_enable_stdio_usb(${target} 1)
    pico_enable_stdio_uart(${target} 0)
    # create map/bin/hex/uf2 file etc.
    pico_add_uf2_output(${target})
endfunction()