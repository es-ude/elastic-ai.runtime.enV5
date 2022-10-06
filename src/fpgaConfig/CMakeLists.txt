add_library(fpga_config_lib INTERFACE)
target_sources(fpga_config_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/FpgaConfiguration.c
        ${CMAKE_CURRENT_LIST_DIR}/FpgaConfigDatahandler.c
        )
target_include_directories(fpga_config_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(fpga_config_lib INTERFACE
        flash_lib
        pico_stdlib
        )