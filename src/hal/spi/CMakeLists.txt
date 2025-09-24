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

add_library(SpiHal2 INTERFACE)
target_include_directories(SpiHal2 INTERFACE include)


if (_BUILDING_FOR_HARDWARE)
  add_library(SpiPico OBJECT)

  target_sources(SpiPico PRIVATE
    SpiPico.c
  )
  target_link_libraries(SpiPico PRIVATE SpiHal2)

  target_include_directories(SpiPico PRIVATE ${CMAKE_CURRENT_LIST_DIR}/picoImpl)
  target_link_libraries(
    SpiPico
    PRIVATE Gpio Pico::hardware_spi Pico::pico_runtime
  )
  target_link_libraries(SpiPico INTERFACE
    SpiHal2
  )
endif()
