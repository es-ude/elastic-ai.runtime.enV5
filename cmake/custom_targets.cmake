function(create_enV5_executable target)
    # enable usb output
    pico_enable_stdio_usb(${target} 1)
    # disable uart output
    pico_enable_stdio_uart(${target} 0)
    # create map/bin/hex/uf2 file etc.
    pico_add_uf2_output(${target})
    #    # copy u2f files after build to out directory
    #    file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    #    add_custom_command(TARGET ${target} POST_BUILD
    #            COMMAND ${CMAKE_COMMAND} -E copy
    #            ${CMAKE_BINARY_DIR}/${relative_path}/${target}.uf2
    #            ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}-Rev${REVISION}/${relative_path}/${target}.uf2)
endfunction()