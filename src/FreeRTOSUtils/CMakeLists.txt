add_library(FreeRTOSUtils INTERFACE)
target_sources(FreeRTOSUtils INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/TaskWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/QueueWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/SemaphoreWrapper.c)
target_include_directories(FreeRTOSUtils INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(FreeRTOSUtils INTERFACE
        pico_stdlib
        freertos)
