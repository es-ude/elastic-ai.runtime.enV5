import os.path

from serial import Serial
import math
from argparse import ArgumentParser


error_occurred: bool = False


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
        "--trigger",
        "-t",
        type=str,
        default="v",
        dest="trigger",
        help="Character to trigger the output",
    )
    arg_parser.add_argument(
        "--page-length",
        "-l",
        type=int,
        default=512,
        dest="page_length",
        help="Length of one flash page",
    )
    arg_parser.add_argument(
        "source_file",
        metavar="source",
        type=str,
        help="Relative path to the Bin-File",
    )
    return arg_parser


def print_bytearray(prefix: str, arr: bytes):
    print(prefix + "0x" + " 0x".join("%02X" % b for b in arr))


def get_total_fragment_size(filename: str) -> int:
    return os.path.getsize(filename)


def verify(source: str, trigger: str, page_length: int) -> None:
    global error_occurred
    with open(source, "rb") as file, open("test.bin", "wb") as sc:
        total_config_size: int = get_total_fragment_size(source)
        page_limit: int = math.ceil(total_config_size / page_length)
        page_counter: int = 0
        serial.write(trigger.encode("utf-8"))
        print(f"Testing {page_limit} pages.")
        while page_counter < page_limit:
            expected = file.read(page_length)
            serial.reset_input_buffer()
            serial.write("n".encode("utf-8"))
            actual = bytes.fromhex(serial.read(2 * len(expected)).decode("utf-8"))
            sc.write(actual)
            if expected != actual:
                error_occurred = True
                print(f"Error on page {page_counter+1}")
                # print_bytearray("  Expected: ", expected)
                # print_bytearray("  Actual: ", actual)
            page_counter += 1


if __name__ == "__main__":
    arguments = parser().parse_args()

    with Serial(arguments.port, arguments.baudrate) as serial:
        print("START")
        serial.reset_output_buffer()
        verify(
            source=arguments.source_file,
            trigger=arguments.trigger,
            page_length=arguments.page_length,
        )
        if not error_occurred:
            print("SUCCESS")
        print("DONE")
