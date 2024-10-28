add_elastic_ai_lib(
        HW_ONLY
        NAME UsbProtocol
        SRCS Protocol.c DefaultCommands.c Tools.c
        DEPS
            EnV5HwController
            EnV5HwConfiguration
            CException
            Pico::hardware_spi
            Gpio
            Flash
            Middleware
            Stub
            Sleep
)
