# expose the pico targets by creating aliases that match our naming scheme

function(__add_pico_lib original)
  if(NOT _BUILDING_FOR_HARDWARE)
    if(NOT TARGET pico_empty_lib)
      add_library(pico_empty_lib INTERFACE)
    endif()
    add_library(Pico::${original} ALIAS pico_empty_lib)
  else()
    add_library(Pico::${original} ALIAS ${original})
  endif()
endfunction()

# general targets (RP2040 and RP2350)
foreach(arg
  # Low Level APIs
  hardware_adc
  hardware_base
  hardware_boot_lock
  hardware_claim
  hardware_clocks
  hardware_divider
  hardware_dma
  hardware_exception
  hardware_flash
  hardware_gpio
  hardware_i2c
  hardware_interp
  hardware_irq
  hardware_pio
  hardware_pll
  hardware_pwm
  hardware_resets
  hardware_spi
  hardware_sync
  hardware_ticks
  hardware_timer
  hardware_uart
  hardware_vreg
  hardware_watchdog
  hardware_xosc
  # High Level APIs
  pico_aon_timer
  pico_bootsel_via_double_reset
  pico_flash
  pico_i2c_slave
  pico_multicore
  pico_stdlib
  pico_sync
  pico_time
  pico_unique_id
  pico_util
  # Runtime Infrastructe Libraries
  pico_atomic
  pico_base
  pico_binary_info
  pico_bootrom
  pico_bit_ops
  pico_cxx_options
  pico_clib_interface
  pico_crt0
  pico_divider
  pico_double
  pico_float
  pico_int64_ops
  pico_malloc
  pico_mem_ops
  pico_platform
  pico_printf
  pico_runtime
  pico_runtime_init
  pico_stdio
  pico_stdio_semihosting
  pico_stdio_uart
  pico_stdio_usb
  pico_stdio_rtt
  pico_standard_binary_info
  pico_standard_link
)
  __add_pico_lib(${arg})
endforeach()

if(${PICO_PLATFORM} STREQUAL rp2040)
  foreach(arg
  # Low Level APIs
  hardware_rtc
  )
    __add_pico_lib(${arg})
  endforeach()
elseif(${PICO_PLATFORM} STREQUAL rp2350)
  foreach(arg
  # Low Level APIs
  hardware_dcp
  hardware_powman
  hardware_rcp
  hardware_sha256
  # High Level APIs
  pico_sha256
  # RISCV Libraries
  #hardware_riscv
  #hardware_riscv_platform_timer
  #hardware_hazard3
  )
    __add_pico_lib(${arg})
  endforeach()
endif()
