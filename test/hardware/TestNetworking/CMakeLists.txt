## Add Test Helper Library
add_library(hardwareTestHelper hardwareTestHelper.c)
target_link_libraries(hardwareTestHelper
        pico_stdlib
        espBroker_lib
        network_lib
        FreeRTOSUtils)

##############################################
## ADD EXECUTABLE: hardware-test_network.c
# creates executable
add_executable(hardware-test_network test_network.c)
# link libraries
target_link_libraries(hardware-test_network
        pico_stdlib
        network_lib
        FreeRTOSUtils
        hardwareTestHelper)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_network 1)
pico_enable_stdio_uart(hardware-test_network 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_network)

##############################################
## ADD EXECUTABLE: hardware-test_MQTTPublish.c
# creates executable
add_executable(hardware-test_MQTTPublish test_MQTTPublish.c)
# link libraries
target_link_libraries(hardware-test_MQTTPublish
        pico_stdlib
        network_lib
        espBroker_lib
        FreeRTOSUtils
        hardwareTestHelper)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_MQTTPublish 1)
pico_enable_stdio_uart(hardware-test_MQTTPublish 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_MQTTPublish)

##############################################
## ADD EXECUTABLE: hardware-test_MQTTSubscribe.c
# creates executable
add_executable(hardware-test_MQTTSubscribe test_MQTTSubscribe.c)
#link libraries
target_link_libraries(hardware-test_MQTTSubscribe
        pico_stdlib
        network_lib
        espBroker_lib
        FreeRTOSUtils
        hardwareTestHelper)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_MQTTSubscribe 1)
pico_enable_stdio_uart(hardware-test_MQTTSubscribe 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_MQTTSubscribe)

##############################################
## ADD EXECUTABLE: hardware-test_MQTTSubscribe.c
# creates executable
add_executable(hardware-test_MQTTPublishSubscribe test_MQTTPublishSubscribe.c)
# link libraries
target_link_libraries(hardware-test_MQTTPublishSubscribe
        pico_stdlib
        network_lib
        espBroker_lib
        FreeRTOSUtils
        hardwareTestHelper)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_MQTTPublishSubscribe 1)
pico_enable_stdio_uart(hardware-test_MQTTPublishSubscribe 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_MQTTPublishSubscribe)
