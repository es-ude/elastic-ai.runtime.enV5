add_library(freeRtosUtils INTERFACE)
target_sources(freeRtosUtils INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosTaskWrapper.c)
target_include_directories(freeRtosUtils INTERFACE ${CMAKE_SOURCE_DIR}/src/rtos/include)
target_link_libraries(freeRtosUtils INTERFACE
        common_lib
        sleep_interface)
