# The Hardware Abstraction Layer (HAL)

The HAL libraries intend to bundle all dependencies to the pico-sdk.
This archives independence from the used hardware in other libraries
such as sensor libraries.

## pico-sdk

The [pico-sdk](https://github.com/raspberrypi/pico-sdk) is included as
an external dependency to enable easy usage of the RP2040 MCU.
It provides simple to use interfaces for the hardware functions provided by the
Raspberry Pi MCU.

[pico-sdk Documentation](https://www.raspberrypi.com/documentation/pico-sdk/)
