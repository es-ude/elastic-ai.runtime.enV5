add_library(env5_lib INTERFACE)
target_sources(env5_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/env5_hw.c)
target_include_directories(env5_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(env5_lib INTERFACE pico_stdlib)