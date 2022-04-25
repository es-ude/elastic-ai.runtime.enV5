## Add Test Helper Library
add_library(hardware-testHelper hardwareTestHelper.c)
target_link_libraries(hardware-testHelper
        pico_stdlib
        esp_lib
        espBroker_lib
        network_lib
        FreeRTOSUtils)
pico_enable_stdio_usb(hardware-testHelper 1)
pico_enable_stdio_uart(hardware-testHelper 0)

##############################################
## ADD EXECUTABLE: hardware-test_MQTTPublish.c
add_executable(hardware-test_MQTTPublish test_MQTTPublish.c)
target_link_libraries(hardware-test_MQTTPublish
        pico_stdlib
        FreeRTOSUtils
        hardware-testHelper)
make_to_output_file(hardware-test_MQTTPublish)

##############################################
## ADD EXECUTABLE: hardware-test_MQTTSubscribe.c
add_executable(hardware-test_MQTTPublishSubscribe test_MQTTPublishSubscribe.c)
target_link_libraries(hardware-test_MQTTPublishSubscribe
        pico_stdlib
        espBroker_lib
        FreeRTOSUtils
        hardware-testHelper)
make_to_output_file(hardware-test_MQTTPublishSubscribe)

##############################################
## ADD EXECUTABLE: hardware-test_MQTTSubscribe.c
# creates executable
add_executable(hardware-test_MQTTSubscribe test_MQTTSubscribe.c)
target_link_libraries(hardware-test_MQTTSubscribe
        pico_stdlib
        espBroker_lib
        FreeRTOSUtils
        hardware-testHelper)
make_to_output_file(hardware-test_MQTTSubscribe)

##############################################
## ADD EXECUTABLE: hardware-test_network.c
add_executable(hardware-test_network test_network.c)
target_link_libraries(hardware-test_network
        pico_stdlib
        FreeRTOSUtils
        hardware-testHelper)
make_to_output_file(hardware-test_network)

##############################################
## ADD EXECUTABLE: hardware-test_network.c
add_executable(hardware-test_StressTestPublish test_StressTestPublish.c)
target_link_libraries(hardware-test_StressTestPublish
        pico_stdlib
        FreeRTOSUtils
        hardware-testHelper)
make_to_output_file(hardware-test_StressTestPublish)

##############################################
## ADD EXECUTABLE: hardware-test_network.c
add_executable(hardware-test_StressTestSubscribe test_StressTestSubscribe.c)
target_link_libraries(hardware-test_StressTestSubscribe
        pico_stdlib
        FreeRTOSUtils
        hardware-testHelper)
make_to_output_file(hardware-test_StressTestSubscribe)
