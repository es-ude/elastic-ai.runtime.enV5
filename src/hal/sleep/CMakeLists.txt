# Provides a Facade for the pico-sdk Time Library

add_library(sleep_interface INTERFACE)
target_sources(sleep_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Sleep.c)
target_include_directories(sleep_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(sleep_interface INTERFACE
        common_lib
        pico_time)
