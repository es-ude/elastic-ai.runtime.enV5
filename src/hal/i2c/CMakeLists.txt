# Provides a Facade for the pico-sdk I2C library

if(BUILDING_FOR_ELASTIC_NODE)
    add_library(I2c I2c.c)
    target_link_libraries(I2c PRIVATE Common Gpio Pico::hardware_i2c)
    target_include_directories(I2c PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
else()
    add_library(I2c INTERFACE)
    target_include_directories(I2c INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif ()
