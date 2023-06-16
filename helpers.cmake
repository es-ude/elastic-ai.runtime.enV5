function(include_src)
    include_directories(src)
    add_subdirectory(src/broker)
    add_subdirectory(src/esp)
    add_subdirectory(src/freeRtos)
    add_subdirectory(src/gpio)
    add_subdirectory(src/i2c)
    add_subdirectory(src/network)
    add_subdirectory(src/uart)
    add_subdirectory(src/sht3x)
    add_subdirectory(src/adxl345b)
    add_subdirectory(src/pac193x)
    add_subdirectory(src/spi)
    add_subdirectory(src/fpgaConfig)
    add_subdirectory(src/middleware)
    add_subdirectory(src/flash)
    add_subdirectory(src/env5)
    add_subdirectory(src/atCommands)
    add_subdirectory(src/http)
    add_subdirectory(src/time)
endfunction()

function(make_to_output_file target)
    # enable usb output, disable uart output
    pico_enable_stdio_usb(${target} 1)
    pico_enable_stdio_uart(${target} 0)
    # create map/bin/hex/uf2 file etc.
    pico_add_uf2_output(${target})
    # move u2f files after build to out directory
    file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/${relative_path}/${target}.uf2
            ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/${relative_path}/${target}.uf2)
endfunction()
