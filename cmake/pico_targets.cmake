# expose the pico targets by creating aliases that match our naming scheme

function(__add_pico_lib original)
    if(NOT BUILDING_FOR_ELASTIC_NODE)
        if(NOT TARGET pico_empty_lib)
            add_library(pico_empty_lib INTERFACE)
        endif ()
        add_library(Pico::${original}__hdrs ALIAS pico_empty_lib)
        add_library(Pico::${original} ALIAS pico_empty_lib)
    else ()
        add_library(Pico::${original} ALIAS ${original})
        add_library(Pico::${original}__hdrs ALIAS ${original}_headers)
    endif ()
endfunction()


foreach (arg
        pico_runtime hardware_gpio
        hardware_sleep
        pico_time hardware_timer
        hardware_uart hardware_irq
)
    __add_pico_lib(${arg})
endforeach ()
