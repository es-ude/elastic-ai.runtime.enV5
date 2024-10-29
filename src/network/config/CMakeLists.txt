if (NOT EXISTS ${CMAKE_SOURCE_DIR}/NetworkConfig.c)
    file(WRITE ${CMAKE_SOURCE_DIR}/NetworkConfig.c "#include \"Network.h\"
#include \"MqttBroker.h\"

networkCredentials_t networkCredentials = {.ssid = \"SSID\", .password = \"password\"};
mqttBrokerHost_t mqttHost = {.ip = \"0.0.0.0\", .port = \"1883\", .userID = \"\", .password = \"\"};
")
endif ()

add_elastic_ai_lib(
        NAME NetworkConfig
        SRCS ${CMAKE_SOURCE_DIR}/NetworkConfig.c
        DEPS MqttBroker Network
)