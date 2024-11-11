add_library(MqttBroker MqttBroker.c)
target_include_directories(MqttBroker PUBLIC include)
target_link_libraries(MqttBroker PRIVATE Common Uart AtCommands Esp Network CException)

target_include_directories(MqttBroker PRIVATE
    $<TARGET_PROPERTY:topicMatcher,INTERFACE_INCLUDE_DIRECTORIES>)

target_include_directories(MqttBroker PUBLIC
    $<TARGET_PROPERTY:protocol,INTERFACE_INCLUDE_DIRECTORIES>
)
