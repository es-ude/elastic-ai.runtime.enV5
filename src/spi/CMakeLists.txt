add_library(spi INTERFACE)
target_sources(spi INTERFACE ${CMAKE_CURRENT_LIST_DIR}/spi.c)
target_include_directories(spi INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(spi INTERFACE
        pico_stdlib
        hardware_spi
        hardware_gpio)
