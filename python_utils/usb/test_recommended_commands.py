import atexit
import time
import serial

from python_utils.usb.helper_utils import get_env5_port
from recommended_commands import *


def exit_handler(serial_con: serial.Serial):
    serial_con.close()
    print(f"closed {serial_con.port=}")


def mcu_blink():
    print("Test running")
    for i in range(2):
        time.sleep(1)
        enV5RRCP.mcu_leds(False, False, False)
        print("LED off")
        time.sleep(1)
        enV5RRCP.mcu_leds(True, False, False)
        print("LED on")


def read_chunksize():
    print("Test running")
    chunk_size = enV5RRCP.get_chunk_size_for_flash()
    return chunk_size


if __name__ == "__main__":
    print("start")
    serial_con = serial.Serial(get_env5_port())
    atexit.register(exit_handler, serial_con)
    print("serial connection done")

    enV5RRCP = EnV5RecommendedRemoteControlProtocol(serial_con)
    print("Env5RRCP build")

    mcu_blink()

    read_chunksize()
