# Prebuild bitfiles for ENV5 rev1

- Targeted FPGA are xc7s15ftgb196-1 (active)
- Features these two bitfile has
  - you can turn on and off LEDs
  - you can read the LED status
  - you can read the ID of the design
  - in it userlogic space, there is a buffer you can write data to it and then read data out. 
- `blink_slow` and `blink_fast`
  - only blinks LEDs one with 100MHz clock, another with 32MHz clock
  - no communication interface to the MCU
  - no middelware