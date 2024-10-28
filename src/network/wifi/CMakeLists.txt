

add_elastic_ai_lib(
        NAME Network
        SRCS Network.c
        DEPS
            freeRtosUtils
            Common
            Uart
            Esp
            Http
            NetworkConfig
            AtCommands
)