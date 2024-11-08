# region Sht3
add_elastic_ai_hardware_test(
        HW_ONLY
        NAME HardwaretestSht3x
        SRCS HardwaretestSht3x.c
        DEPS
            Pico::pico_stdlib
            Pico::pico_stdio
            Pico::pico_bootrom
            Pico::hardware_i2c
            EnV5HwController
            Common
            EnV5HwConfiguration
            I2c
            Sht3x
            CException
            Gpio
            Sleep
)
# endregion hardware-test_sht3

# region Adxl345b
add_elastic_ai_hardware_test(
        NAME HardwaretestAdxl345b
        SRCS HardwaretestAdxl345b.c
        DEPS
            Common
            Pico::pico_stdlib
            Pico::pico_stdio_usb
            Pico::pico_bootrom
            Pico::hardware_i2c
            Pico::pico_time
            I2c
            Adxl345b
            Time
            Sleep
            Gpio
)
# endregion hardware-test_adxl345b

# region Pac193x
add_elastic_ai_hardware_test(
        NAME HardwaretestPac193x
        SRCS HardwaretestPac193x.c
        DEPS
            Common
            Pico::pico_stdlib
            Pico::pico_stdio_usb
            Pico::pico_bootrom
            Pico::pico_time
            Pico::hardware_i2c
            I2c
            EnV5HwConfiguration
            Pac193x
            Gpio
            Sleep
)
# endregion hardware-test_pac193x

# region Pac193xAccumulator
add_elastic_ai_hardware_test(
        NAME HardwaretestPac193xAccumulator
        SRCS HardwaretestPac193xAccumulator.c
        DEPS
        Common
        Pico::pico_stdlib
        Pico::pico_stdio_usb
        Pico::pico_bootrom
        Pico::pico_time
        Pico::hardware_i2c
        I2c
        EnV5HwConfiguration
        EnV5HwController
        Pac193x
        Gpio
        Sleep
)
# endregion hardware-test_pac193x_stream

# region Dualpac193x
add_elastic_ai_hardware_test(
        NAME HardwaretestDualPac193x
        SRCS HardwaretestDualPac193x.c
        DEPS
        Common
        Pico::pico_stdlib
        Pico::pico_stdio_usb
        Pico::pico_bootrom
        Pico::pico_time
        Pico::hardware_i2c
        I2c
        EnV5HwConfiguration
        Pac193x
        Gpio
        Sleep
)
# endregion hardware-test_dualpac193x

# region Pac193xBuffer
add_elastic_ai_hardware_test(
        NAME HardwaretestPac193xBuffer
        SRCS HardwaretestPac193xBuffer.c
        DEPS
        Common
        Pico::pico_stdlib
        Pico::pico_stdio_usb
        Pico::pico_bootrom
        Pico::pico_time
        Pico::hardware_i2c
        I2c
        EnV5HwConfiguration
        Pac193x
        Gpio
        Sleep
)
# endregion hardware-test_pac193xbuffer

# region Bmi323
add_elastic_ai_hardware_test(
        HW_ONLY
        NAME HardwaretestBmi323
        SRCS HardwaretestBmi323.c
        DEPS
            Pico::pico_stdlib
            Pico::hardware_spi
            CException
            Common
            EnV5HwConfiguration
            EnV5HwController
            Spi
            Sleep
            Bmi323
            Gpio
            external_bmi323_bosch_lib
)
# endregion Harwaretest_Bmi323

# FIX
# region StressTest_Adxl345b

add_library(A__impl_with_deps INTERFACE)
target_link_libraries(A__impl_with_deps INTERFACE A_impl B__impl_with_deps)


add_elastic_ai_hardware_test(
        NAME StressTestAdxl345b
        SRCS StressTestAdxl345b.c
        DEPS
            Pico::pico_stdlib
            Pico::pico_stdio
            Pico::pico_bootrom
            Pico::hardware_i2c

            Adxl345b
            Common
            EnV5HwConfiguration
            EnV5HwController
            Esp
            freeRtosUtils
            HardwaretestHelper
            I2c
            Sleep
            Gpio
            Time
            Uart
            MqttBroker
            CException
            Network
            NetworkConfig
            Http

            freeRtos::FreeRTOS-Kernel
            freeRtos::FreeRTOS-Kernel-Heap3

            RuntimeC::TopicMatcher
            RuntimeC::Protocol
)
# endregion hardware-stresstest_adxl345b

