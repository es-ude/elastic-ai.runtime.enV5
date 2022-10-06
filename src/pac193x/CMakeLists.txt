#
# Created by David P. Federl
#

add_library(sensor_lib_pac193x INTERFACE)
target_sources(sensor_lib_pac193x INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Pac193x.c)
target_include_directories(sensor_lib_pac193x INTERFACE
        ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(sensor_lib_pac193x INTERFACE
        i2c_interface
        gpio_interface
        pico_time)