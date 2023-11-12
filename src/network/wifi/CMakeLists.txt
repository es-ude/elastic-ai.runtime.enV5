add_library(network_lib INTERFACE)
target_sources(network_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Network.c)
target_include_directories(network_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(network_lib INTERFACE
        common_lib
        freeRtosUtils
        uart_interface
        at_commands
        esp_lib
        http_lib)
