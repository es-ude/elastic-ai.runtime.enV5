add_elastic_ai_lib(
        NAME HttpDummy
        SRCS HTTP.c
        DEPS
            CException
            Http
)
