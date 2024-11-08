add_elastic_ai_lib(
        HW_ONLY
        NAME freeRtosUtils
        SRCS FreeRtosMutexWrapper.c FreeRtosQueueWrapper.c FreeRtosSMP.c FreeRtosTaskWrapper.c
        DEPS

            Common

            Sleep
            Pico::pico_bootrom
            Pico::hardware_base
)

target_link_libraries(freeRtosUtils__impl PRIVATE freeRtos::FreeRTOS-Kernel
        freeRtos::FreeRTOS-Kernel-Heap3)