#
# Created by David P. Federl
#

add_library(sensor_lib_sht3x INTERFACE)
target_sources(sensor_lib_sht3x INTERFACE ${CMAKE_CURRENT_LIST_DIR}/sht3x.c)
target_include_directories(sensor_lib_sht3x INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(sensor_lib_sht3x INTERFACE
        i2c_interface
        pico_time)
