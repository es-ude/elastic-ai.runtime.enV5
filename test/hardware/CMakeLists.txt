################## hardwareTestHelper ##################
add_library(hardware-testHelper hardwareTestHelper.c)
target_link_libraries(hardware-testHelper
        pico_stdlib
        esp_lib
        espBroker_lib
        network_lib
        FreeRTOSUtils)
target_compile_definitions(hardware-testHelper PRIVATE DEBUG)
pico_enable_stdio_usb(hardware-testHelper 1)
pico_enable_stdio_uart(hardware-testHelper 0)

add_subdirectory(TestFreeRTOS)
add_subdirectory(TestNetworking)
add_subdirectory(Sensors)
