##############################################
## ADD EXECUTABLE: hardware-test_network.c
# creates executable
add_executable(hardware-test_network test_network.c)
# add libraries
target_include_directories(hardware-test_network PUBLIC
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(hardware-test_network
        pico_stdlib
        network_lib
        FreeRTOSUtils)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_network 1)
pico_enable_stdio_uart(hardware-test_network 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_network)

##############################################
## ADD EXECUTABLE: hardware-test_MQTT.c
# creates executable
add_executable(hardware-test_MQTT test_MQTT.c)
# add libraries
target_include_directories(hardware-test_MQTT PUBLIC
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(hardware-test_MQTT
        pico_stdlib
        esp_lib
        espBroker_lib
        FreeRTOSUtils)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_MQTT 1)
pico_enable_stdio_uart(hardware-test_MQTT 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_MQTT)
