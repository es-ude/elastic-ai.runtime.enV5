add_elastic_ai_lib(
        NAME HardwaretestHelper
        SRCS HardwaretestHelper.c
        DEPS
            Common
            Pico::pico_stdlib
            MqttBroker
            Network
            Network
            freeRtosUtils
)
