##############################################
## region UNIT-TEST: unit-test_sht3
add_subdirectory(dummies/http)
add_subdirectory(dummies/flash)
add_subdirectory(dummies/i2c)
add_subdirectory(dummies/gpio)
add_subdirectory(dummies/sleep)
add_subdirectory(dummies/esp)
add_subdirectory(dummies/rtos)


add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Sht3x
        MORE_SOURCES
            ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/I2c.c
        MORE_LIBS
            CException I2c__hdrs
)
target_include_directories(UnitTestSht3x PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dummies/i2c/include)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            FpgaConfigurationHandler
        MORE_LIBS
            CException
            Common
            Flash__hdrs
            FlashDummy
            HttpDummy
            Http__hdrs
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
            Esp__hdrs
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
            Esp__hdrs
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            MqttBroker
        MORE_LIBS
            CException
            Common
            HttpDummy
            EspDummy
            Esp__hdrs
            topicMatcher
            NetworkConfig
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
            Adxl345b
        MORE_LIBS
            CException
            Common
            Sleep__hdrs
            Time__hdrs
            I2c__hdrs
)

add_custom_target(all_unit_tests)

add_dependencies(all_unit_tests
        UnitTestSht3x
        UnitTestFpgaConfigurationHandler
        UnitTestPac193x
        UnitTestNetwork
        UnitTestHttp
        UnitTestMqttBroker
        UnitTestAdxl345b
)