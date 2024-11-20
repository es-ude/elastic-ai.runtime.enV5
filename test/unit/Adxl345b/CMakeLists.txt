add_library(sensor_lib_adxl345b__srcs INTERFACE)
target_include_directories(sensor_lib_adxl345b__srcs INTERFACE
        ${CMAKE_SOURCE_DIR}/src/hal/i2c/include
        ${CMAKE_SOURCE_DIR}/src/hal/time/include
        ${CMAKE_SOURCE_DIR}/src/hal/sleep/include
        ${CMAKE_SOURCE_DIR}/src/sensor/adxl345b/include
)

target_link_libraries(sensor_lib_adxl345b__srcs INTERFACE
        Common
        Adxl345b
        SleepDummy
        TimeDummy
        I2cDummy
)
add_executable(UnitTestAdxl345bNew UnitTestAdxl345bNew.c)
target_link_libraries(UnitTestAdxl345bNew PRIVATE sensor_lib_adxl345b__srcs unity)
add_test(unit-test_Adxl345bNew unit-test_Adxl345bNew)