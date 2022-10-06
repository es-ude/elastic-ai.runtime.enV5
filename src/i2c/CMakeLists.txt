add_library(i2c_interface INTERFACE)
target_sources(i2c_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/I2c.c)
target_include_directories(i2c_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(i2c_interface INTERFACE
        pico_stdlib
        hardware_i2c
        hardware_gpio)
