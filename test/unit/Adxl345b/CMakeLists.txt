add_executable(unit-test_Adxl345bNew UnittestAdxl345bNew.c)
target_include_directories(unit-test_Adxl345bNew INTERFACE ${CMAKE_SOURCE_DIR}/src/hal/i2c/include)
target_link_libraries(unit-test_Adxl345bNew sensor_lib_adxl345b unity)