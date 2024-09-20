add_library(time_interface INTERFACE)
target_include_directories(time_interface INTERFACE ${CMAKE_SOURCE_DIR}/src/hal/time/include)
target_sources(time_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Time.c)
target_link_libraries(time_interface INTERFACE
        common_lib)
