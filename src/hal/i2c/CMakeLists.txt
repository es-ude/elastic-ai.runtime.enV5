# Provides a Facade for the pico-sdk I2C library

add_library(i2c_interface INTERFACE)
target_include_directories(i2c_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(i2c_interface INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/I2c.c)
target_link_libraries(i2c_interface INTERFACE
        common_lib
        gpio_interface
        hardware_i2c
)
