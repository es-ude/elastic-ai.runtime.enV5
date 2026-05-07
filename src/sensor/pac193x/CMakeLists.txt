add_library(Pac193x Pac193x.c)
target_include_directories(Pac193x PUBLIC include)
target_link_libraries(Pac193x PRIVATE
        Common
        Sleep
        Gpio
        I2c
)

add_library(Pac193x_New Pac193x_New.c)
target_include_directories(Pac193x_New PUBLIC include)
target_link_libraries(Pac193x_New PRIVATE
        Common
        Sleep
        Gpio
        I2cPico
        Pico::pico_stdlib
        Pico::hardware_i2c
)