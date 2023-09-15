add_library(fpga_config_lib INTERFACE)
target_sources(fpga_config_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FpgaConfiguration.c
        ${CMAKE_CURRENT_LIST_DIR}/FpgaConfigDatahandler.c
        ${CMAKE_CURRENT_LIST_DIR}/FpgaConfigurationHttp.c)
target_include_directories(fpga_config_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(fpga_config_lib INTERFACE
        flash_lib
        network_lib
        http_lib
        pico_stdio
        pico_printf)
