IF(NOT WIFI_SSID)
    SET(WIFI_SSID SSID)
ENDIF()
IF(NOT WIFI_PWD)
    SET(WIFI_PWD PASSWORD)
ENDIF()
IF(NOT MQTT_HOST)
    SET(MQTT_HOST 0.0.0.0)
ENDIF()
IF(NOT MQTT_PORT)
    SET(MQTT_PORT 1883)
ENDIF()


if (NOT EXISTS ${CMAKE_SOURCE_DIR}/NetworkConfig.c)
    file(WRITE ${CMAKE_SOURCE_DIR}/NetworkConfig.c "
#include \"Network.h\"
#include \"MqttBroker.h\"

networkCredentials_t networkCredentials = {
    .ssid = \"${WIFI_SSID}\",
    .password = \"${WIFI_PWD}\"
};
mqttBrokerHost_t mqttHost = {
    .ip = \"${MQTT_HOST}\",
    .port = \"${MQTT_PORT}\",
    .userID = \"${MQTT_USER}\",
    .password = \"${MQTT_PWD}\"
};
")
endif ()

add_library(network_config INTERFACE)
target_sources(network_config INTERFACE
        ${CMAKE_SOURCE_DIR}/NetworkConfig.c)
target_link_libraries(network_config INTERFACE
        espBroker_lib
        network_lib)
