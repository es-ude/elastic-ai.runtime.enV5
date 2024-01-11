# Provides a Facade for the pico-sdk GPIO library

add_library(gpio_interface INTERFACE)
target_include_directories(gpio_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(gpio_interface INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Gpio.c)
target_link_libraries(gpio_interface INTERFACE
        common_lib
        hardware_gpio)
