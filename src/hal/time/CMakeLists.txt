# Provides a Facade for the pico-sdk Time Library

add_library(time_interface INTERFACE)
target_sources(time_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Time.c)
target_include_directories(time_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(time_interface INTERFACE
        common_lib
        pico_time)