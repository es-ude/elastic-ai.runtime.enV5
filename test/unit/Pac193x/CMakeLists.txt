add_library(sensor_lib_pac193x__srcs INTERFACE)
target_include_directories(sensor_lib_pac193x__srcs INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/common/include
        ${CMAKE_CURRENT_LIST_DIR}/src/hal/i2c/include
        ${CMAKE_CURRENT_LIST_DIR}/src/hal/sleep/include
        ${CMAKE_CURRENT_LIST_DIR}/src/hal/gpio/include
        ${CMAKE_CURRENT_LIST_DIR}/src/sensor/pac193x/include
)
target_link_libraries(sensor_lib_pac193x__srcs INTERFACE
        Pac193x
        Common
        GpioDummy
        I2cDummy
        SleepDummy
)

add_executable(UnitTestPac193xNew UnitTestPac193x.c)
target_link_libraries(UnitTestPac193xNew PRIVATE sensor_lib_pac193x__srcs unity)
add_test(UnitTestPac193xNew UnitTestPac193xNew)