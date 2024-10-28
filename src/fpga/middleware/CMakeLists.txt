add_elastic_ai_lib(
        NAME Middleware
        SRCS middleware.c
        DEPS Common Qxi Gpio Env5HwController
)