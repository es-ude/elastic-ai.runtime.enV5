# FPGA configuration handler

The FPGA on the elastic-node is configured by writing a bit- or binfile to the
flash. The FPGA always reconfigures from address `0x000000` of the flash.

The library provides functions to download configuration files to the flash
via HTTP (see [Via HTTP GET](#via-http-get)) or USB.

> [!WARNING]
> The serial transfer with the `BitfileFlasher.py` script is not maintained
> anymore and must be considered **broken**.
> Use the [USB remote-control protocol](../../../python_utils/usb-protocol/README.md)
> instead.

## Via HTTP GET

The ESP32 Wi-Fi module on the extension board downloads the configuration
file from a webserver and the library writes it to the flash.
The
[HardwareTestConfigureViaHTTP.c](../../../test/hardware/TestConfiguration/HardwareTestConfigureViaHTTP.c)
hardware test shows how to use it.

1. Set your network credentials as described in the
   [network README](../../network/README.md).
2. Set `baseUrl` in the hardware test to the address of the webserver.
   The default is `http://127.0.0.1:5000`.
3. Flash the hardware test to the MCU.
4. Start a webserver that serves the configuration files.
   For test purposes, a simple webserver can be started by running
   [HW-Test_Webserver.py](../../../python_utils/bitfile_scripts/HW-Test_Webserver.py).

Send the following characters to the device, for example via minicom,
to control the hardware test:

| Character | Action                                           |
| :-------: | :----------------------------------------------- |
|    `d`    | download the fast blinking configuration         |
|    `D`    | download the slow blinking configuration         |
|    `r`    | read the fast blinking configuration             |
|    `R`    | read the slow blinking configuration             |
|    `v`    | verify the fast blinking configuration           |
|    `V`    | verify the slow blinking configuration           |
|    `E`    | erase the whole flash                            |
|    `P`    | power on the FPGA, which loads the configuration |
|    `p`    | power off the FPGA                               |
