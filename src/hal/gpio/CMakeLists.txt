# Provides a Facade for the pico-sdk GPIO library

if (BUILDING_FOR_ELASTIC_NODE)
    add_library(Gpio Gpio.c)
    target_link_libraries(Gpio PRIVATE Common Pico::hardware_gpio)
    target_include_directories(Gpio PUBLIC include)
else ()
    add_library(Gpio INTERFACE)
    target_include_directories(Gpio INTERFACE include)
endif ()