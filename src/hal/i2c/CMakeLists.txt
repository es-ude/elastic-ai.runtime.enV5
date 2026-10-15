
# Provides a Facade for the pico-sdk I2C library

if (_BUILDING_FOR_HARDWARE)
    add_library(I2c I2c.c)
    target_link_libraries(I2c PRIVATE
            Common
            Gpio
            Pico::hardware_i2c
            Pico::pico_runtime
    )
    target_include_directories(I2c PUBLIC include)
    add_library(hal2_I2c I2cPico.c)
    target_link_libraries(hal2_I2c PRIVATE
        Pico::hardware_i2c
        Pico::pico_runtime
        Pico::pico_stdlib
        Pico::hardware_gpio
    )
    target_include_directories(hal2_I2c PUBLIC include)
else ()
    add_library(I2c INTERFACE)
    target_include_directories(I2c INTERFACE include)
    add_library(hal2_I2c INTERFACE)
    target_include_directories(hal2_I2c INTERFACE include)
endif ()



