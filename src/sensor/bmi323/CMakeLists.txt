include(FetchContent)

FetchContent_Declare(
        bmi323
        GIT_REPOSITORY https://github.com/boschsensortec/BMI323_SensorAPI.git
        GIT_TAG v2.1.0
)
FetchContent_Populate(bmi323)

add_library(external_bmi323_bosch_lib
        "${bmi323_SOURCE_DIR}/bmi3.c"
        "${bmi323_SOURCE_DIR}/bmi323.c")

target_include_directories(external_bmi323_bosch_lib PUBLIC "${bmi323_SOURCE_DIR}")



if(BUILDING_FOR_ELASTIC_NODE)
    add_library(Bmi323 Bmi323.c)
    target_include_directories(Bmi323 PUBLIC include)
    target_link_libraries(Bmi323 PRIVATE
            external_bmi323_bosch_lib
            Common
            Pico::pico_stdlib
            CException
            Sleep
            Gpio
            Spi
    )
else()
    add_library(Bmi323 INTERFACE)
    target_include_directories(Bmi323 INTERFACE include)
endif ()

target_link_libraries(Bmi323 INTERFACE external_bmi323_bosch_lib)

