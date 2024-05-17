add_library(enV5_hwc INTERFACE)
target_include_directories(enV5_hwc INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(enV5_hwc INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/EnV5HwController.c
        ${CMAKE_CURRENT_LIST_DIR}/Config.c
        )
target_link_libraries(enV5_hwc INTERFACE
        common_lib
        gpio_interface
        spi_interface
        uart_interface
        i2c_interface
        hardware_i2c
        sensor_lib_adxl345b
        sensor_lib_pac193x
        sensor_lib_sht3x
        sleep_interface
)
