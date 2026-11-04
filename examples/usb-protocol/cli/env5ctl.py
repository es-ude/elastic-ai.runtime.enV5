#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.12"
# dependencies = [
#     "pyserial>=3.5",
#     "typer>=0.12",
# ]
# ///
"""Command line tool to remote control an elastic-node over USB.

The elastic-node has to run the firmware from `../firmware` (or any firmware that
implements the USB protocol of `python_utils/usb-protocol`).
"""

import sys
import time
from contextlib import contextmanager
from enum import Enum
from pathlib import Path
from typing import Annotated, Iterator

import serial
import typer

try:
    from elasticai.usbprotocol import EnV5RecommendedRemoteControlProtocol, get_env5_port
except ImportError:  # not installed, use the sources in this repository
    sys.path.insert(0, str(Path(__file__).resolve().parents[3] / "python_utils" / "usb-protocol"))
    from elasticai.usbprotocol import EnV5RecommendedRemoteControlProtocol, get_env5_port

MCU_LEDS = (1, 2, 3)

app = typer.Typer(
    help="Remote control an elastic-node over USB.",
    no_args_is_help=True,
    add_completion=False,
)


class State(str, Enum):
    on = "on"
    off = "off"


class Connection:
    """Serial port selected with `--port`, opened when a command needs it."""

    def __init__(self, port: str | None):
        self.port = port

    def open(self) -> serial.Serial:
        port = self.port or get_env5_port()
        if port is None:
            typer.echo("no elastic-node found, connect it or pass --port", err=True)
            raise typer.Exit(1)
        return serial.Serial(port, 115200, timeout=10)


@app.callback()
def main(
    ctx: typer.Context,
    port: Annotated[
        str | None,
        typer.Option("--port", "-p", help="Serial port of the elastic-node (default: detect it)."),
    ] = None,
) -> None:
    ctx.obj = Connection(port)


@contextmanager
def connect(ctx: typer.Context) -> Iterator[EnV5RecommendedRemoteControlProtocol]:
    """Opens the connection for the duration of a command."""
    with ctx.obj.open() as device:
        yield EnV5RecommendedRemoteControlProtocol(device)


def parse_int(value: str | int) -> int:
    """Accepts decimal and prefixed numbers such as 0x1000, defaults arrive as int."""
    if isinstance(value, int):
        return value
    try:
        return int(value, 0)
    except ValueError:
        raise typer.BadParameter(f"'{value}' is not a number")


AddressOption = Annotated[
    int,
    typer.Option(
        "--address",
        "-a",
        parser=parse_int,
        metavar="ADDRESS",
        help="Flash start address, decimal or hexadecimal (0x...).",
    ),
]


@app.command()
def fpga(ctx: typer.Context, state: State) -> None:
    """Power the FPGA on or off."""
    with connect(ctx) as remote:
        remote.fpga_power(state is State.on)


@app.command()
def led(
    ctx: typer.Context,
    state: State,
    leds: Annotated[
        list[int],
        typer.Option(
            "--led",
            "-l",
            min=1,
            max=3,
            help="MCU LED (1 to 3) to switch on, can be repeated. Default: all. "
            "All other LEDs are switched off.",
        ),
    ] = list(MCU_LEDS),
) -> None:
    """Switch the MCU LEDs on or off."""
    with connect(ctx) as remote:
        # The protocol always sends the state of all LEDs: "off" switches all of them off.
        remote.mcu_leds(*[state is State.on and number in leds for number in MCU_LEDS])


@app.command()
def write(
    ctx: typer.Context,
    file: Annotated[
        Path,
        typer.Argument(exists=True, dir_okay=False, readable=True, help="File to write, e.g. a .bin bitfile."),
    ],
    address: AddressOption = 0,
    verify: Annotated[bool, typer.Option(help="Read the data back and compare it.")] = False,
    power_on: Annotated[bool, typer.Option(help="Power the FPGA on afterwards.")] = False,
) -> None:
    """Write a file to the flash.

    The address must be aligned to a flash sector, because the sectors are erased before writing.
    """
    data = file.read_bytes()
    if not data:
        typer.echo(f"{file} is empty", err=True)
        raise typer.Exit(1)

    with connect(ctx) as remote:
        # The FPGA must not access the flash while it is written.
        remote.fpga_power(False)
        chunk_size = remote.get_chunk_size_for_flash()

        typer.echo(f"writing {len(data)} bytes from {file} to flash address {address:#x} ...")
        start = time.monotonic()
        remote.send_data_to_flash(address, bytearray(data), chunk_size)
        typer.echo(f"written in {time.monotonic() - start:.1f} s")

        if verify:
            typer.echo("verifying ...")
            read_back = bytes(remote.read_data_from_flash(address, len(data), chunk_size))
            if read_back[: len(data)] != data:
                typer.echo("verification failed: the flash content differs from the file", err=True)
                raise typer.Exit(1)
            typer.echo("verification OK")

        if power_on:
            remote.fpga_power(True)
            typer.echo("FPGA powered on")


@app.command()
def read(
    ctx: typer.Context,
    file: Annotated[Path, typer.Argument(dir_okay=False, writable=True, help="File to write the data to.")],
    size: Annotated[
        int,
        typer.Option(
            "--size",
            "-s",
            parser=parse_int,
            metavar="BYTES",
            help="Number of bytes to read, e.g. the size of the bitfile.",
        ),
    ],
    address: AddressOption = 0,
) -> None:
    """Read data from the flash to a file."""
    if size <= 0:
        raise typer.BadParameter("size must be greater than 0", param_hint="--size")

    with connect(ctx) as remote:
        remote.fpga_power(False)
        chunk_size = remote.get_chunk_size_for_flash()

        typer.echo(f"reading {size} bytes from flash address {address:#x} ...")
        start = time.monotonic()
        data = bytes(remote.read_data_from_flash(address, size, chunk_size))[:size]
        file.write_bytes(data)
        typer.echo(f"{len(data)} bytes written to {file} in {time.monotonic() - start:.1f} s")


if __name__ == "__main__":
    app()
