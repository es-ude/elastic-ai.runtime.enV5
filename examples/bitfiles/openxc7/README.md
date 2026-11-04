# Open source FPGA build for the elastic-node

Builds a bitfile from Verilog sources with yosys, nextpnr-xilinx and Project X-Ray.
The toolchain runs in a Linux container, because the openXC7 packages do not build on macOS.

> [!WARNING]
> The Project X-Ray database only contains fabric data for the Spartan-7 XC7S25, XC7S50 and XC7S100.
> There is no data for the XC7S15, so this build **only works if your board has an XC7S50**.
> The enV5 revision 2 schematic lists an XC7S15 (FTGB196).
> The XC7S50 uses the same package and pin names, so the pin constraints are identical.

| File         | Purpose                                                                  |
| :----------- | :----------------------------------------------------------------------- |
| `Dockerfile` | Image with yosys, nextpnr-xilinx, Project X-Ray and the chip data        |
| `build.sh`   | Synthesis, place and route, bitstream and `.bin` (runs in the container) |

## Build

Build the image once. Run the commands from the `bitfiles` folder:

```bash
docker build -t enuo-openxc7 openxc7
```

Then run `build.sh` in the container with the sources and constraints of your design.
The `bitfiles` folder is mounted at `/work`, so the paths are relative to it:

```bash
docker run --rm -v "$PWD":/work -w /work enuo-openxc7 bash openxc7/build.sh \
    top=<module> src=<file.v>[,<file.v>...] xdc=<constraints.xdc> [key=value ...]
```

| Argument    | Default           | Description                                                 |
| :---------- | :---------------- | :---------------------------------------------------------- |
| `top`       | – (required)      | Top module                                                  |
| `src`       | – (required)      | Verilog or SystemVerilog sources, separated by commas       |
| `xdc`       | – (required)      | Constraints file with pins and I/O standards                |
| `name`      | `<top>`           | Base name of the output files                               |
| `part`      | `xc7s50ftgb196-1` | FPGA part                                                   |
| `freq_mhz`  | `100`             | Target clock frequency in whole MHz                         |
| `out`       | `build`           | Output directory                                            |
| `param.<N>` | –                 | Override parameter `<N>` of the top module, can be repeated |

For example, the LED test design:

```bash
docker run --rm -v "$PWD":/work -w /work enuo-openxc7 bash openxc7/build.sh \
    top=led_blink src=env5_bitfiles/blink/src/led_blink.v \
    xdc=env5_bitfiles/blink/src/constraints.xdc \
    param.CLK_HZ=100000000 param.BLINK_HZ=2 name=led_blink_2hz out=openxc7/build
```

The script [`env5_bitfiles/blink/src/build.sh`](../env5_bitfiles/blink/src/build.sh) builds the 1 Hz and 2 Hz
variants this way and copies them to `env5_bitfiles/blink/xc7s50`.

The results are written to `<out>`: `<name>.bit` and `<name>.bin`,
plus the intermediate `.json`, `.fasm` and `.frames` files.

## Constraints

nextpnr-xilinx only understands a subset of the XDC format:

- Use `set_property PACKAGE_PIN` and `set_property IOSTANDARD` for every port.
- Wildcards in port names (`leds[*]`) are not expanded, list each port.
- Clock constraints are ignored, set the frequency with `freq_mhz`.

## Limitations

- The bitstream has no compression and no SPI flash settings
  (bus width, configuration rate), so it is about 2.2 MB.
  Check that the FPGA loads it from the flash.
- Timing is only checked against `freq_mhz`, in whole MHz.
