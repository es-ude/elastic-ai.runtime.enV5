add_library(middleware_lib INTERFACE)
target_sources(middleware_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/middleware.c
        ${CMAKE_CURRENT_LIST_DIR}/qxi.c)
target_include_directories(middleware_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(middleware_lib INTERFACE
        common_lib
        spi_interface
        gpio_interface
        enV5_hwc
        hardware_spi
        pico_stdlib)
