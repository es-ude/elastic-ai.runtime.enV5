add_library(middleware_lib INTERFACE)
target_include_directories(middleware_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(middleware_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/middleware.c)
target_link_libraries(middleware_lib INTERFACE
        common_lib
        qxi_interface
        gpio_interface
        enV5_hw_controller)
