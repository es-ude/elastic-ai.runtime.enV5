import atexit
import time
import serial

from helper_utils import get_env5_port
from recommended_commands import *


def get_my_custom_commands() -> dict:
    custom_commands = get_base_commands() | dict({"mcu countdown": 241})
    return custom_commands


class CustomCommands(EnV5RecommendedRemoteControlProtocol):
    def __init__(
        self,
        device: serial.Serial,
        get_commands_lut: Callable[[], dict] = get_my_custom_commands,
    ):
        super().__init__(device=device, get_commands_lut=get_commands_lut)

    def mcu_blink(self) -> None:
        command = self.commands["mcu countdown"]
        payload = bytearray()

        self._send(command, payload)


def exit_handler(serial_con: serial.Serial):
    serial_con.close()
    print(f"closed {serial_con.port=}")


def mcu_blink():
    print("Test running")
    enV5RRCP.mcu_leds(False, False, False)
    for i in range(2):
        time.sleep(1)
        enV5RRCP.mcu_leds(False, True, False)
        print("LED on")
        time.sleep(1)
        enV5RRCP.mcu_leds(False, False, False)
        print("LED off")


def read_chunksize():
    print("Test running")
    chunk_size = enV5RRCP.get_chunk_size_for_flash()
    return chunk_size


def send_blink_file() -> (int, bytearray):
    with open(
        "./bitfile_scripts/bitfiles/env5_bitfiles/blink/blink_fast/led_test.bin",
        "rb",
    ) as fpga_config:
        bin_file: bytes = fpga_config.read()
    enV5RRCP.send_data_to_flash(0, bytearray(bin_file), chunk_size)
    return len(bin_file), bin_file


def read_blink_file() -> bytearray:
    return enV5RRCP.read_data_from_flash(0, file_length, chunk_size)


if __name__ == "__main__":
    print("start")
    serial_con = serial.Serial(get_env5_port())
    atexit.register(exit_handler, serial_con)
    print("serial connection done")

    # enV5_cc = CustomCommands(serial_con)
    # enV5_cc.mcu_blink()

    enV5RRCP = EnV5RecommendedRemoteControlProtocol(serial_con)
    print("Env5RRCP build")

    # mcu_blink()

    chunk_size = read_chunksize()
    print(f"{chunk_size=}")
    enV5RRCP.fpga_power(on=False)
    file_length, expected_content = send_blink_file()
    actual_content = read_blink_file()
    print(f"MATCH: {actual_content == expected_content}")
    enV5RRCP.fpga_power(on=True)
