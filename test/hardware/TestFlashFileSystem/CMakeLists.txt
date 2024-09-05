add_executable(demo_file_system DemoFileSystem.c)
target_link_libraries(demo_file_system
        common_lib
        pico_stdlib
        spi_interface
        flash_lib
        sleep_interface
        enV5_hw_controller
        enV5_hw_configuration
        fpga_configuration_handler
        filesystem_lib)
create_enV5_executable(demo_file_system)
