add_library(http_lib_dummy INTERFACE)
target_include_directories(http_lib_dummy INTERFACE
        ${CMAKE_SOURCE_DIR}/src/network/http/include
        ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(http_lib_dummy INTERFACE ${CMAKE_CURRENT_LIST_DIR}/HTTP.c)
target_link_libraries(http_lib_dummy INTERFACE
        common_lib
        CException)
