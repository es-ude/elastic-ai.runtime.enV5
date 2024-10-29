add_elastic_ai_lib(
        HW_ONLY
        NAME EnV5HwController
        SRCS EnV5HwController.c
        DEPS Common EnV5HwConfiguration Gpio Sleep
)

