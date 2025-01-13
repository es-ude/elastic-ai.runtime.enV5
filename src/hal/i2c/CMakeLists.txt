# Provides a Facade for the pico-sdk I2C library

if(BUILDING_FOR_ELASTIC_NODE)
    add_library(I2c I2c.c)
    target_link_libraries(I2c PRIVATE Common Gpio Pico::hardware_i2c Pico::pico_runtime)
    target_include_directories(I2c PUBLIC include)
else()
    add_library(I2c INTERFACE)
    target_include_directories(I2c INTERFACE include)
endif ()
