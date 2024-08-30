import time
from typing import Literal, Callable

import serial
import logging

logger = logging.getLogger(__name__)


class WrongCommand(Exception): ...


class SendingNotSuccesful(Exception): ...


def get_base_commands() -> dict:
    base_commands = dict(
        {
            "nack": 0,
            "ack": 1,
        }
    )
    return base_commands


def xor_byte(a: int, b: int) -> int:
    my_xor = a ^ b
    return my_xor


def xor_calculate_checksum(data: bytearray) -> bytes:
    checksum = data[0]
    for i, byte in enumerate(data[1:]):
        checksum = xor_byte(checksum, byte)
    return checksum.to_bytes(length=1, byteorder="little")


class EnV5BaseRemoteControlProtocol:
    def __init__(
        self,
        device: serial.Serial,
        get_commands_lut: Callable[[], dict] = get_base_commands,
        calculate_checksum: [[bytearray], bytes] = xor_calculate_checksum,
        _logger: logging.Logger = logger
    ):
        self.serial: serial.Serial = device
        self.commands: dict[str, int] = get_commands_lut()
        self.commands_inv: dict[int, str] = {v: k for k, v in self.commands.items()}
        self.calculate_checksum = calculate_checksum
        self.checksum_length: int = 1
        self.endian: Literal["little", "big"] = "big"
        self.allowed_transmission_fails = 5
        self.logger = _logger

    def set_logger_level(self, level: str):
        allowed_levels = ["INFO", "DEBUG", "WARNING", "ERROR", "CRITICAL"]
        if level in allowed_levels:
            self.logger.setLevel(level)
        else:
            raise Exception(f"Logging level must be INFO, DEBUG, WARNING, ERROR or CRITICAL. You chose {level}.")

    def _send(self, command: int, payload: bytearray) -> None:
        ack_received: bool = False
        i = 0
        self.logger.debug("start sending")
        while not ack_received:
            # build message
            message = self._build_message(command, payload)

            self.logger.debug(f"{message=}")

            self.logger.debug("message build")
            # send message
            self.serial.write(message)

            self.logger.debug("message send")
            self.logger.debug("waiting for ack")
            # receive ack/nack
            ack_received = self._receive_ack()
            self.logger.debug(f"{ack_received=}")
            # if allowed transmission fails exceeds then
            i += 1
            if i > self.allowed_transmission_fails:
                raise SendingNotSuccesful
        self.logger.debug("send succesful")

    def _build_message(self, command: int, payload: bytearray) -> bytearray:
        # get payload_size
        payload_size = len(payload)

        # message body
        message = bytearray()

        # append command to message body
        message.extend(command.to_bytes(length=1, signed=False, byteorder=self.endian))

        # append payload_size to message body
        message.extend(
            payload_size.to_bytes(length=4, signed=False, byteorder=self.endian)
        )

        # append data to message body
        message.extend(payload)

        # append checksum to message body
        message.extend(self.calculate_checksum(message))
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
        data_length = int.from_bytes(
            data_length_raw, byteorder=self.endian, signed=False
        )

        # If data length > 0 read it
        if data_length > 0:
            data_raw = self.serial.read(data_length)
            message.extend(data_raw)

        # Read checksum
        transmitted_checksum = self.serial.read(self.checksum_length)

        # Calculate checksum
        calculated_checksum = self.calculate_checksum(message)

        # If checksum equal return true
        if calculated_checksum == transmitted_checksum:
            return True
        else:
            return False

    def _receive(self) -> tuple[int, bytes]:
        message_complete = False
        while not message_complete:
            # Message body
            message = bytearray()

            self.logger.debug(f"wait for command")
            # Read command + convert to int
            command_raw = self.serial.read(1)
            command = int.from_bytes(command_raw, byteorder=self.endian, signed=False)
            message.extend(command_raw)
            self.logger.debug(f"{command_raw=}")

            self.logger.debug(f"wait for data length")
            # Read data_length + convert to int
            data_length_raw = self.serial.read(4)
            data_length = int.from_bytes(
                data_length_raw, byteorder=self.endian, signed=False
            )
            message.extend(data_length_raw)
            self.logger.debug(f"{data_length_raw=}")

            if data_length > 0:
                # Read data for data_length
                self.logger.debug(f"wait for payload")
                payload_raw = self.serial.read(data_length)
                message.extend(payload_raw)
            else:
                payload_raw = None
            self.logger.debug(f"{payload_raw=}")

            self.logger.debug("wait for checksum")
            # read checksum
            transmitted_checksum = self.serial.read(self.checksum_length)

            # calculate checksum on message
            calculated_checksum = self.calculate_checksum(message)

            if transmitted_checksum != calculated_checksum:
                self.logger.debug("checksum wrong")
                # If checksum not correct empty buffer and sent nack
                while self.serial.readable():
                    self.serial.reset_input_buffer()  # This might be problematic for timing. Maybe move it to other space
                    time.sleep(0.1)
                self._send_nack()
            else:
                self.logger.debug("checksum correct")
                # if checksum correct send ack and return data
                self._send_ack()
                return command, payload_raw

    def _send_nack(self) -> None:
        self.logger.error("send nack")
        command = self.commands["nack"]
        no_data = bytearray()

        message = self._build_message(command, no_data)
        # send message
        self.serial.write(message)

    def _send_ack(self) -> None:
        self.logger.debug("send ack")
        command = self.commands["ack"]
        no_data = bytearray()

        message = self._build_message(command, no_data)

        # send message
        self.serial.write(message)
