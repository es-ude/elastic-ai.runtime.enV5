# Constraints for the LED test design on the elastic-node enV5 revision 2,
# reduced to what nextpnr-xilinx understands (pins and I/O standards).
# The clock frequency is passed to nextpnr with freq_mhz by build.sh.
# nextpnr-xilinx does not expand wildcards in port names.
#
# Source: elasticai.hardware, v5/revision_2/main_board/en_v5_rev2.sch
# - Net GCLK_100 is the 100 MHz oscillator X3. It also reaches pin A10 (EMCCLK), which is
#   not clock-capable and stays unused. H11 is a MRCC pin.
# - Nets FPGA_LED0..3 have a 499 R series resistor and a red LED to GND (active high).
# - Bank 14 is supplied with 3.0 V, which is inside the LVCMOS33 range.

set_property PACKAGE_PIN H11 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]

set_property PACKAGE_PIN H12 [get_ports {leds[0]}]
set_property PACKAGE_PIN J12 [get_ports {leds[1]}]
set_property PACKAGE_PIN K12 [get_ports {leds[2]}]
set_property PACKAGE_PIN J11 [get_ports {leds[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {leds[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {leds[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {leds[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {leds[3]}]
