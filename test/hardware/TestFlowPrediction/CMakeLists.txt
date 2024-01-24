# Hardwareflow_prediction stub test
add_library(flow_prediction_lib INTERFACE)
target_sources(flow_prediction_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/flow_prediction.c)
target_include_directories(flow_prediction_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(flow_prediction_lib INTERFACE middleware_v1_lib)

add_executable(hardware-test_flow_prediction HardwaretestStub.c)
target_link_libraries(hardware-test_flow_prediction
        common_lib
        pico_stdlib
        hardware_spi
        flash_lib
        enV5_hwc
        fpga_configuration_handler
        middleware_v1_lib
        flow_prediction_lib)
create_enV5_executable(hardware-test_flow_prediction)

