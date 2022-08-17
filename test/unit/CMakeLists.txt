#############################################
## region UNIT-TEST: unit-test_sht3
add_executable(unit-test_sht3x
        ${CMAKE_CURRENT_LIST_DIR}/test_sht3x.c
        ${CMAKE_SOURCE_DIR}/src/sht3x/sht3x.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/i2c.c)
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
        ${CMAKE_CURRENT_LIST_DIR}/test_adxl345b.c
        ${CMAKE_SOURCE_DIR}/src/adxl345b/adxl345b.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/i2c.c)
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
        ${CMAKE_CURRENT_LIST_DIR}/test_pac193x.c
        ${CMAKE_SOURCE_DIR}/src/pac193x/pac193x.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/gpio/gpio.c
        ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/i2c.c)
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
        ${CMAKE_CURRENT_LIST_DIR}/test_network.c
        ${CMAKE_SOURCE_DIR}/src/network/Network.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/hardware/uart.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/esp/esp.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/FreeRTOSUtils/TaskWrapper.c
        ${CMAKE_SOURCE_DIR}/src/uart/uartToESP.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/gpio/gpio.c)
target_include_directories(unit-test_network PUBLIC
        ${CMAKE_SOURCE_DIR}/test/unit/dummies
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/hardware
        ${CMAKE_SOURCE_DIR}/src/gpio
        ${CMAKE_SOURCE_DIR}/src/at_commands
        ${CMAKE_SOURCE_DIR}/src/esp
        ${CMAKE_SOURCE_DIR}/src/FreeRTOSUtils
        ${CMAKE_SOURCE_DIR}/src/network
        ${CMAKE_SOURCE_DIR}/src/uart
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit-test_network unity)
add_test(unit-test_network unit-test_network)
## endregion
