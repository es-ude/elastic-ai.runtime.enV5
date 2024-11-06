##############################################
## region hardware-test_sht3

add_elastic_ai_hardware_test(
        NAME HardwaretestSht3x
        SRCS HardwaretestSht3x.c
        DEPS
            Pico::pico_stdlib
            Pico::pico_bootrom
            Pico::pico_stdio_usb
            Pico::hardware_i2c
            EnV5HwController
            Common
            EnV5HwConfiguration
            I2c
            Sht3x
            CException
)

#############################################
# region hardware-test_adxl345b

add_elastic_ai_hardware_test(
        NAME HardwaretestAdxl345b
        SRCS HardwaretestAdxl345b.c
        DEPS
            Common
            Pico::pico_stdlib
            Pico::pico_stdio_usb
            Pico::pico_bootrom
            Pico::hardware_i2c
            I2c
            Adxl345b
            Time
            Pico::pico_time
)



#add_executable(hardware-test_adxl345b HardwaretestAdxl345b.c)
#target_link_libraries(hardware-test_adxl345b
#        Common
#        Pico::pico_stdlib
#        Pico::pico_stdio_usb
#
#        Pico::pico_bootrom
#        I2c
#        Adxl345b
#        Time
#        Pico::pico_time
#
#)
#create_enV5_executable(hardware-test_adxl345b)
## endregion hardware-test_adxl345b
###############################################
### hardware-test_pac193x
add_executable(hardware-test_pac193x HardwaretestPac193x.c)


add_library(_HWTestLibForPac139x INTERFACE)
target_link_libraries( _HWTestLibForPac139x INTERFACE
        Common
        Pico::pico_stdlib
        Pico::pico_stdio_usb
        Pico::pico_bootrom
        Pico::pico_time
        I2c
        EnV5HwConfiguration
        Pac193x
)

target_link_libraries(hardware-test_pac193x _HWTestLibForPac139x)
create_enV5_executable(hardware-test_pac193x)
###############################################
## hardware-test_pac193x_stream
add_executable(hardware-test_pac193x_stream HardwaretestPac193xAccumulator.c)
target_link_libraries(hardware-test_pac193x_stream _HWTestLibForPac139x)
create_enV5_executable(hardware-test_pac193x_stream)
##############################################
## hardware-test_dualpac193x
add_executable(hardware-test_dualpac193x HardwaretestDualPac193x.c)
target_link_libraries(hardware-test_dualpac193x _HWTestLibForPac139x)

create_enV5_executable(hardware-test_dualpac193x)
##############################################
### hardware-test_pac193xbuffer
add_executable(hardware-test_pac193xbuffer HardwaretestPac193xBuffer.c)
target_link_libraries(hardware-test_pac193xbuffer _HWTestLibForPac139x)
create_enV5_executable(hardware-test_pac193xbuffer)
##############################################
add_executable(hardware-test_bmi323 HardwaretestBmi323.c)
target_link_libraries(hardware-test_bmi323
        Pico::pico_stdlib
        Pico::hardware_spi

        CException
        Common
        EnV5HwConfiguration
        EnV5HwController
        Spi
        Sleep
        Bmi323

)
create_enV5_executable(hardware-test_bmi323)
### hardware-stresstest_adxl345b
#add_executable(hardware-test_stressTest_adxl345b StressTestAdxl345b.c)
#target_link_libraries(hardware-test_stressTest_adxl345b
#        Pico::common_lib
#        Pico::pico_stdlib
#        Pico::pico_stdio_usb
#        Pico::pico_time
#        freeRtosUtils
#        espBroker_lib
#        middleware_lib
#        sensor_lib_adxl345b
#        hardware-testHelper)
#create_enV5_executable(hardware-test_stressTest_adxl345b)
##############################################
