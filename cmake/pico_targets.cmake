# expose the pico targets by creating aliases that match our naming scheme

function(__add_pico_lib original)
    if(NOT BUILDING_FOR_ELASTIC_NODE)
        if(NOT TARGET pico_empty_lib)
            add_library(pico_empty_lib INTERFACE)
        endif ()
        add_library(Pico::${original} ALIAS pico_empty_lib)
    else ()
        add_library(Pico::${original} ALIAS ${original})
    endif ()
endfunction()


foreach (arg
        pico_runtime
        pico_util
        pico_platform
        pico_clib_interface
        pico_time
        pico_stdlib
        pico_stdio
        pico_bootrom
        pico_stdio_usb
        pico_sync
        hardware_gpio
        hardware_i2c
        hardware_spi
        hardware_timer
        hardware_rtc
        hardware_uart
        hardware_irq
        hardware_base
        hardware_watchdog
)
    __add_pico_lib(${arg})
endforeach ()
