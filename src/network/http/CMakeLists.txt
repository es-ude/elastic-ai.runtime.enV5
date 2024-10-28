add_elastic_ai_lib(
        NAME Http
        SOURCES HTTP.c
        DEPS
            Common
            CException
            AtCommands
            Esp
)
