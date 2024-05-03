add_library(sensor_lib_adxl345b INTERFACE
        ../../hal/enV5HwController/include/config/Sensor.h)
target_include_directories(sensor_lib_adxl345b INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(sensor_lib_adxl345b INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Adxl345b.c)
target_link_libraries(sensor_lib_adxl345b INTERFACE
        common_lib
        sleep_interface
        i2c_interface
)
