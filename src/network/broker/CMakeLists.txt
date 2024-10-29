add_elastic_ai_lib(
        NAME MqttBroker
        SRCS MqttBroker.c
        DEPS
            Common
            Uart
            AtCommands
            Esp
            Network
            CException
)

target_include_directories(
        MqttBroker__impl
        INTERFACE
            $<TARGET_PROPERTY:topicMatcher,INTERFACE_INCLUDE_DIRECTORIES>
            $<TARGET_PROPERTY:protocol,INTERFACE_INCLUDE_DIRECTORIES>
)

target_include_directories(
        MqttBroker__hdrs
        INTERFACE
            $<TARGET_PROPERTY:protocol,INTERFACE_INCLUDE_DIRECTORIES>

)
