add_library(espBroker_lib INTERFACE)
target_sources(espBroker_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/MqttBroker.c)
target_include_directories(espBroker_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/extern/elastic-ai.runtime.c)
target_link_libraries(espBroker_lib INTERFACE
        elastic-ai.runtime.c
        uart_lib
        esp_lib
        network_lib)
