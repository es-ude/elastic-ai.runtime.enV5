add_library(test_lib_mqtt_broker__srcs INTERFACE)
target_include_directories(test_lib_mqtt_broker__srcs INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/network/esp/include
)
target_link_libraries(test_lib_mqtt_broker__srcs INTERFACE
        MqttBroker
        EspDummy
        CException
        m
)

add_executable(UnitTestMqttBrokerNew UnitTestMqttBroker.c)
target_link_libraries(UnitTestMqttBrokerNew PRIVATE test_lib_mqtt_broker__srcs unity)
add_test(UnitTestMqttBrokerNew UnitTestMqttBrokerNew)