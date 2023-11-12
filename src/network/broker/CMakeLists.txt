add_library(espBroker_lib INTERFACE)
target_include_directories(espBroker_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_SOURCE_DIR}/extern/elastic-ai.runtime.c)
target_sources(espBroker_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/MqttBroker.c)
target_link_libraries(espBroker_lib INTERFACE
        common_lib
        CException
        uart_interface
        at_commands
        network_lib
        elastic-ai.runtime.c
        esp_lib)
