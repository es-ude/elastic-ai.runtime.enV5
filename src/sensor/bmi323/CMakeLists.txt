include(FetchContent)

FetchContent_Declare(
        bmi323
        GIT_REPOSITORY https://github.com/boschsensortec/BMI323_SensorAPI.git
        GIT_TAG v2.1.0
)
FetchContent_Populate(bmi323)

add_library(sensor_lib_bmi323 STATIC
        "${bmi323_SOURCE_DIR}/bmi3.c"
        "${bmi323_SOURCE_DIR}/bmi323.c"
        "${CMAKE_CURRENT_LIST_DIR}/Bmi323.c"
)
target_include_directories(sensor_lib_bmi323 PUBLIC
        "${bmi323_SOURCE_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/include"
)
target_link_libraries(sensor_lib_bmi323 PUBLIC
        pico_stdlib
        CException
)
target_link_libraries(sensor_lib_bmi323 PRIVATE
        sleep_interface
        gpio_interface
        spi_interface
)
