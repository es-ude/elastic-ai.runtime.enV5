##############################################
## region UNIT-TEST: unit-test_sht3
add_subdirectory(dummies/http)
add_subdirectory(dummies/flash)
add_subdirectory(dummies/i2c)
add_subdirectory(dummies/gpio)
add_subdirectory(dummies/sleep)
add_subdirectory(dummies/esp)
add_subdirectory(dummies/rtos)


add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Sht3x
        MORE_SOURCES
            ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/I2c.c
        MORE_LIBS
            CException I2c__hdrs
)
target_include_directories(UnitTestSht3x PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/include)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            FpgaConfigurationHandler
        MORE_LIBS
            CException
            Common
            Flash__hdrs
            FlashDummy
            HttpDummy
            Http__hdrs
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Pac193
        MORE_LIBS
            Common
            I2cDummy
            GpioDummy
            SleepDummy
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Network
        MORE_LIBS
            Common
            HttpDummy
            RtosDummy
            EspDummy
            Esp__hdrs
            CException
            NetworkConfig
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Http
        MORE_LIBS
            CException
            Common
            EspDummy
            Esp__hdrs
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            MqttBroker
        MORE_LIBS
            CException
            Common
            HttpDummy
            EspDummy
            Esp__hdrs
            topicMatcher
            protocol
            NetworkConfig

)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Adxl345b
        MORE_LIBS
            CException
            Common
            Sleep__hdrs
            Time__hdrs
            I2c__hdrs
)
#add_executable(unit-test_sht3x ${CMAKE_CURRENT_LIST_DIR}/UnittestSht3x.c)
#target_link_libraries(unit-test_sht3x
#        sensor_lib_sht3x
#        i2c_interface
#        unity)
#add_test(unit-test_sht3x unit-test_sht3x)
### endregion
###############################################
### region UNIT-TEST: unit-test_adxl345b
#add_executable(unit-test_adxl345b ${CMAKE_CURRENT_LIST_DIR}/UnittestAdxl345b.c)
#target_link_libraries(unit-test_adxl345b
#        sensor_lib_adxl345b
#        time_interface
#        unity)
#add_test(unit-test_adxl345b unit-test_adxl345b)
### endregion
###############################################
### region UNIT-TEST: unit-test_pac193x
#add_executable(unit-test_pac193x ${CMAKE_CURRENT_LIST_DIR}/UnittestPac193x.c)
#target_link_libraries(unit-test_pac193x
#        sensor_lib_pac193x
#        unity)
#add_test(unit-test_pac193x unit-test_pac193x)
### endregion
###############################################
### region UNIT-TEST: unit-test_network
#add_executable(unit-test_network ${CMAKE_CURRENT_LIST_DIR}/UnittestNetwork.c)
#target_link_libraries(unit-test_network
#        network_config
#        esp_lib
#        network_lib
#        unity
#        CException)
#add_test(unit-test_network unit-test_network)
### endregion
###############################################
### region UNIT-TEST: unit-test_mqtt
#add_executable(unit-test_mqtt ${CMAKE_CURRENT_LIST_DIR}/UnittestMqtt.c)
#target_link_libraries(unit-test_mqtt
#        network_config
#        esp_lib
#        espBroker_lib
#        unity
#        CException)
#add_test(unit-test_mqtt unit-test_mqtt)
### endregion
###############################################
### region UNIT-TEST: unit-test_http
#add_executable(unit-test_http ${CMAKE_CURRENT_LIST_DIR}/UnittestHTTP.c)
#target_link_libraries(unit-test_http
#        http_lib
#        unity
#        CException)
#add_test(unit-test_http unit-test_http)
### endregion
###############################################
### region UNIT-TEST: unit-test_fpgaConfigurationHandler
## replace http lib with dummy
#get_target_property(FPGA_CONFIG_LIBS fpga_configuration_handler INTERFACE_LINK_LIBRARIES)
#list(REMOVE_ITEM FPGA_CONFIG_LIBS http_lib)
#list(APPEND FPGA_CONFIG_LIBS http_lib_dummy)
#set_property(TARGET fpga_configuration_handler PROPERTY INTERFACE_LINK_LIBRARIES ${FPGA_CONFIG_LIBS})
## replace http lib with dummy
#get_target_property(NET_LIBS network_lib INTERFACE_LINK_LIBRARIES)
#list(REMOVE_ITEM NET_LIBS http_lib)
#list(APPEND NET_LIBS http_lib_dummy)
#set_property(TARGET network_lib PROPERTY INTERFACE_LINK_LIBRARIES ${NET_LIBS})
## add test executable
#add_executable(unit-test_fpgaConfigurationHandler ${CMAKE_CURRENT_LIST_DIR}/UnittestFpgaConfigurationHandler.c)
#target_link_libraries(unit-test_fpgaConfigurationHandler
#        http_lib_dummy
#        flash_lib
#        fpga_configuration_handler
#        unity)
#add_test(unit-test_fpgaConfigurationHandler unit-test_fpgaConfigurationHandler)
### endregion
###############################################


add_custom_target(all_unit_tests)

add_dependencies(all_unit_tests
        UnitTestSht3x
        UnitTestFpgaConfigurationHandler
        UnitTestPac193
        UnitTestNetwork
        UnitTestHttp
        UnitTestMqttBroker
        UnitTestAdxl345b
)