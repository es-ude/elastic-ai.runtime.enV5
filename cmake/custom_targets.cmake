function(create_enV5_executable target)
  # enable usb output
  pico_enable_stdio_usb(${target} 1)
  # disable uart output
  pico_enable_stdio_uart(${target} 0)
  # create map/bin/hex/uf2 file etc.
  pico_add_uf2_output(${target})
endfunction()
