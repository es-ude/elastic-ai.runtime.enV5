add_subdirectory(atCommands)
add_subdirectory(esp)
add_subdirectory(wifi)
add_subdirectory(http)
add_subdirectory(broker)

if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/NetworkConfig.c)
    file(WRITE NetworkConfig.c "#include \"Network.h\"
#include \"MqttBroker.h\"

networkCredentials_t networkCredentials = {.ssid = \"SSID\", .password = \"password\"};
mqttBrokerHost_t mqttHost = {.ip = \"0.0.0.0\", .port = \"1883\", .userID = \"\", .password = \"\"};
")
endif ()
add_library(network_config INTERFACE)
target_sources(network_config INTERFACE NetworkConfig.c)
target_include_directories(network_config INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(network_config INTERFACE
        espBroker_lib
        network_lib)
