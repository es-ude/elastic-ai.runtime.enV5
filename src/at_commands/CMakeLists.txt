add_library(at_commands INTERFACE)
target_sources(at_commands INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/at_commands.c)
target_include_directories(at_commands INTERFACE ${CMAKE_CURRENT_LIST_DIR})