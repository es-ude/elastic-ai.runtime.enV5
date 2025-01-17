################## test_freeRTOSTask ##################
add_executable(HardwareTestFreeRtosTask HardwareTestFreeRtosTask.c)
target_link_libraries(HardwareTestFreeRtosTask
        Common
        EnV5HwConfiguration
        EnV5HwController
        Gpio
        freeRtosUtils
        Pico::hardware_watchdog
        Pico::pico_bootrom
        Pico::pico_stdlib
)
create_enV5_executable(HardwareTestFreeRtosTask)

################## test_freeRTOSQueues ##################
add_executable(HardwareTestFreeRtosQueues HardwareTestFreeRtosQueues.c)
target_link_libraries(HardwareTestFreeRtosQueues
        Common
        EnV5HwController
        freeRtosUtils

        freeRtos::FreeRTOS-Kernel
        Pico::pico_stdlib
        Pico::hardware_spi
)
create_enV5_executable(HardwareTestFreeRtosQueues)

################## test_SMP ##################
add_executable(HardwareTestSMP HardwareTestSMP.c)
target_link_libraries(HardwareTestSMP
        Common
        EnV5HwController
        freeRtosUtils

        freeRtos::FreeRTOS-Kernel
        Pico::pico_stdlib
        Pico::pico_bootrom

)
target_compile_definitions(HardwareTestSMP PRIVATE DEBUG)
create_enV5_executable(HardwareTestSMP)
