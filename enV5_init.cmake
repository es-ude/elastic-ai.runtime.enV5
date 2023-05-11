function(include_unity)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/extern/unity)
endfunction()

function(enV5_init)
    # add runtime.c
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/extern/elastic-ai.runtime.c)

    # add freeRTOS
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/extern/freeRTOS)

    # libraries from the elastic-ai.runtime.enV5 repo
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/adxl345b)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/atCommands)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/broker)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/env5)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/esp)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/flash)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/fpgaConfig)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/freeRtos)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/gpio)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/i2c)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/middleware)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/network)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/pac193x)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/sht3x)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/spi)
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/uart)
endfunction()
