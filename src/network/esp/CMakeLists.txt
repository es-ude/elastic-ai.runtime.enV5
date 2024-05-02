add_library(esp_lib INTERFACE)
target_include_directories(esp_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(esp_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Esp.c)
target_link_libraries(esp_lib INTERFACE
        freeRtosUtils
        common_lib
        uart_interface
        at_commands
        enV5_hwc
)