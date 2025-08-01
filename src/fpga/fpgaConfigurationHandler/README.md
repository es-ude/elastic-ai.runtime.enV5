# Via serial connection (USB)

The script is currently not maintained and must be considered **BROKEN**!

The implementation only works on the enV5, **not** the SensorBoard.

The FPGA on the enV5 can be configured by writing a Bit- or Binfile to
the flash. To write a file to the flash, the enV5 needs to be flashed
with the correct hardware test
[hardware-test_configUSB](../../../test/hardware/TestConfiguration/HardwaretestConfigureViaUSB.c).
Place the Binfile you want to send to the enV5 under
[bitfile_scripts/bitfiles](../../../bitfile_scripts/bitfiles/) and
execute the
[BitfileFlasher.py](../../../bitfile_scripts/BitfileFlasher.py) python
script with the required arguments:

    python BitfileFlasher.py \
        --port <device_serial_port> \
        [--baudrate <serial_baudrate>] \
        <path_to_bitfile> \
        <start_flash_address>

- Serial-Port of the device
- Baudrate of serial connection
- Path to bitfile
- Start address of the flash, where the bitfile should be written to

The python script will send the Bitfile over the serial connection to
the enV5. To configure the FPGA with the new Bitfile, the FPGA has to be
resetted.

The FPGA will always reconfigure from address 0x000000 of the flash. The
hardware test executable can be used for this by sending the character
`f`/`F` via minicom to the device. This will trigger a restart of the
FPGA.

# Via HTTPGet

The FPGA on the enV5 can be configured by writing a Binfile to the
flash. This can be archived by utilizing the ESP32 WiFi-module on the
extension board. If you want to use the WiFi-module, you have to flash
the
[hardware-test_configHTTP](../../../test/hardware/TestConfiguration/HardwaretestConfigureViaHTTP.c)
to the MCU.

You have to adjust the SSID and Password according to your Network
credentials!

Afterward, you must start the corresponding webserver stat provides the
server. For test purposes, a simple webserver can be started by
executing
[HW-Test_Webserver.py](../../../bitfile_scripts/HW-Test_Webserver.py).

The FPGA will always reconfigure from address 0x000000 of the flash. The
hardware test executable can be used for this by sending the character
`f`/`F` via minicom to the device. This will trigger a restart of the
FPGA.

# Via Elastic-ai.runtime

Coming soon…
