add_executable(unit-test_Adxl345bNew UnittestAdxl345bNew.c)
target_include_directories(unit-test_Adxl345bNew PRIVATE ${CMAKE_SOURCE_DIR}/src/hal/i2c/include
        ${CMAKE_SOURCE_DIR}/src/hal/time/include
        ${CMAKE_SOURCE_DIR}/src/hal/sleep/include
        ${CMAKE_SOURCE_DIR}/src/sensor/adxl345b/include
)

target_link_libraries(unit-test_Adxl345bNew sensor_lib_adxl345b__srcs unity)

add_test(unit-test_Adxl345bNew unit-test_Adxl345bNew)