add_library(env5_hal INTERFACE)
target_sources(env5_hal INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Env5Hw.c)
target_include_directories(env5_hal INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(env5_hal INTERFACE pico_stdlib)