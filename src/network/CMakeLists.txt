add_library(network_lib INTERFACE)
target_sources(network_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Network.c)
target_include_directories(network_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(network_lib INTERFACE
        uart_lib
        esp_lib
        at_commands)
