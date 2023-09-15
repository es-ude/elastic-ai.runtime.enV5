################## test_freeRTOSTask ##################
add_executable(hardware-test_freeRTOSTask HardwaretestFreeRtosTask.c)
target_link_libraries(hardware-test_freeRTOSTask
        pico_stdlib
        freeRtosUtils)
make_to_output_file(hardware-test_freeRTOSTask)

################## test_freeRTOSQueues ##################
add_executable(hardware-test_freeRTOSQueues HardwaretestFreeRtosQueues.c)
target_link_libraries(hardware-test_freeRTOSQueues
        pico_stdlib
        freeRtosUtils)
make_to_output_file(hardware-test_freeRTOSQueues)

################## test_SMP ##################
add_executable(hardware-test_SMP HardwaretestSMP.c)
target_link_libraries(hardware-test_SMP
        pico_stdlib
        freeRtosUtils)
target_compile_definitions(hardware-test_SMP PRIVATE DEBUG)
make_to_output_file(hardware-test_SMP)

