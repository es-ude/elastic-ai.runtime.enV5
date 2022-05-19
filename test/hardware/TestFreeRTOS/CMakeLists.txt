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
make_to_output_file(freeRTOS_task)

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
make_to_output_file(freeRTOS_queues)
