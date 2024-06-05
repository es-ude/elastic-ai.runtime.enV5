add_library(enV5_hw_configuration_rev_one INTERFACE)
target_include_directories(enV5_hw_configuration_rev_one INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(enV5_hw_configuration_rev_one INTERFACE
        hardware_i2c
        hardware_uart
        hardware_spi
)