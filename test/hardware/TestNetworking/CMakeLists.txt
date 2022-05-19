################## hardwareTestHelper ##################
add_library(hardware-testHelper hardwareTestHelper.c)
target_link_libraries(hardware-testHelper
        pico_stdlib
        esp_lib
        espBroker_lib
        network_lib
        FreeRTOSUtils)
target_compile_definitions(hardware-testHelper PRIVATE DEBUG)
pico_enable_stdio_usb(hardware-testHelper 1)
pico_enable_stdio_uart(hardware-testHelper 0)

################## test_MQTTPublish ##################
add_executable(hardware-test_MQTTPublish test_MQTTPublish.c)
target_link_libraries(hardware-test_MQTTPublish
        pico_stdlib
        FreeRTOSUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTPublish PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTPublish)

################## test_MQTTPublishSubscribe ##################
add_executable(hardware-test_MQTTPublishSubscribe test_MQTTPublishSubscribe.c)
target_link_libraries(hardware-test_MQTTPublishSubscribe
        pico_stdlib
        espBroker_lib
        FreeRTOSUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTPublishSubscribe PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTPublishSubscribe)

################## test_MQTTResetBrokerDomainDuringRuntime ##################
add_executable(hardware-test_MQTTResetBrokerDomainDuringRuntime test_MQTTResetBrokerDomainDuringRuntime.c)
target_link_libraries(hardware-test_MQTTResetBrokerDomainDuringRuntime
        pico_stdlib
        espBroker_lib
        FreeRTOSUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTResetBrokerDomainDuringRuntime PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTResetBrokerDomainDuringRuntime)

################## test_MQTTSubscribe ##################
# creates executable
add_executable(hardware-test_MQTTSubscribe test_MQTTSubscribe.c)
target_link_libraries(hardware-test_MQTTSubscribe
        pico_stdlib
        espBroker_lib
        FreeRTOSUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_MQTTSubscribe PRIVATE DEBUG)
make_to_output_file(hardware-test_MQTTSubscribe)

################## test_network ##################
add_executable(hardware-test_network test_network.c)
target_link_libraries(hardware-test_network
        pico_stdlib
        FreeRTOSUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_network PRIVATE DEBUG)
make_to_output_file(hardware-test_network)

################## test_StressTestPublish ##################
add_executable(hardware-test_StressTestPublish test_StressTestPublish.c)
target_link_libraries(hardware-test_StressTestPublish
        pico_stdlib
        FreeRTOSUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_StressTestPublish PRIVATE DEBUG)
make_to_output_file(hardware-test_StressTestPublish)

################## test_StressTestSubscribe ##################
add_executable(hardware-test_StressTestSubscribe test_StressTestSubscribe.c)
target_link_libraries(hardware-test_StressTestSubscribe
        pico_stdlib
        FreeRTOSUtils
        hardware-testHelper)
target_compile_definitions(hardware-test_StressTestSubscribe PRIVATE DEBUG)
make_to_output_file(hardware-test_StressTestSubscribe)
