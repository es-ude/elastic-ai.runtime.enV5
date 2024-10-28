add_elastic_ai_lib(
        NAME FpgaConfigurationHandler
        SRCS FpgaConfigurationHandler.c FpgaConfigurationHandlerHTTP.c
        DEPS Common Http CException EnV5HwConfiguration Flash
)

add_elastic_ai_lib(
        HW_ONLY
        NAME FpgaConfigurationHandlerUSB
        SRCS FpgaConfigurationHandlerUSB.c
        DEPS Common Http CException Env5HWConfiguration Flash
)

