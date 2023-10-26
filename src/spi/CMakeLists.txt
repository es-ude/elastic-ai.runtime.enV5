add_library(spi_lib INTERFACE)
target_include_directories(spi_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(spi_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Spi.c)
target_link_libraries(spi_lib INTERFACE
        hardware_spi
        gpio_interface)
