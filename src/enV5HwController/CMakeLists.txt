add_library(enV5_hwc INTERFACE)
target_sources(enV5_hwc INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/enV5HwController.c)
target_include_directories(enV5_hwc INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(enV5_hwc INTERFACE
        gpio_interface
        pico_stdlib)