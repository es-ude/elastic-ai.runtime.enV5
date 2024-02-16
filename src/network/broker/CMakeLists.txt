add_library(espBroker_lib INTERFACE)
target_include_directories(espBroker_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(espBroker_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/MqttBroker.c)
target_link_libraries(espBroker_lib INTERFACE
        CException
        topicMatcher
        protocol
        common_lib
        uart_interface
        at_commands
        esp_lib
        network_lib
        m
        network_config)
