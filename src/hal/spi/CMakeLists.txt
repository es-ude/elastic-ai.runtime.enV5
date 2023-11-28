# Provides a Facade for the pico-sdk SPI library

add_library(spi_interface INTERFACE)
target_include_directories(spi_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(spi_interface INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Spi.c)
target_link_libraries(spi_interface INTERFACE
        common_lib
        gpio_interface
        hardware_spi)
