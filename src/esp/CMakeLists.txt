add_library(esp_lib INTERFACE)
target_sources(esp_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/esp.c)
target_include_directories(esp_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(esp_lib INTERFACE
        FreeRTOSUtils
        pico_stdlib
        uart_lib)
