add_library(http_lib INTERFACE)
target_include_directories(http_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(http_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/HTTP.c)
target_link_libraries(http_lib INTERFACE
        CException
        common_lib
        at_commands
        esp_lib)
