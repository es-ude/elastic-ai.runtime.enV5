from pathlib import Path

from serial import Serial
from serial.tools import list_ports
from python_utils.elasticai.runtime.env5.usb import UserRemoteControl, get_env5_port

echo_server: (str, bytes) = (
    "python_utils/bitfile_scripts/bitfiles/env5_bitfiles/echo_server/env5_top_reconfig_stub_skeleton_v2.bin",
    b"\x32\x34\x30\x38\x32\x33\x45\x43\x48\x4F\x53\x45\x52\x56\x45\x52",  # == 240823ECHOSERVER
)

def get_pico_port():
    for c in list_ports.comports():
        if c.vid == 0x2e8a and c.pid == 0x000A:
            return c.device

def test_user_remote_control():


    serial_port = get_pico_port()
    with Serial(serial_port) as serial_con:

        echoserver_path = Path(echo_server[0])

        urcp = UserRemoteControl(device=serial_con)

        with open(echoserver_path, 'rb') as f:
            echoserver_file = f.read()
        binfile_address = 0

        urcp.send_data_to_flash(binfile_address, echoserver_file)

        data = b'\xDE\xAD\xBE\xEF'
        result = urcp.inference_with_data(data, num_bytes_outputs=len(data))

    assert data == result


if __name__ == "__main__":
    test_user_remote_control()