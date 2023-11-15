################## test_freeRTOSTask ##################
add_executable(hardware-test_freeRTOSTask HardwaretestFreeRtosTask.c)
target_link_libraries(hardware-test_freeRTOSTask
        common_lib
        pico_stdlib
        freeRtosUtils)
create_enV5_executable(hardware-test_freeRTOSTask)

################## test_freeRTOSQueues ##################
add_executable(hardware-test_freeRTOSQueues HardwaretestFreeRtosQueues.c)
target_link_libraries(hardware-test_freeRTOSQueues
        common_lib
        pico_stdlib
        freeRtosUtils)
create_enV5_executable(hardware-test_freeRTOSQueues)

################## test_SMP ##################
add_executable(hardware-test_SMP HardwaretestSMP.c)
target_link_libraries(hardware-test_SMP
        common_lib
        pico_stdlib
        freeRtosUtils)
target_compile_definitions(hardware-test_SMP PRIVATE DEBUG)
create_enV5_executable(hardware-test_SMP)

