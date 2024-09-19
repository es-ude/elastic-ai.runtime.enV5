add_library(time_facade_interface INTERFACE)
target_include_directories(time_facade_interface INTERFACE ${CMAKE_SOURCE_DIR}/src/hal/timeFacade/include)
target_sources(time_facade_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/TimeFacade.c)
target_link_libraries(time_facade_interface INTERFACE
        common_lib)
