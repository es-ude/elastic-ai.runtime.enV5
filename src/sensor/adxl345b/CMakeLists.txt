add_library(sensor_lib_adxl345b INTERFACE)
target_include_directories(sensor_lib_adxl345b INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(sensor_lib_adxl345b INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Adxl345b.c)
target_link_libraries(sensor_lib_adxl345b INTERFACE
        common_lib
        sleep_interface
        time_interface
        i2c_interface
)

add_library(sensor_lib_adxl345b__srcs STATIC )
target_sources(sensor_lib_adxl345b__srcs PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Adxl345b.c)
target_include_directories(sensor_lib_adxl345b__srcs PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(sensor_lib_adxl345b__srcs PRIVATE
    ${CMAKE_SOURCE_DIR}/src/hal/i2c/include
        ${CMAKE_SOURCE_DIR}/src/hal/sleep/include
        ${CMAKE_SOURCE_DIR}/src/hal/time/include
        ${CMAKE_SOURCE_DIR}/src/common/include)
