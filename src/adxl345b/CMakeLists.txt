add_library(sensor_lib_adxl345b INTERFACE)
target_sources(sensor_lib_adxl345b INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Adxl345b.c)
target_include_directories(sensor_lib_adxl345b INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(sensor_lib_adxl345b INTERFACE
        i2c_interface
        pico_time)
