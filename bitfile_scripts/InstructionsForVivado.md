# Starting Vivado:

required commands:
cd into Vivado folder and source the settings64.sh

Example:
cd ../../tools/Xilinx/Vivado/2023.2/
source settings64.sh

-------------------------------------------------------------------------------

# Flashing & Verifying

All necessary files are in "experiment"

1. connect both USB cables (special JTAG cable is required)
2. make sure FPGA is powered on (type "P" in minicom)
3. right-click device and click "add configuration memory device"
4. search for "fl128" and choose "s25fl128sxxxxxx1-spi-x1_x2_x4"

Now you can either flash the device, or readback for verifying purposes

## How-To flash

1. right-click the memory configuration device and choose "Program memory configuration device"
2. choose .bin file
3. Afterward click the "MCU RST" Button on the device, to turn on the MCU


## How-To Readback

1. right-click configuration memory
2. click on readback configuration memory device
3. always choose "BIN" file format
4. under "Address" you can choose to redback everything, or use Offset and Datacount to specify the memory you want to read back (hex!)
    - to get the length of your file, use: du -b yourFile
    - convert this into hex, so you can use the "Datacount" field
   
## How-To verify if readback is correct

1. use "xxd" to convert .bin file into .hex file. Do this for both the source .bin file and the readback .bin file.
2. use "diff .hex .hex" to compare the two hex-files


## Example for env5_top_reconfig.bin

1. flash via HTTP or USB
2. readback from device (the address of the end of the file is "2AD10". Put this into Datacount when reading back) and choose BIN file format
3. convert source .bin file and the readback .bin file into hex files using xxd
4. compare them using diff
5. The only diff that should show up, is from trailing "ffff"s. This can be ignored. If this is the case, the readback is correct and the flashing process worked.
  
--------------------------------------------------------------------------------

# TCL Commands
- TCL Commands can be used to write a script that automates e.g. the readback process. These TCL Commands can be found in the TCL Console in VIvado, after reading back a file.


----------------------------------------------------------------------------

# How to install drivers for JTAG cables (if missing):

https://support.xilinx.com/s/question/0D52E00007BsRFTSA3/cable-drivers-not-working-for-users-that-didnt-install-them?language=en_US

cd /tools/Xilinx/Vivado/2023.2/data/xicom/cable_drivers/lin64/install_script/install_drivers

sudo /tools/Xilinx/Vivado/2023.2/data/xicom/cable_drivers/lin64/install_script/install_drivers

sudo chmod 644 /etc/udev/rules.d/52-xilinx-digilent-usb.rules


--------------------------------------------------------------------

# Errors and how to solve them

ERROR: [Labtools 27-2269] No devices detected on target localhost:3121/xilinx_tcf/Digilent/210249ACC056.
Check cable connectivity and that the target board is powered up then
use the disconnect_hw_server and connect_hw_server to re-register this hardware target.
ERROR: [Common 17-39] 'open_hw_target' failed due to earlier errors.

- make sure MCU is powered on (both cables connected, program doesnt wait for usb connection etc.)
- make sure FPGA is powered on!! ("P" in minicom window)



If freshly installed, try:
cd bc013-3/tools/Xilinx/Vitis_HLS/2023.2
sudo apt install libtinfo-dev
sudo ln -s /lib/x86_64-linux-gnu/libtinfo.so.6 /lib/x86_64-linux-gnu/libtinfo.so.5





