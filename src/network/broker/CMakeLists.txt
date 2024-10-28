add_elastic_ai_lib(
        NAME MqttBroker
        SRCS MqttBroker.c
        DEPS
            Protocol
            Common
            Uart
            AtCommands
            Esp
            Network
            CException
)

target_include_directories(
        MqttBroker__impl
        PRIVATE
            $<TARGET_PROPERTY:topicMatcher,INTERFACE_INCLUDE_DIRECTORIES>)
