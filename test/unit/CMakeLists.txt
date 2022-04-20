##############################################
## ADD EXECUTABLE: unit-test_network
add_executable(unit-test_network
        ${CMAKE_CURRENT_LIST_DIR}/test_network.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/hardware/uart.c
        ${CMAKE_SOURCE_DIR}/test/unit/dummies/FreeRTOSUtils/TaskWrapper.c
        ${CMAKE_SOURCE_DIR}/src/network/Network.c
        ${CMAKE_SOURCE_DIR}/src/network/tcp.c
        ${CMAKE_SOURCE_DIR}/src/esp/esp.c
        ${CMAKE_SOURCE_DIR}/src/uart/uartToESP.c)
target_include_directories(unit-test_network PUBLIC
        ${CMAKE_SOURCE_DIR}/test/unit/hardware
        ${CMAKE_SOURCE_DIR}/test/unit/dummies
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/FreeRTOSUtils
        ${CMAKE_SOURCE_DIR}/src/network
        ${CMAKE_SOURCE_DIR}/src/esp
        ${CMAKE_SOURCE_DIR}/src/uart
        ${CMAKE_SOURCE_DIR}/extern)
target_link_libraries(unit-test_network unity)

##############################################
## ADD EXECUTABLE: unit-test_sht3x
add_executable(unit-test_sht3x
        ${CMAKE_CURRENT_LIST_DIR}/test_sht3x.c
        ${CMAKE_SOURCE_DIR}/src/sht3x/sht3x.c)
target_include_directories(unit-test_sht3x PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dummies
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/extern)
target_link_libraries(unit-test_sht3x unity)