include(FetchContent)

FetchContent_Declare(
        bmi323
        GIT_REPOSITORY https://github.com/boschsensortec/BMI323_SensorAPI.git
        GIT_TAG v2.1.0
)
FetchContent_Populate(bmi323)

add_library(__external_bmi323_bosch_lib
        "${bmi323_SOURCE_DIR}/bmi3.c"
        "${bmi323_SOURCE_DIR}/bmi323.c")

target_include_directories(__external_bmi323_bosch_lib PUBLIC "${bmi323_SOURCE_DIR}")

add_elastic_ai_lib(
        HW_ONLY
        NAME Bmi323
        SRCS Bmi323.c
        DEPS
            __external_bmi323_bosch_lib
            Pico::pico_stdlib
            CException
            Sleep
            Gpio
            Spi
)