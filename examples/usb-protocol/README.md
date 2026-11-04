# USB protocol example

Remote control of the elastic-node from a computer over USB.
The example consists of a firmware for the elastic-node and a command line tool for the host.
Both use the [USB protocol](../../python_utils/usb-protocol/README.md).

| Folder                  | Content                                                                |
| :---------------------- | :--------------------------------------------------------------------- |
| [`firmware`](firmware/) | Firmware for the elastic-node, handles the commands of the protocol    |
| [`cli`](cli/env5ctl.py) | `env5ctl.py`, command line tool to control the elastic-node            |

## Firmware

The firmware is built by the `env5_rev2_*` CMake presets as target `ExampleUsbProtocol`:

```bash
cmake --preset env5_rev2_release
cmake --build --preset env5_rev2_release --target ExampleUsbProtocol
```

Flash `build/env5_rev2_release/examples/usb-protocol/firmware/ExampleUsbProtocol.uf2` to the elastic-node
as described in the [README](../../README.md#flash-the-elastic-node).

The firmware waits until the host opens the serial connection, then answers the protocol commands.
If a command fails, all LEDs light up for two seconds, then the third LED blinks once per error code.
Own commands can be added with `usbProtocolRegisterCommand`.

## Command line tool

The tool uses [Typer](https://typer.tiangolo.com/).
`uv run` installs its dependencies (`typer` and `pyserial`) on the first start, no setup is needed.
It detects the elastic-node automatically, use `--port` to select a serial port.
Every command has a `--help` option.

```bash
uv run examples/usb-protocol/cli/env5ctl.py <command> [options]
```

| Command               | Description                                                                              |
| :-------------------- | :--------------------------------------------------------------------------------------- |
| `fpga on\|off`        | Power the FPGA on or off                                                                 |
| `led on [--led N]...` | Switch the selected MCU LEDs (1 to 3, repeat `--led`, default all) on and the others off |
| `led off`             | Switch all MCU LEDs off                                                                  |
| `write FILE`          | Write a file to the flash, e.g. a `.bin` bitfile                                         |
| `read FILE --size N`  | Read `N` bytes from the flash to a file                                                  |

`write` and `read` accept `--address` (default `0`, decimal or hexadecimal such as `0x1000`).
`write` also accepts `--verify`, which reads the data back and compares it,
and `--power-on`, which powers the FPGA on afterwards.
Both commands power the FPGA off first, because the FPGA must not access the flash during the transfer.

> [!IMPORTANT]
> The FPGA loads its configuration from flash address `0`.
> The write address must be aligned to a flash sector, because the sectors are erased before writing.

Example: write a bitfile, check it and start it:

```bash
uv run examples/usb-protocol/cli/env5ctl.py write \
    examples/bitfiles/env5_bitfiles/blink/xc7s50/led_blink_1hz.bin --verify --power-on
```

Transferring a 2.2 MB bitfile takes about two minutes.
