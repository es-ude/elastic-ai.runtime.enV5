##############################################
## ADD EXECUTABLE: freeRTOS_task
# creates executable
add_executable(freeRTOS_task test_task.c)
# add libraries
target_include_directories(freeRTOS_task PUBLIC
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(freeRTOS_task
        pico_stdlib
        FreeRTOSUtils)
# enable usb output, disable uart output
pico_enable_stdio_usb(freeRTOS_task 1)
pico_enable_stdio_uart(freeRTOS_task 0)
# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(freeRTOS_task)

##############################################
## ADD EXECUTABLE: freeRTOS_queues
# creates executable
add_executable(freeRTOS_queues test_queues.c)
# add libraries
target_include_directories(freeRTOS_queues PUBLIC
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(freeRTOS_queues
        pico_stdlib
        esp_lib
        FreeRTOSUtils)
# enable usb output, disable uart output
pico_enable_stdio_usb(freeRTOS_queues 1)
pico_enable_stdio_uart(freeRTOS_queues 0)
# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(freeRTOS_queues)
