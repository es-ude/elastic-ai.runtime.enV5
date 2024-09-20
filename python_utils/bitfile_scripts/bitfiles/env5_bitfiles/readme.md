# FPGA configuration for ENV5 hardware tests

> Targeted FPGA are xc7s15ftgb196-1

## [Reconfig](top_recofnig)

- You can turn on and off LEDs
- You can read the LED status
- You can read the ID of the design
- Provides userlogic space,
  - A buffer you can write data to and then read data out

## [Blink](blink)

- Blink the FPGA LEDs, based on different clock frequency
  - [slow](blink/blink_slow/) -> 100MHz clock
  - [fast](blink/blink_fast/) -> 32MHz clock
- **No** communication interface to the MCU
- **No** middelware

## [Echo Server](echo_server)

- Provides a simple echo server
- You can send a uint16_t to it
- Received value is incremented by one
- You can read out the incremented value