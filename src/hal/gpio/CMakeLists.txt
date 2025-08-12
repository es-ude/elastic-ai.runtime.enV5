# Provides a Facade for the pico-sdk GPIO library

if(_BUILDING_FOR_HARDWARE)
  add_library(Gpio Gpio.c)
  target_link_libraries(Gpio PRIVATE
    Common
    Pico::hardware_gpio
    Pico::pico_runtime
  )
  target_include_directories(Gpio PUBLIC include)
else()
  add_library(Gpio INTERFACE)
  target_include_directories(Gpio INTERFACE include)
endif()
