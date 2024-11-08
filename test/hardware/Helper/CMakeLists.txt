add_elastic_ai_lib(
        NAME HardwaretestHelper
        SRCS HardwaretestHelper.c
        DEPS
            Common
            Pico::pico_stdlib
            Pico::pico_bootrom

            MqttBroker
            Network
            freeRtosUtils
            EnV5HwController
            Esp
)
