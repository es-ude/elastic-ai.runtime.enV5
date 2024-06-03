add_executable(hardware-test_flash HardwaretestFlash.c)
target_link_libraries(hardware-test_flash
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware_spi
        spi_interface
        enV5_hwc
        flash_lib)
create_enV5_executable(hardware-test_flash)

add_executable(hardware-test_flash_read_config HardwareTestFlashReadConfiguration.c)
target_link_libraries(hardware-test_flash_read_config
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware_spi
        spi_interface
        enV5_hwc
        flash_lib)
create_enV5_executable(hardware-test_flash_read_config)
