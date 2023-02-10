################## hardwareTestHelper ##################
add_library(hardware-testHelper HardwaretestHelper.c)
target_link_libraries(hardware-testHelper
        pico_stdlib
        esp_lib
        espBroker_lib
        network_lib
        freeRtosUtils)
target_compile_definitions(hardware-testHelper PRIVATE DEBUG)
pico_enable_stdio_usb(hardware-testHelper 1)
pico_enable_stdio_uart(hardware-testHelper 0)

################## test_MQTTPublish ##################
add_executable(hardware-test_MQTTPublish HardwaretestMqttPublish.c)
target_link_libraries(hardware-test_MQTTPublish
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTPublish PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTPublish)

################## test_MQTTPublishSubscribe ##################
add_executable(hardware-test_MQTTPublishSubscribe HardwaretestMqttPublishSubscribe.c)
target_link_libraries(hardware-test_MQTTPublishSubscribe
        pico_stdlib
        espBroker_lib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTPublishSubscribe PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTPublishSubscribe)

################## test_MQTTSubscribe ##################
# creates executable
add_executable(hardware-test_MQTTSubscribe HardwaretestMqttSubscribe.c)
target_link_libraries(hardware-test_MQTTSubscribe
        pico_stdlib
        espBroker_lib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTSubscribe PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTSubscribe)

################## test_StressTestSubscribe ##################
add_executable(hardware-test_MQTTSubscribeUnsubscribe HardwaretestMqttSubscribeUnsubscribe.c)
target_link_libraries(hardware-test_MQTTSubscribeUnsubscribe
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTSubscribeUnsubscribe PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTSubscribeUnsubscribe)

################## test_network ##################
add_executable(hardware-test_network HardwaretestNetwork.c)
target_link_libraries(hardware-test_network
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_network PRIVATE DEBUG)
make_to_output_file(hardware-test_network)

################## test_StressTestPublish ##################
add_executable(hardware-test_StressTestPublish StresstestPublish.c)
target_link_libraries(hardware-test_StressTestPublish
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_StressTestPublish PRIVATE DEBUG)
make_to_output_file(hardware-test_StressTestPublish)

################## test_StressTestSubscribe ##################
add_executable(hardware-test_StressTestPublishSubscribe StresstestPublishSubscribe.c)
target_link_libraries(hardware-test_StressTestPublishSubscribe
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_StressTestPublishSubscribe PRIVATE DEBUG)
make_to_output_file(hardware-test_StressTestPublishSubscribe)

################## test_StressTestSubscribe ##################
add_executable(hardware-test_StressTestSubscribe StresstestSubscribe.c)
target_link_libraries(hardware-test_StressTestSubscribe
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_StressTestSubscribe PRIVATE DEBUG)
make_to_output_file(hardware-test_StressTestSubscribe)

################## test_MqttKeepAlive ##################
add_executable(hardware-test_MQTTKeepalive HardwaretestMqttKeepAlive.c)
target_link_libraries(hardware-test_MQTTKeepalive
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTKeepalive PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTKeepalive)

################## test_MQTTPublish ##################
add_executable(hardware-test_MQTTPublishRuntime HardwaretestPublishValuesToRuntime.c)
target_link_libraries(hardware-test_MQTTPublishRuntime
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTPublishRuntime PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTPublishRuntime)

################## test_Durability ##################
add_executable(hardware-test_Durability HardwaretestDurability.c)
target_link_libraries(hardware-test_Durability
        sensor_lib_pac193x
        hardware_i2c
        pico_stdlib
        freeRtosUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_Durability PRIVATE DEBUG)
make_to_output_file(hardware-test_Durability)
