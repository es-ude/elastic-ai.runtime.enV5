################## hardwareTestHelper ##################
add_library(hardware-testHelper HardwaretestHelper.c)
target_link_libraries(hardware-testHelper
        common_lib
        pico_stdlib
        espBroker_lib
        network_lib
        freeRtosUtils)
target_compile_definitions(hardware-testHelper PRIVATE DEBUG)
pico_enable_stdio_usb(hardware-testHelper 1)
pico_enable_stdio_uart(hardware-testHelper 0)

################## test_MQTTPublish ##################
add_executable(hardware-test_MQTTPublish HardwaretestMqttPublish.c)
target_link_libraries(hardware-test_MQTTPublish
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTPublish PRIVATE DEBUG)
create_enV5_executable(hardware-test_MQTTPublish)

################## test_MQTTPublishSubscribe ##################
add_executable(hardware-test_MQTTPublishSubscribe HardwaretestMqttPublishSubscribe.c)
target_link_libraries(hardware-test_MQTTPublishSubscribe
        common_lib
        pico_stdlib
        espBroker_lib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTPublishSubscribe PRIVATE DEBUG)
create_enV5_executable(hardware-test_MQTTPublishSubscribe)

################## test_MQTTSubscribe ##################
# creates executable
add_executable(hardware-test_MQTTSubscribe HardwaretestMqttSubscribe.c)
target_link_libraries(hardware-test_MQTTSubscribe
        common_lib
        pico_stdlib
        espBroker_lib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTSubscribe PRIVATE DEBUG)
create_enV5_executable(hardware-test_MQTTSubscribe)

################## test_StressTestSubscribe ##################
add_executable(hardware-test_MQTTSubscribeUnsubscribe HardwaretestMqttSubscribeUnsubscribe.c)
target_link_libraries(hardware-test_MQTTSubscribeUnsubscribe
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTSubscribeUnsubscribe PRIVATE DEBUG)
create_enV5_executable(hardware-test_MQTTSubscribeUnsubscribe)

################## test_network ##################
add_executable(hardware-test_network HardwaretestNetwork.c)
target_link_libraries(hardware-test_network
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_network PRIVATE DEBUG)
create_enV5_executable(hardware-test_network)

################## test_StressTestPublish ##################
add_executable(hardware-test_StressTestPublish StresstestPublish.c)
target_link_libraries(hardware-test_StressTestPublish
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_StressTestPublish PRIVATE DEBUG)
create_enV5_executable(hardware-test_StressTestPublish)

################## test_StressTestSubscribe ##################
add_executable(hardware-test_StressTestPublishSubscribe StresstestPublishSubscribe.c)
target_link_libraries(hardware-test_StressTestPublishSubscribe
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_StressTestPublishSubscribe PRIVATE DEBUG)
create_enV5_executable(hardware-test_StressTestPublishSubscribe)

################## test_StressTestSubscribe ##################
add_executable(hardware-test_StressTestSubscribe StresstestSubscribe.c)
target_link_libraries(hardware-test_StressTestSubscribe
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_StressTestSubscribe PRIVATE DEBUG)
create_enV5_executable(hardware-test_StressTestSubscribe)

################## test_MqttKeepAlive ##################
add_executable(hardware-test_MQTTKeepalive HardwaretestMqttKeepAlive.c)
target_link_libraries(hardware-test_MQTTKeepalive
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTKeepalive PRIVATE DEBUG)
create_enV5_executable(hardware-test_MQTTKeepalive)

################## test_MQTTPublish ##################
add_executable(hardware-test_MQTTPublishRuntime HardwaretestPublishValuesToRuntime.c)
target_link_libraries(hardware-test_MQTTPublishRuntime
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTPublishRuntime PRIVATE DEBUG)
create_enV5_executable(hardware-test_MQTTPublishRuntime)

################## test_HTTP ##################
add_executable(hardware-test_HTTP HardwaretestHTTP.c)
target_link_libraries(hardware-test_HTTP
        common_lib
        pico_stdlib
        freeRtosUtils
        network_lib
        http_lib
        hardware-testHelper)
target_compile_definitions(hardware-test_HTTP PRIVATE DEBUG)
create_enV5_executable(hardware-test_HTTP)

################## test_Durability ##################
add_executable(hardware-test_Durability HardwaretestDurability.c)
target_link_libraries(hardware-test_Durability
        common_lib
        sensor_lib_pac193x
        hardware_i2c
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_Durability PRIVATE DEBUG)
create_enV5_executable(hardware-test_Durability)

################## test_networkStrength ##################
add_executable(hardware-test_checknetwork HardwaretestCheckNetwork.c)
target_link_libraries(hardware-test_checknetwork
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_checknetwork PRIVATE DEBUG)
create_enV5_executable(hardware-test_checknetwork)

################## test_networkStrength ##################
add_executable(hardware-test_publishSensor HardwaretestPublishSensor.c)
target_link_libraries(hardware-test_publishSensor
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware-testHelper
        sensor_lib_pac193x
        spi_interface
        sensor_lib_adxl345b)
target_compile_definitions(hardware-test_publishSensor PRIVATE DEBUG)
create_enV5_executable(hardware-test_publishSensor)
