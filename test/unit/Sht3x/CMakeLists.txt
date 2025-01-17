add_library(sensor_lib_sht3x__srcs INTERFACE)
target_include_directories(sensor_lib_sht3x__srcs INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/hal/i2c
)
target_link_libraries(sensor_lib_sht3x__srcs INTERFACE
        Sht3x
        CException
        I2cDummy

)

add_executable(UnitTestSht3xNew UnitTestSht3x.c)
target_link_libraries(UnitTestSht3xNew PRIVATE sensor_lib_sht3x__srcs unity)
add_test(UnitTestSht3xNew UnitTestSht3xNew)