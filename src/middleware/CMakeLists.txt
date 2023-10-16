add_library(middleware_lib INTERFACE)
target_sources(middleware_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/middleware.c
        ${CMAKE_CURRENT_LIST_DIR}/qxi.c
)
target_include_directories(middleware_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(middleware_lib INTERFACE
        pico_stdlib
        hardware_spi
        gpio_interface
        enV5_hwc
        spi_lib)
