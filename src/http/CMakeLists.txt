add_library(http_lib INTERFACE)
target_sources(http_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/HTTP.c)
target_include_directories(http_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(http_lib INTERFACE
        at_commands
        CException
        esp_lib)
