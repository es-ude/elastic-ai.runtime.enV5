## Test with HTTP Dummy Server
add_executable(HardwareTestStub HardwareTestStub.c)
target_link_libraries(HardwareTestStub
        Common
        EnV5HwController
        EnV5HwConfiguration
        Esp
        Flash
        FpgaConfigurationHandler
        Http
        Network
        Stub

        Pico::pico_stdlib
        Pico::hardware_spi
)
target_compile_definitions(HardwareTestStub PRIVATE DEBUG)
create_enV5_executable(HardwareTestStub)

## Test in combination with Monitor
add_executable(HardwareTestStubWithMonitor HardwareTestStubWithMonitor.c)
target_link_libraries(HardwareTestStubWithMonitor
        Common
        EnV5HwConfiguration
        EnV5HwController
        Esp
        FpgaConfigurationHandler
        freeRtosUtils
        MqttBroker
        Network
        Middleware
        Flash

        freeRtos::FreeRTOS-Kernel
        Pico::pico_stdlib
        RuntimeC::Protocol
)
target_compile_definitions(HardwareTestStubWithMonitor PRIVATE DEBUG)
create_enV5_executable(HardwareTestStubWithMonitor)
