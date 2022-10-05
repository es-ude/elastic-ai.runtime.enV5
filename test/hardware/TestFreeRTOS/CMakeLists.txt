################## test_freeRTOSTask ##################
add_executable(hardware-test_freeRTOSTask test_task.c)
target_include_directories(hardware-test_freeRTOSTask PUBLIC
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(hardware-test_freeRTOSTask
        pico_stdlib
        FreeRTOSUtils)
make_to_output_file(hardware-test_freeRTOSTask)

################## test_freeRTOSQueues ##################
add_executable(hardware-test_freeRTOSQueues test_queues.c)
target_include_directories(hardware-test_freeRTOSQueues PUBLIC
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(hardware-test_freeRTOSQueues
        pico_stdlib
        esp_lib
        FreeRTOSUtils)
make_to_output_file(hardware-test_freeRTOSQueues)
