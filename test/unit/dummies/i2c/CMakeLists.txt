add_library(i2c_interface INTERFACE)
target_include_directories(i2c_interface INTERFACE
        ${CMAKE_SOURCE_DIR}/src/hal/i2c/include
        ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(i2c_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/I2c.c)
target_link_libraries(i2c_interface INTERFACE
        common_lib
        gpio_interface)
