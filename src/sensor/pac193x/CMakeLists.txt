add_library(sensor_lib_pac193x INTERFACE)
target_include_directories(sensor_lib_pac193x INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(sensor_lib_pac193x INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Pac193x.c)
target_link_libraries(sensor_lib_pac193x INTERFACE
        common_lib
        sleep_interface
        gpio_interface
        i2c_interface)

add_elastic_ai_lib(
        NAME Pac193
        SRCS Pac193x.c
        DEPS
            Common
            Sleep
            Gpio
            I2c
)