import os.path
from pathlib import Path
from time import sleep

import serial
from serial.tools import list_ports
import atexit

from python_utils.usb import UserRemoteControl, get_env5_port


def exit_handler(serial_connection: serial.Serial):
    serial_connection.close()
    print(f"closed {serial_connection.port=}")


def run_inference_with_data(rcp: UserRemoteControl):
    data = bytearray(b'\xFF\xFE\x00\x00')
    skeleton_id = bytearray(b'\x32\x33\x31\x30\x30\x39\x65\x63\x68\x6F\x73\x65\x72\x76\x65\x72')

    rcp.inference_with_data(data, num_bytes_outputs=len(data), bin_file_address=0, skeleton_id=skeleton_id)


if __name__ == "__main__":
    #serial_port = get_env5_port()
    serial_port = "/dev/tty.usbmodem1101"
    print(f"{serial_port=}")
    serial_con = serial.Serial(serial_port)

    atexit.register(exit_handler, serial_con)

    base = Path("/Users/leoburon/work/elastic-ai.runtime.enV5/bitfile_scripts/bitfiles/env5_bitfiles/")

    urcp = UserRemoteControl(device=serial_con)

    with open(os.path.join(base, "echo_server/env5_top_reconfig.bin"), 'rb') as f:
        data = f.read()
    urcp.send_data_to_flash(0, data)
    run_inference_with_data(urcp)
