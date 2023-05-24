#############################################
## region UNIT-TEST: unit-test_sht3
add_executable(unit-test_sht3x
        ${CMAKE_CURRENT_LIST_DIR}/UnittestSht3x.c
        ${CMAKE_SOURCE_DIR}/src/sht3x/Sht3x.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/I2c.c)
target_include_directories(unit-test_sht3x PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dummies
        ${CMAKE_SOURCE_DIR}/src/i2c
        ${CMAKE_SOURCE_DIR}/src/sht3x
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit-test_sht3x unity)
add_test(unit-test_sht3x unit-test_sht3x)
## endregion
##############################################
## region UNIT-TEST: unit-test_adxl345b
add_executable(unit-test_adxl345b
        ${CMAKE_CURRENT_LIST_DIR}/UnittestAdxl345b.c
        ${CMAKE_SOURCE_DIR}/src/adxl345b/Adxl345b.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/I2c.c)
target_include_directories(unit-test_adxl345b PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dummies
        ${CMAKE_SOURCE_DIR}/src/i2c
        ${CMAKE_SOURCE_DIR}/src/adxl345b
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit-test_adxl345b unity)
add_test(unit-test_adxl345b unit-test_adxl345b)
## endregion
##############################################
## region UNIT-TEST: unit-test_pac193x
add_executable(unit-test_pac193x
        ${CMAKE_CURRENT_LIST_DIR}/UnittestPac193x.c
        ${CMAKE_SOURCE_DIR}/src/pac193x/Pac193x.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/gpio/Gpio.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/I2c.c)
target_include_directories(unit-test_pac193x PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dummies
        ${CMAKE_SOURCE_DIR}/src/gpio
        ${CMAKE_SOURCE_DIR}/src/i2c
        ${CMAKE_SOURCE_DIR}/src/pac193x
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit-test_pac193x unity)
add_test(unit-test_pac193x unit-test_pac193x)
## endregion
##############################################
## region UNIT-TEST: unit-test_network
add_executable(unit-test_network
        ${CMAKE_CURRENT_LIST_DIR}/UnittestNetwork.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/hardware/uart.c
        ${CMAKE_SOURCE_DIR}/src/uart/Uart.c
        ${CMAKE_SOURCE_DIR}/src/network/Network.c
        ${CMAKE_SOURCE_DIR}/src/http/HTTP.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/esp/Esp.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/freeRtos/FreeRtosTaskWrapper.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/gpio/Gpio.c)
target_include_directories(unit-test_network PUBLIC
        ${CMAKE_SOURCE_DIR}/test/unit/dummies
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/hardware
        ${CMAKE_SOURCE_DIR}/src/http
        ${CMAKE_SOURCE_DIR}/src/gpio
        ${CMAKE_SOURCE_DIR}/src/atCommands
        ${CMAKE_SOURCE_DIR}/src/esp
        ${CMAKE_SOURCE_DIR}/src/freeRtos
        ${CMAKE_SOURCE_DIR}/src/network
        ${CMAKE_SOURCE_DIR}/src/uart
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit-test_network unity)
add_test(unit-test_network unit-test_network)
## endregion
##############################################
## region UNIT-TEST: unit-test_mqtt
add_executable(unit-test_mqtt
        ${CMAKE_CURRENT_LIST_DIR}/UnittestMqtt.c
        ${CMAKE_SOURCE_DIR}/src/broker/MqttBroker.c
        ${CMAKE_SOURCE_DIR}/src/uart/Uart.c
        ${CMAKE_SOURCE_DIR}/extern/elastic-ai.runtime.c/src/Protocol.c
        ${CMAKE_SOURCE_DIR}/extern/elastic-ai.runtime.c/src/TopicMatcher.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/gpio/Gpio.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/esp/Esp.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/hardware/uart.c)
target_include_directories(unit-test_mqtt PUBLIC
        ${CMAKE_SOURCE_DIR}/test/unit/dummies
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/hardware
        ${CMAKE_SOURCE_DIR}/src/http
        ${CMAKE_SOURCE_DIR}/src/gpio
        ${CMAKE_SOURCE_DIR}/src/atCommands
        ${CMAKE_SOURCE_DIR}/src/esp
        ${CMAKE_SOURCE_DIR}/src/FreeRTOSUtils
        ${CMAKE_SOURCE_DIR}/src/broker
        ${CMAKE_SOURCE_DIR}/src/uart
        ${CMAKE_SOURCE_DIR}/extern/elastic-ai.runtime.c/src
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit-test_mqtt unity m)
add_test(unit-test_mqtt unit-test_mqtt)
## endregion
##############################################
## region UNIT-TEST: unit-test_configuration
add_executable(unit-test_configuration
        ${CMAKE_CURRENT_LIST_DIR}/UnittestConfiguration.c
        ${CMAKE_SOURCE_DIR}/src/fpgaConfig/FpgaConfiguration.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/flash/Flash.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/flash/FpgaConfigDatahandler.c)
target_include_directories(unit-test_configuration PUBLIC
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/flash
        ${CMAKE_SOURCE_DIR}/test/unit/dummies
        ${CMAKE_SOURCE_DIR}/src/flash
        ${CMAKE_SOURCE_DIR}/src/fpgaConfig
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit-test_configuration unity m)
add_test(unit-test_configuration unit-test_configuration)
## endregion
##############################################
## region UNIT-TEST: unit-test_configurationHttp
add_executable(unit-test_configurationHttp
        ${CMAKE_CURRENT_LIST_DIR}/UnittestConfigurationHttp.c
        ${CMAKE_SOURCE_DIR}/src/fpgaConfig/FpgaConfigurationHttp.c
        ${CMAKE_SOURCE_DIR}/src/fpgaConfig/FpgaConfiguration.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/flash/Flash.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/http/HTTP.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/flash/FpgaConfigDatahandler.c)
target_include_directories(unit-test_configurationHttp PUBLIC
        ${CMAKE_SOURCE_DIR}/test/unit/dummies
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/flash
        ${CMAKE_SOURCE_DIR}/src/flash
        ${CMAKE_SOURCE_DIR}/src/fpgaConfig
        ${CMAKE_SOURCE_DIR}/src/network
        ${CMAKE_SOURCE_DIR}/src/http
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit-test_configurationHttp unity m)
add_test(unit-test_configurationHttp unit-test_configurationHttp)
## endregion
