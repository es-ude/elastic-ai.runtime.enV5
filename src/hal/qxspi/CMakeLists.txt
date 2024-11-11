# Provides a Facade for the SPI library
# Placeholder -> Should later be replaced by real Quad SPI implementation

if (BUILDING_FOR_ELASTIC_NODE)
    add_library(Qxi Qxi.c)
    target_include_directories(Qxi PUBLIC include)
    target_link_libraries(Qxi PRIVATE Gpio Spi Pico::hardware_spi )
else ()
    add_library(Qxi INTERFACE)
    target_link_libraries(Qxi INTERFACE include)
endif ()