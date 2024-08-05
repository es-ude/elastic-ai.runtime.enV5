from typing import Any

import serial


def xor_byte(a: int, b: int) -> int:
    my_xor = a ^ b
    return my_xor


def calculate_checksum(data: bytearray) -> bytes:
    checksum = data[0]
    print(f"{checksum=}")
    print()
    for i, byte in enumerate(data[1:]):
        print(f"{i=}: {byte=}")
        checksum = xor_byte(checksum, byte)
        print(f"{i=}: {checksum=}")
        print()
    return checksum.to_bytes(length=1, byteorder="little")


class Env5SerialCommunication:

    def __init__(self, device: serial.Serial):
        self.serial = serial.Serial

    def _send(self, command: int, data: Any | bytes):
        message = self._build_message(command, data)
        #self.serial.write(message)

    def _build_message(self, command: int, data: Any | bytearray) -> bytearray:
        message = bytearray()
        payload_size = len(data)
        message.append(command.to_bytes(length=1, signed=False, byteorder="little"))
        message.append(payload_size.to_bytes(length=4, signed=False, byteorder="little"))
        message.append(data)
        message.append(calculate_checksum(message))


