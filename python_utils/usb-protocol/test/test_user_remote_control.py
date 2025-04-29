from pathlib import Path

import serial
import atexit

from python_utils.elasticai.runtime.env5.usb import UserRemoteControl, get_env5_port

echo_server: (str, bytes) = (
    "./bitfile_scripts/bitfiles/env5_bitfiles/echo_server/env5_top_reconfig_stub_skeleton_v2.bin",
    b"\x32\x34\x30\x38\x32\x33\x45\x43\x48\x4F\x53\x45\x52\x56\x45\x52",  # == 240823ECHOSERVER
)


def exit_handler(serial_connection: serial.Serial):
    serial_connection.close()
    print(f"closed {serial_connection.port=}")


if __name__ == "__main__":
    serial_port = get_env5_port()
    #serial_port = "/dev/tty.usbmodem3213101"
    print(f"{serial_port=}")
    serial_con = serial.Serial(serial_port)

    atexit.register(exit_handler, serial_con)

    echoserver_path = Path(echo_server[0])

    urcp = UserRemoteControl(device=serial_con)

    with open(echoserver_path, 'rb') as f:
        echoserver_file = f.read()
    binfile_address = 0

    urcp.send_data_to_flash(binfile_address, echoserver_file)

    data = bytearray(b'\xFF\xFE\x00\x00')
    data = urcp.inference_with_data(data, num_bytes_outputs=len(data), bin_file_address=binfile_address, skeleton_id=echo_server[1])
    print(f"inference result = {data}")

