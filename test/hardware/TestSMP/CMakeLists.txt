################## test_MQTTPublish ##################
add_executable(hardware-test_SMP HardwaretestSMP.c)
target_link_libraries(hardware-test_SMP
        pico_stdlib
        FreeRTOS-Kernel-Heap3
        freeRtosUtils)
target_compile_definitions(hardware-test_SMP PRIVATE DEBUG)
make_to_output_file(hardware-test_SMP)
