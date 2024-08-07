import time
from typing import Any, Literal

import serial


_recommended_commands = dict({
    "nack": 0,
    "ack": 1,
    "read_device_id": 2,
    "send_data_to_ram": 3,
    "read_data_from_ram": 4,
    "send_data_to_flash": 5,
    "read_data_from_flash": 6,
    "inference_with_ram_data": 7,
    "inference_with_flash_data": 8,
    "training_with_ram_data": 9 ,
    "training_with_flash_data": 10,
})

def xor_byte(a: int, b: int) -> int:
    my_xor = a ^ b
    return my_xor


def calculate_checksum(data: bytearray) -> bytes:
    checksum = data[0]
    for i, byte in enumerate(data[1:]):
        checksum = xor_byte(checksum, byte)
    return checksum.to_bytes(length=1, byteorder="little")


class Env5SerialCommunication:
    def __init__(self, device: serial.Serial, commands_lut: dict = _recommended_commands):
        self.serial: serial.Serial = device
        self.commands: dict[str, int] = commands_lut
        self.commands_inv: dict[int, str] = {v: k for k, v in commands_lut.items()}
        self.endian: Literal["little", "big"] = "little"
        self.checksum_length: int = 1

    def send(self, command: int, data: bytearray):
        ack_received = False
        while not ack_received:
            message = self._build_message(command, data)
            self.serial.write(message)
            rec_command, ack_received = self._receive_ack()
            if rec_command != command:
                ack_received = False

    def _build_message(self, command: int, data: bytearray) -> bytearray:
        message = bytearray()
        payload_size = len(data)
        message.extend(command.to_bytes(length=1, signed=False, byteorder=self.endian))
        message.extend(payload_size.to_bytes(length=4, signed=False, byteorder=self.endian))
        message.extend(data)
        message.extend(calculate_checksum(message))
        return message

    def _receive_ack(self) -> tuple[int, bool]:
        message = bytearray()

        command_raw = self.serial.read(1)
        command = int.from_bytes(command_raw, byteorder=self.endian, signed=False)

        data_length_raw = self.serial.read(4)
        data_length = int.from_bytes(data_length_raw, byteorder=self.endian, signed=False)

        data_raw = self.serial.read(data_length)

        transmitted_checksum = self.serial.read(self.checksum_length)

        message.extend(command_raw)
        message.extend(data_length_raw)
        message.extend(data_raw)

        calculated_checksum = calculate_checksum(message)
        if calculated_checksum == transmitted_checksum:
            if command == self.commands["ack"]:
                return command, True
        return -1, False

    def receive(self) -> bytes:
        message_complete = False
        while message_complete:
            message = bytearray()

            command_raw = self.serial.read(1)
            command = int.from_bytes(command_raw, byteorder=self.endian, signed=False)

            data_length_raw = self.serial.read(4)
            data_length = int.from_bytes(data_length_raw, byteorder=self.endian, signed=False)

            data_raw = self.serial.read(data_length)

            transmitted_checksum = self.serial.read(self.checksum_length)

            message.extend(command_raw)
            message.extend(data_length_raw)
            message.extend(data_raw)

            calculated_checksum = calculate_checksum(message)

            if transmitted_checksum != calculated_checksum:
                time.sleep(0.1)
                self.serial.reset_input_buffer()  # This might be problematic for timing. Maybe move it to other space
                self._send_nack(command)
            else:
                self._send_ack(command)
                return data_raw

    def _send_nack(self, command_to_nack: int) -> None:
        command = self.commands["nack"]
        data = bytearray(command_to_nack.to_bytes(length=1, signed=False, byteorder=self.endian))
        self._send(command, data)

    def _send_ack(self, command_to_ack: int) -> None:
        command = self.commands["ack"]
        data = bytearray(command_to_ack.to_bytes(length=1, signed=False, byteorder=self.endian))
        self._send(command, data)

    def send_data_to_flash(self, data_raw: bytearray, address: int) -> None:
        command = self.commands["send_data_to_flash"]
        data_raw = bytearray(address.to_bytes(length=4, signed=False, byteorder=self.endian))
        data_raw.extend(data_raw)
        message_send_successful = False
        while not message_send_successful:
            self._send(command=command, data=data_raw)


