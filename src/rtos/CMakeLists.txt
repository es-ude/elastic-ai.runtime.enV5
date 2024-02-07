add_library(freeRtosUtils INTERFACE)
target_include_directories(freeRtosUtils INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(freeRtosUtils INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosTaskWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosQueueWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosSemaphoreWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosSMP.c)
target_link_libraries(freeRtosUtils INTERFACE
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap3
        common_lib
        sleep_interface)
