add_executable(CheckEspForOkViaPico CheckEspForOkViaPico.c)
target_link_libraries(CheckEspForOkViaPico PRIVATE
        Pico::pico_stdlib
        Pico::hardware_uart
        Pico::hardware_gpio
        unity
)
create_enV5_executable(CheckEspForOkViaPico)

add_executable(HardwareTestEsp HardwareTestEsp.c)
target_link_libraries(HardwareTestEsp PRIVATE
        Pico::pico_stdlib
        Pico::hardware_uart
        Pico::hardware_gpio
)
create_env5_executable(HardwareTestEsp)

add_executable(HardwareTestEspWithUartLib HardwareTestEspWithUartLib.c)
target_link_libraries(HardwareTestEspWithUartLib PRIVATE
        Uart
        EnV5HwConfiguration
        Pico::pico_stdlib
)
create_env5_executable(HardwareTestEspWithUartLib)