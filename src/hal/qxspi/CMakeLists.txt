# Provides a Facade for the SPI library
# Placeholder -> Should later be replaced by real Quad SPI implementation

add_library(qxi_interface INTERFACE)
target_include_directories(qxi_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(qxi_interface INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Qxi.c)
target_link_libraries(qxi_interface INTERFACE
        gpio_interface
        spi_interface
        hardware_spi)