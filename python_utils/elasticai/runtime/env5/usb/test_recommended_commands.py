import atexit
import time
from time import sleep
from unittest import expectedFailure

from serial import Serial

from helper_utils import get_env5_port
from recommended_commands import *

echo_server: (str, bytes) = (
    "./bitfile_scripts/bitfiles/env5_bitfiles/echo_server/env5_top_reconfig_stub_skeleton_v2.bin",
    b"\x32\x34\x30\x38\x32\x33\x45\x43\x48\x4F\x53\x45\x52\x56\x45\x52",  # == 240823ECHOSERVER
)


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


def exit_handler(cdc_port: Serial):
    serial_con.close()
    print(f"closed {cdc_port.port=}")


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


def upload_binary(bin_file: str) -> (int, bytearray):
    with open(bin_file, "rb") as fpga_config:
        bin_file: bytes = fpga_config.read()
    enV5RRCP.send_data_to_flash(0, bytearray(bin_file), chunk_size)
    return len(bin_file), bin_file


if __name__ == "__main__":
    print("START")
    serial_con = serial.Serial(get_env5_port())
    atexit.register(exit_handler, serial_con)
    print("serial connection established")

    # enV5_cc = CustomCommands(serial_con)
    # enV5_cc.mcu_blink()

    enV5RRCP = EnV5RecommendedRemoteControlProtocol(serial_con)
    print("Env5RRCP build")

    # mcu_blink()

    # enV5RRCP.mcu_leds(False, False, False)

    chunk_size = enV5RRCP.get_chunk_size_for_flash()
    print(f"{chunk_size=}")

    # enV5RRCP.fpga_power(on=False)
    #
    # file_length, expected_content = upload_binary(echo_server[0])
    #
    # actual_content = enV5RRCP.read_data_from_flash(0, file_length, chunk_size)
    # print(f"MATCH: {actual_content == bytearray(expected_content)}")

    enV5RRCP.fpga_power(on=True)
    sleep(0.1)

    actual_skeleton_id = enV5RRCP.read_skeleton_id()
    print(f"MATCH: {actual_skeleton_id == echo_server[1]}")

    # enV5RRCP.fpga_leds(True, True, True, True)
    # sleep(5)
    # enV5RRCP.fpga_leds(False, False, False, False)

    test_input: bytes = b"\xFF\x00\xFF\00\xFF"
    result = enV5RRCP.inference_with_data(
        chunk_size, bytearray(test_input), len(test_input), 0, bytearray(echo_server[1])
    )
    print(f"MATCH: {result == bytearray(test_input)}")