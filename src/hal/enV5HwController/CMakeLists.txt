add_library(enV5_hw_controller INTERFACE)
target_include_directories(enV5_hw_controller INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(enV5_hw_controller INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/EnV5HwController.c
)
target_link_libraries(enV5_hw_controller INTERFACE
        common_lib
        enV5_hw_configuration
        gpio_interface
        i2c_interface
        spi_interface
        uart_interface
        sleep_interface
        flash_lib
        sensor_lib_adxl345b
        sensor_lib_pac193x
        sensor_lib_sht3x
)
