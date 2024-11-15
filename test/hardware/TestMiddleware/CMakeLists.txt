add_executable(HardwareTestMiddleware HardwareTestMiddleware.c)
target_link_libraries(HardwareTestMiddleware
        Common
        EnV5HwConfiguration
        EnV5HwController
        Esp
        Flash
        FpgaConfigurationHandler
        Http
        Network
        Middleware
        Pico::pico_stdio
        Pico::pico_stdlib
        Pico::hardware_spi
)
target_compile_definitions(HardwareTestMiddleware PRIVATE DEBUG)
create_enV5_executable(HardwareTestMiddleware)
