# Provides a Facade for the SPI library
# Placeholder -> Should later be replaced by real Quad SPI implementation

add_elastic_ai_lib(
        HW_ONLY
        NAME Qxi
        SRCS ${CMAKE_CURRENT_LIST_DIR}/Qxi.c
        DEPS Common Gpio Spi Pico::hardware_spi
)