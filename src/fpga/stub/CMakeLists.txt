set(MIDDLEWARE_STUB_VERSION CACHE STRING "1" "use stub version 1, compatible with skeleton version 1")
set(MIDDLEWARE_STUB_VERSION CACHE STRING "2" "use stub version 2, compatible with skeleton version 2")

add_elastic_ai_lib(
        NAME Stub
        SRCS stub.c
        DEPS Common Sleep Middleware
)