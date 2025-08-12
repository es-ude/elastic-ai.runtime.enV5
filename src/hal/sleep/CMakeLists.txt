# Provides a Facade for the pico-sdk Time Library

if(_BUILDING_FOR_HARDWARE)
  add_library(Sleep Sleep.c)
  target_include_directories(Sleep PUBLIC include)
  target_link_libraries(Sleep PRIVATE
    Common
    Pico::pico_time
    Pico::pico_runtime
  )
else()
  add_library(Sleep INTERFACE)
  target_include_directories(Sleep INTERFACE include)
endif()
