add_library(gpio_interface INTERFACE)
target_sources(gpio_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Gpio.c)
target_include_directories(gpio_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(gpio_interface INTERFACE
        pico_stdlib
        hardware_gpio)
