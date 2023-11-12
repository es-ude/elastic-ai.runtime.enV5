function(include_cexception)
    add_library(CException INTERFACE)
    target_sources(CException INTERFACE ${CMAKE_SOURCE_DIR}/extern/cexception/lib/CException.c)
    target_include_directories(CException INTERFACE ${CMAKE_SOURCE_DIR}/extern/cexception/lib)
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
