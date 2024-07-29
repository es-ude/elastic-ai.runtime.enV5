add_library(sensor_lib_sht3x STATIC
        ${CMAKE_CURRENT_LIST_DIR}/Sht3x.c
)
target_include_directories(sensor_lib_sht3x INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(sensor_lib_sht3x PUBLIC
        CException
)
target_link_libraries(sensor_lib_sht3x PRIVATE
        common_lib
        i2c_interface
)
