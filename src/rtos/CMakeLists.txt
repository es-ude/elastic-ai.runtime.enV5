add_elastic_ai_lib(
        HW_ONLY
        NAME freeRtosUtils
        SRCS FreeRtosMutexWrapper.c FreeRtosQueueWrapper.c FreeRtosSMP.c FreeRtosTaskWrapper.c
        DEPS FreeRTOS-Kernel FreeRTOS-Kernel-Heap3 Common Sleep
)