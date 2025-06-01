##############################################
## region UNIT-TEST: unit-test_sht3
add_subdirectory(dummies)
include(unit_test.cmake)

add_library(UnitTestCommon INTERFACE)
target_link_libraries(UnitTestCommon INTERFACE
        CException
        Common
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Sht3x
        MORE_LIBS
            UnitTestCommon
            I2cDummy
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            FpgaConfigurationHandler
        MORE_LIBS
            UnitTestCommon
            FlashDummy
            HttpDummy
)


add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Pac193x
        MORE_LIBS
            Common
            I2cDummy
            GpioDummy
            SleepDummy
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Network
        MORE_LIBS
            Common
            HttpDummy
            RtosDummy
            EspDummy
            CException
            NetworkConfig
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Http
        MORE_LIBS
            CException
            Common
            EspDummy
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            MqttBroker
        MORE_LIBS
            CException
            Common
            HttpDummy
            EspDummy
            topicMatcher
            protocol
            NetworkConfig
            m
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Adxl345b
        MORE_LIBS
            CException
            Common
            SleepDummy
            I2cDummy
            TimeDummy
            m
)

