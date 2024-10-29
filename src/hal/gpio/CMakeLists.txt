# Provides a Facade for the pico-sdk GPIO library

block(SCOPE_FOR VARIABLES)
    set(deps Common Pico::hardware_gpio)

    add_library(Gpio__hdrs INTERFACE)
    if(BUILDING_FOR_ELASTIC_NODE)
        add_library(Gpio__impl Gpio.c)
        elastic_ai_target_link_hdrs(Gpio__impl ${deps})
        elastic_ai_target_link_impl(Gpio__impl ${deps})
    endif ()
    add_library(Gpio INTERFACE)

    elastic_ai_connect_libs(Gpio)


endblock()