add_elastic_ai_lib(
        NAME RtosDummy
        SRCS FreeRtosTaskWrapper.c
        DEPS freeRtosUtils Sleep Common
)