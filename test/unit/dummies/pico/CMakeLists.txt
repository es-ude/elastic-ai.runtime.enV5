add_library(pico_stdio INTERFACE)
target_include_directories(pico_stdio INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(pico_stdio INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/stdio.c)
