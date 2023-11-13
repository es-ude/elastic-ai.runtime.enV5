add_library(fpga_configuration_handler INTERFACE)
target_include_directories(fpga_configuration_handler INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(fpga_configuration_handler INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FpgaConfigurationHandler.c
        ${CMAKE_CURRENT_LIST_DIR}/FpgaConfigurationHandlerHTTP.c
        ${CMAKE_CURRENT_LIST_DIR}/FpgaConfigurationHandlerUSB.c)
target_link_libraries(fpga_configuration_handler INTERFACE
        common_lib
        flash_lib
        middleware_lib
        network_lib
        http_lib
        pico_stdio)
