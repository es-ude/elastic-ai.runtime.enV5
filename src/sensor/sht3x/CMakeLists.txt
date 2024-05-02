add_library(sensor_lib_sht3x INTERFACE)
target_include_directories(sensor_lib_sht3x INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(sensor_lib_sht3x INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Sht3x.c)
target_link_libraries(sensor_lib_sht3x INTERFACE
        common_lib
        sleep_interface
        i2c_interface
        enV5_hwc
)
