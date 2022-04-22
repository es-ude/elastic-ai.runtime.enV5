add_library(spi_lib INTERFACE)
target_sources(spi_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/spi_handler.c)
target_include_directories(spi_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(spi_lib INTERFACE
        pico_stdlib
        hardware_spi
        hardware_gpio)
