
# Hardwareflow_prediction stub test
add_library(flow_prediction_lib INTERFACE)
target_sources(flow_prediction_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/flow_prediction.c)
target_include_directories(flow_prediction_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(flow_prediction_lib INTERFACE middleware_lib)

add_executable(hardware-test_flow_prediction HardwaretestStub.c)
target_link_libraries(hardware-test_flow_prediction
        pico_stdlib
        hardware_spi
        flash_lib
        enV5_hwc
        fpga_configuration_handler
        middleware_lib
        flow_prediction_lib)

# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_flow_prediction 1)
pico_enable_stdio_uart(hardware-test_flow_prediction 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_flow_prediction)

target_compile_definitions(hardware-test_flow_prediction PRIVATE DEBUG)
make_to_output_file(hardware-test_flow_prediction)

