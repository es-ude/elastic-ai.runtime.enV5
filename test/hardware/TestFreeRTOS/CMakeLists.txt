################## test_freeRTOSTask ##################
add_executable(hardware-test_freeRTOSTask HardwaretestFreeRtosTask.c)
target_link_libraries(hardware-test_freeRTOSTask
        pico_stdlib
        hardware_watchdog
        common_lib
        enV5_hw_controller
        enV5_hw_configuration
        gpio_interface
        freeRtosUtils)
create_enV5_executable(hardware-test_freeRTOSTask)

################## test_freeRTOSQueues ##################
add_executable(hardware-test_freeRTOSQueues HardwaretestFreeRtosQueues.c)
target_link_libraries(hardware-test_freeRTOSQueues
        pico_stdlib
        hardware_watchdog
        common_lib
        enV5_hw_controller
        freeRtosUtils)
create_enV5_executable(hardware-test_freeRTOSQueues)

################## test_SMP ##################
add_executable(hardware-test_SMP HardwaretestSMP.c)
target_link_libraries(hardware-test_SMP
        pico_stdlib
        hardware_watchdog
        common_lib
        enV5_hw_controller
        freeRtosUtils)
target_compile_definitions(hardware-test_SMP PRIVATE DEBUG)
create_enV5_executable(hardware-test_SMP)
