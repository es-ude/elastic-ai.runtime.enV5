if (_BUILDING_FOR_HARDWARE)
    add_library(I2cPico I2cPico.c)
    target_link_libraries(I2cPico PRIVATE
            Common
            Gpio
            Pico::hardware_i2c
            Pico::pico_runtime
            Pico::pico_stdlib
    )
    target_include_directories(I2cPico PUBLIC include)
else ()
    add_library(I2cPico INTERFACE)
    target_include_directories(I2cPico INTERFACE include)
endif ()
