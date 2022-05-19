add_library(espBroker_lib INTERFACE)
target_sources(espBroker_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/MQTTBroker.c)
target_include_directories(espBroker_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/extern/elastic-ai.runtime.c)
target_link_libraries(espBroker_lib INTERFACE
        elastic-ai.runtime.c
        esp_lib
        uart_lib
        network_lib)
