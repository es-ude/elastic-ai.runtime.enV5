#add_library(FreeRTOS
#        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/include/FreeRTOS.h)

add_library(freeRtosUtils INTERFACE
        FreeRTOSConfig.h)
target_sources(freeRtosUtils INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosTaskWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosQueueWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosSemaphoreWrapper.c
        ${CMAKE_CURRENT_LIST_DIR}/FreeRtosSMP.c
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/portable/ThirdParty/GCC/RP2040/include/rp2040_config.h
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/include/task.h
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/list.c
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/queue.c
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/tasks.c
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/timers.c
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/event_groups.c
        )
target_include_directories(freeRtosUtils INTERFACE ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/include
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/portable/ThirdParty/GCC/RP2040
        ${CMAKE_SOURCE_DIR}/extern/freeRTOS/Kernel/portable/ThirdParty/GCC/RP2040/include
)
target_link_libraries(freeRtosUtils INTERFACE
        sleep_interface
        FreeRTOS-Kernel
)
