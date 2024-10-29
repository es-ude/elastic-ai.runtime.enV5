include(FetchContent)

FetchContent_Declare(
        bmi323
        GIT_REPOSITORY https://github.com/boschsensortec/BMI323_SensorAPI.git
        GIT_TAG v2.1.0
)
FetchContent_Populate(bmi323)

add_library(__external_bmi323_bosch_lib__impl
        "${bmi323_SOURCE_DIR}/bmi3.c"
        "${bmi323_SOURCE_DIR}/bmi323.c")

add_library(__external_bmi323_bosch_lib__hdrs INTERFACE)
target_include_directories(__external_bmi323_bosch_lib__hdrs INTERFACE "${bmi323_SOURCE_DIR}")
target_link_libraries(__external_bmi323_bosch_lib__impl PRIVATE __external_bmi323_bosch_lib__hdrs)
add_library(__external_bmi323_bosch_lib INTERFACE)
target_link_libraries(__external_bmi323_bosch_lib INTERFACE __external_bmi323_bosch_lib__hdrs __external_bmi323_bosch_lib__impl)

add_elastic_ai_lib(
        HW_ONLY
        NAME Bmi323
        SRCS Bmi323.c
        DEPS
            __external_bmi323_bosch_lib
            Common
            Pico::pico_stdlib
            CException
            Sleep
            Gpio
            Spi
)

target_link_libraries(Bmi323__hdrs INTERFACE __external_bmi323_bosch_lib__hdrs)

