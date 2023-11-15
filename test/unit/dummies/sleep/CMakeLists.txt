add_library(sleep_interface INTERFACE)
target_include_directories(sleep_interface INTERFACE ${CMAKE_SOURCE_DIR}/src/hal/sleep/include)
target_sources(sleep_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Sleep.c)
target_link_libraries(sleep_interface INTERFACE
        common_lib)
