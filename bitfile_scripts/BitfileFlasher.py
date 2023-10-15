from __future__ import annotations

import os.path

from serial import Serial
from argparse import ArgumentParser
from math import ceil


def print_bytearray(arr: bytes | bytearray):
    print("0x" + " 0x".join("%02X" % b for b in arr))


def get_total_fragment_size(filename: str) -> int:
    return os.path.getsize(filename)


def parser() -> ArgumentParser:
    arg_parser = ArgumentParser(description="Flash Bin-File")
    arg_parser.add_argument(
        "--port",
        "-p",
        type=str,
        default="/dev/ttyACM0",
        dest="port",
        help="Serial Port for enV5 connection",
    )
    arg_parser.add_argument(
        "--baudrate",
        "-b",
        type=int,
        default=5000000,
        dest="baudrate",
        help="Baudrate for the Serial Connection",
    )
    arg_parser.add_argument(
        "--destination_address",
        "-a",
        type=int,
        default=0,
        dest="address",
        help="Location in Flash",
    )
    arg_parser.add_argument(
        "--page-length",
        "-l",
        type=int,
        default=512,
        dest="page_length",
        help="Bytes Per Page",
    )
    arg_parser.add_argument(
        "source_file",
        metavar="source",
        type=str,
        help="Relative path to the Bin-File",
    )
    return arg_parser


def wait_for_response(ser: Serial, expected: str = "ack") -> None:
    response: str = ser.read(3).decode("utf-8")
    if response != expected:
        print(f"Received: {response}. Exit now!")
        exit(1)


def init_transmission(ser: Serial, length: int) -> None:
    ser.write("s".encode("utf-8"))
    wait_for_response(ser)
    ser.write(length.to_bytes(length=4, byteorder="big", signed=False))
    wait_for_response(ser)


def transmit_configuration(
    ser: Serial, total_size: int, source: str, expected_chunk_length: int
) -> None:
    number_of_fragments: int = ceil(total_size / expected_chunk_length)
    print(f"Total amount of fragments: {number_of_fragments} for {total_size} Bytes")
    with open(source, "rb") as file:
        while True:
            received: bytes = ser.read(1)
            if received.decode("utf-8") == "o":
                return

            received += ser.read(3)
            fragment = int(received, base=16)

            file.seek(fragment * expected_chunk_length, 0)
            chunk = file.read(expected_chunk_length)
            real_chunk_length = len(chunk)

            print(f"    Send fragment: {fragment} with length {real_chunk_length}")
            ser.write(
                real_chunk_length.to_bytes(length=2, byteorder="big", signed=False)
            )
            wait_for_response(ser)
            ser.write(chunk)
            wait_for_response(ser)


if __name__ == "__main__":
    arguments = parser().parse_args()
    total_config_size: int = get_total_fragment_size(arguments.source_file)

    print("Transmission:")
    with Serial(arguments.port, arguments.baudrate) as serial:
        # serial.write("d".encode("utf-8"))
        print("Initialize Transmission")
        init_transmission(serial, total_config_size)
        print("Transmit Data")
        transmit_configuration(
            serial, total_config_size, arguments.source_file, arguments.page_length
        )
    print("DONE")
