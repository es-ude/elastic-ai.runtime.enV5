add_library(freeRtosUtils INTERFACE)
target_sources(freeRtosUtils INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosTaskWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosSemaphoreWrapper.c)
target_include_directories(freeRtosUtils INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(freeRtosUtils INTERFACE
        common_lib
        sleep_interface)
