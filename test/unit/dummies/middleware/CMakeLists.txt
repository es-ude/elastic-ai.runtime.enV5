add_library(middleware_lib INTERFACE)
target_include_directories(middleware_lib INTERFACE ${CMAKE_SOURCE_DIR}/src/fpga/middleware/include)
target_sources(middleware_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/middleware.c)
target_link_libraries(middleware_lib INTERFACE
        common_lib)
