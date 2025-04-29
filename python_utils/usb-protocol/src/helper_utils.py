from serial.tools import list_ports


def get_env5_port() -> str:
    for ports in list_ports.comports():
        if ports.vid == 11914:
            if ports.pid == 10 or ports.pid == 3:
                return ports.device


def is_env5_in_bootselect_stage() -> bool:
    for ports in list_ports.comports():
        if ports.vid == 11914:
            if ports.pid == 3:
                return True
