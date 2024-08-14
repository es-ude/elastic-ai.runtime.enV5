import time
from typing import Any, Literal

import serial


_recommended_commands = dict({
    "nack": 0,
    "ack": 1,
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
        self.allowed_transmission_fails = 10

    def send(self, command: int, data: bytearray):
        ack_received: bool = False
        i = 0
        while not ack_received:
            # build message
            message = self._build_message(command, data)

            # send message
            self.serial.write(message)

            # receive ack/nack
            ack_received = self._receive_ack()

            # if allowed transmission fails exceeds then
            i += 1
            if i > self.allowed_transmission_fails:
                return False
        return True

    def _build_message(self, command: int, data: bytearray) -> bytearray:
        # get payload_size
        payload_size = len(data)

        # message body
        message = bytearray()

        # append command to message body
        message.extend(command.to_bytes(length=1, signed=False, byteorder=self.endian))

        # append payload_size to message body
        message.extend(payload_size.to_bytes(length=4, signed=False, byteorder=self.endian))

        # append data to message body
        message.extend(data)

        # append checksum to message body
        message.extend(calculate_checksum(message))
        return message

    def _receive_ack(self) -> bool:
        # message body to append
        message = bytearray()

        # Receive command + append to message body
        command_raw = self.serial.read(1)
        message.extend(command_raw)

        # Receive data length + append to message body
        data_length_raw = self.serial.read(4)
        message.extend(data_length_raw)

        # convert data_length to int
        data_length = int.from_bytes(data_length_raw, byteorder=self.endian, signed=False)

        # If data length > 0 read it
        if data_length > 0:
            data_raw = self.serial.read(data_length)
            message.extend(data_raw)

        # Read checksum
        transmitted_checksum = self.serial.read(self.checksum_length)

        # Calculate checksum
        calculated_checksum = calculate_checksum(message)

        # If checksum equal return true
        if calculated_checksum == transmitted_checksum:
            return True
        else:
            return False

    def receive(self) -> tuple[int, bytes]:
        message_complete = False
        while message_complete:
            # Message body
            message = bytearray()

            # Read command + convert to int
            command_raw = self.serial.read(1)
            command = int.from_bytes(command_raw, byteorder=self.endian, signed=False)

            # Read data_length + convert to int
            data_length_raw = self.serial.read(4)
            data_length = int.from_bytes(data_length_raw, byteorder=self.endian, signed=False)

            # Read data for data_length
            data_raw = self.serial.read(data_length)

            # read checksum
            transmitted_checksum = self.serial.read(self.checksum_length)

            # append command, data_length_raw and data_raw to message
            message.extend(command_raw)
            message.extend(data_length_raw)
            message.extend(data_raw)

            # calculate checksum on message
            calculated_checksum = calculate_checksum(message)

            if transmitted_checksum != calculated_checksum:
                # If checksum not correct empty buffer and sent nack
                while self.serial.readable():
                    self.serial.reset_input_buffer() # This might be problematic for timing. Maybe move it to other space
                    time.sleep(0.1)
                self._send_nack()
            else:
                # if checksum correct send ack and return data
                self._send_ack()
                return command, data_raw

    def _send_nack(self) -> None:
        command = self.commands["nack"]
        no_data = bytearray()

        message = self._build_message(command, no_data)
        # send message
        self.serial.write(message)

    def _send_ack(self) -> None:
        command = self.commands["ack"]
        no_data = bytearray()

        message = self._build_message(command, no_data)

        # send message
        self.serial.write(message)


