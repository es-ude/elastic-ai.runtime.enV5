add_library(gpio_interface INTERFACE)
target_sources(gpio_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Gpio.c)
target_include_directories(gpio_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(gpio_interface INTERFACE
        common_lib)
