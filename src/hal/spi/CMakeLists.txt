# Provides a Facade for the pico-sdk SPI library

if(_BUILDING_FOR_HARDWARE)
  add_library(Spi Spi.c)
  target_include_directories(Spi PUBLIC include)
  target_link_libraries(Spi PRIVATE
    Common
    Gpio
    Pico::hardware_spi
    Pico::pico_runtime
  )
else()
  add_library(Spi INTERFACE)
  target_include_directories(Spi INTERFACE include)
endif()
