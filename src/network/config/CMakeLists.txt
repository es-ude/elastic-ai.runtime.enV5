if (NOT EXISTS ${CMAKE_SOURCE_DIR}/NetworkConfig.c)
    file(WRITE ${CMAKE_SOURCE_DIR}/NetworkConfig.c "#include \"Network.h\"
#include \"MqttBroker.h\"

networkCredentials_t networkCredentials = {.ssid = \"SSID\", .password = \"password\"};
mqttBrokerHost_t mqttHost = {.ip = \"0.0.0.0\", .port = \"1883\", .userID = \"\", .password = \"\"};
")
endif ()

add_library(NetworkConfig ${CMAKE_SOURCE_DIR}/NetworkConfig.c)
target_link_libraries(NetworkConfig PRIVATE MqttBroker Network)
