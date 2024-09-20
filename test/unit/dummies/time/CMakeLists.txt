add_library(time_interface STATIC
        ${CMAKE_CURRENT_LIST_DIR}/Time.c
)
target_include_directories(time_interface PUBLIC
        ${CMAKE_SOURCE_DIR}/src/hal/time/include
)
target_link_libraries(time_interface PRIVATE
        common_lib
)
