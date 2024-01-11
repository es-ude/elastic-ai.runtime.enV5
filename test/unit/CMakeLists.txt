##############################################
## region UNIT-TEST: unit-test_sht3
add_executable(unit-test_sht3x ${CMAKE_CURRENT_LIST_DIR}/UnittestSht3x.c)
target_link_libraries(unit-test_sht3x
        sensor_lib_sht3x
        unity)
add_test(unit-test_sht3x unit-test_sht3x)
## endregion
##############################################
## region UNIT-TEST: unit-test_adxl345b
add_executable(unit-test_adxl345b ${CMAKE_CURRENT_LIST_DIR}/UnittestAdxl345b.c)
target_link_libraries(unit-test_adxl345b
        sensor_lib_adxl345b
        unity)
add_test(unit-test_adxl345b unit-test_adxl345b)
## endregion
##############################################
## region UNIT-TEST: unit-test_pac193x
add_executable(unit-test_pac193x ${CMAKE_CURRENT_LIST_DIR}/UnittestPac193x.c)
target_link_libraries(unit-test_pac193x
        sensor_lib_pac193x
        unity)
add_test(unit-test_pac193x unit-test_pac193x)
## endregion
##############################################
## region UNIT-TEST: unit-test_network
add_executable(unit-test_network ${CMAKE_CURRENT_LIST_DIR}/UnittestNetwork.c)
target_link_libraries(unit-test_network
        esp_lib
        network_lib
        unity
        CException)
add_test(unit-test_network unit-test_network)
## endregion
##############################################
## region UNIT-TEST: unit-test_mqtt
add_executable(unit-test_mqtt ${CMAKE_CURRENT_LIST_DIR}/UnittestMqtt.c)
target_link_libraries(unit-test_mqtt
        esp_lib
        espBroker_lib
        unity
        CException)
add_test(unit-test_mqtt unit-test_mqtt)
## endregion
##############################################
## region UNIT-TEST: unit-test_http
add_executable(unit-test_http ${CMAKE_CURRENT_LIST_DIR}/UnittestHTTP.c)
target_link_libraries(unit-test_http
        esp_lib
        http_lib
        unity
        CException)
add_test(unit-test_http unit-test_http)
## endregion
##############################################
## region UNIT-TEST: unit-test_fpgaConfigurationHandler
# replace http lib with dummy
get_target_property(FPGA_CONFIG_LIBS fpga_configuration_handler INTERFACE_LINK_LIBRARIES)
list(REMOVE_ITEM FPGA_CONFIG_LIBS http_lib)
list(APPEND FPGA_CONFIG_LIBS http_lib_dummy)
set_property(TARGET fpga_configuration_handler PROPERTY INTERFACE_LINK_LIBRARIES ${FPGA_CONFIG_LIBS})
# replace http lib with dummy
get_target_property(NET_LIBS network_lib INTERFACE_LINK_LIBRARIES)
list(REMOVE_ITEM NET_LIBS http_lib)
list(APPEND NET_LIBS http_lib_dummy)
set_property(TARGET network_lib PROPERTY INTERFACE_LINK_LIBRARIES ${NET_LIBS})
# add test executable
add_executable(unit-test_fpgaConfigurationHandler ${CMAKE_CURRENT_LIST_DIR}/UnittestFpgaConfigurationHandler.c)
target_link_libraries(unit-test_fpgaConfigurationHandler
        http_lib_dummy
        flash_lib
        fpga_configuration_handler
        unity)
add_test(unit-test_fpgaConfigurationHandler unit-test_fpgaConfigurationHandler)
## endregion
