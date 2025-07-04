import math
from typing import Callable

import serial

from .communication_base_protocol import (
    get_base_commands,
    EnV5BaseRemoteControlProtocol,
    WrongCommand,
)


def get_recommended_commands() -> dict:
    recommend_commands = get_base_commands() | dict(
        {
            "read skeleton id": 2,
            "get chunk-size for flash": 3,
            "send data to flash": 4,
            "read data from flash": 5,
            "FPGA power": 6,
            "FPGA LEDs": 7,
            "MCU LEDs": 8,
            "inference with data": 9,
            "deploy model": 10,
        }
    )
    return recommend_commands


class EnV5RecommendedRemoteControlProtocol(EnV5BaseRemoteControlProtocol):
    def __init__(
        self,
        device: serial.Serial,
        get_commands_lut: Callable[[], dict] = get_recommended_commands,
    ):
        super().__init__(device=device, get_commands_lut=get_commands_lut)

    def _send_data_in_chunks(
        self,
        command: int,
        data: bytearray,
        chunk_size: int,
    ):
        num_chunks = math.ceil(len(data) / chunk_size)

        for i in range(num_chunks):
            payload = bytearray()
            if i + 1 == num_chunks:
                data_chunk = data[chunk_size * i :]
            else:
                data_chunk = data[chunk_size * i : chunk_size * (i + 1)]
            payload.extend(data_chunk)
            self._send(command, payload)

    def _receive_data_in_chunks(
        self, command: int, num_bytes: int, chunk_size: int
    ) -> bytearray:
        num_chunks = math.ceil(num_bytes / chunk_size)

        data_to_receive = bytearray()

        for i in range(num_chunks):
            command_rec, payload_rec = self._receive()
            if command != command_rec:
                raise WrongCommand
            data_to_receive.extend(payload_rec)
        return data_to_receive

    def read_skeleton_id(self) -> bytes:
        command = self.commands["read skeleton id"]
        empty_payload = bytearray()

        self._send(command, empty_payload)

        rec_command, data_raw = self._receive()

        if rec_command != command:
            raise WrongCommand
        return data_raw

    def get_chunk_size_for_flash(self) -> int:
        command = self.commands["get chunk-size for flash"]
        empty_payload = bytearray()

        self._send(command, empty_payload)

        rec_command, data_raw = self._receive()

        if rec_command != command:
            raise WrongCommand

        return int.from_bytes(data_raw, byteorder=self.endian, signed=False)

    def send_data_to_flash(
        self, flash_start_address: int, data: bytearray, chunk_size: int
    ) -> bool:
        # Send initial command
        command = self.commands["send data to flash"]

        payload = bytearray()
        flash_start_address_raw = flash_start_address.to_bytes(
            length=4, byteorder=self.endian, signed=False
        )
        size_in_bytes = len(data)
        size_in_bytes_raw = size_in_bytes.to_bytes(
            length=4, byteorder=self.endian, signed=False
        )

        payload.extend(flash_start_address_raw)
        payload.extend(size_in_bytes_raw)

        self._send(command, payload)

        self._send_data_in_chunks(command, data, chunk_size)

        return True

    def read_data_from_flash(
        self, flash_start_address: int, num_bytes: int, chunk_size
    ) -> bytearray:
        # Send initial command
        command = self.commands["read data from flash"]

        payload = bytearray()
        payload.extend(
            flash_start_address.to_bytes(length=4, byteorder=self.endian, signed=False)
        )
        payload.extend(
            num_bytes.to_bytes(length=4, byteorder=self.endian, signed=False)
        )

        self._send(command, payload)

        # receive chunks
        return self._receive_data_in_chunks(command, num_bytes, chunk_size)

    def fpga_power(self, on: bool) -> None:
        command = self.commands["FPGA power"]

        if on:
            data = bytes([0xFF])
        else:
            data = bytes([0x00])
        payload = bytearray(data)

        self._send(command, payload)

    def fpga_leds(self, led1: bool, led2: bool, led3: bool, led4: bool) -> None:
        command = self.commands["FPGA LEDs"]

        leds = int(led1) + 2 * int(led2) + int(led3) * 4 + int(led4) * 8

        payload = bytearray()
        payload.extend(leds.to_bytes(length=1, byteorder=self.endian, signed=False))

        self._send(command, payload)

    def mcu_leds(self, led1: bool, led2: bool, led3: bool) -> None:
        command = self.commands["MCU LEDs"]

        leds = int(led1) + 2 * int(led2) + int(led3) * 4

        payload = bytearray()
        payload.extend(leds.to_bytes(length=1, byteorder=self.endian, signed=False))

        self._send(command, payload)

    def deploy_model(self, flash_start_address: int, skeleton_id: bytearray) -> bool:
        command = self.commands["deploy model"]

        payload = bytearray()

        payload.extend(flash_start_address.to_bytes(length=4, byteorder=self.endian, signed=False))
        payload.extend(skeleton_id)

        self._send(command, payload)

        command_rec, data = self._receive()
        if command_rec == command:
            if int.from_bytes(data) == 1:
                return True
        return False

    def inference_with_data(
        self,
        chunk_size: int,
        data: bytearray,
        num_bytes_outputs: int,
    ) -> bytearray:

        # send starting command
        command = self.commands["inference with data"]

        payload = bytearray()
        num_bytes_inputs = len(data)
        num_bytes_inputs_raw = num_bytes_inputs.to_bytes(
            length=4, byteorder=self.endian, signed=False
        )
        num_bytes_outputs_raw = num_bytes_outputs.to_bytes(
            length=4, byteorder=self.endian, signed=False
        )

        payload.extend(num_bytes_inputs_raw)
        payload.extend(num_bytes_outputs_raw)

        self._send(command, payload)

        # send data chunks
        self._send_data_in_chunks(command, data, chunk_size)

        inference_result = self._receive_data_in_chunks(
            command, num_bytes_outputs, chunk_size
        )

        return inference_result
