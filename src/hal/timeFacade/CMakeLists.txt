# Provides a Facade for the pico-sdk Time Library

add_library(time_facade_interface INTERFACE)
target_sources(time_facade_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/TimeFacade.c)
target_include_directories(time_facade_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(time_facade_interface INTERFACE
        common_lib
        pico_time)