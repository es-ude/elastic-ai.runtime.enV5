add_elastic_ai_lib(
        NAME Http
        SRCS HTTP.c
        DEPS
            Common
            CException
            AtCommands
            Esp
)
