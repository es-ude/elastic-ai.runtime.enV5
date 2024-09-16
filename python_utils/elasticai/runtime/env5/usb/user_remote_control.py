from pathlib import Path
from time import sleep

import serial

from .recommended_commands import EnV5RecommendedRemoteControlProtocol


class UserRemoteControl:
    def __init__(self, device: serial.Serial, logging_level: str = "INFO"):
        self._enV5RCP = EnV5RecommendedRemoteControlProtocol(device)
        self._enV5RCP.set_logger_level(logging_level)
        self.chunk_size = self._enV5RCP.get_chunk_size_for_flash()

    def fpga_leds(self, led1: bool, led2: bool, led3: bool, led4: bool) -> None:
        self._enV5RCP.fpga_leds(led1, led2, led3,
                                led4)

    def fpga_power_on(self, on: bool) -> None:
        self._enV5RCP.fpga_power(on)

    def mcu_leds(self, led1: bool, led2: bool, led3: bool) -> None:
        self._enV5RCP.mcu_leds(led1, led2, led3)

    def send_data_to_flash(self, flash_start_address: int, data: bytearray) -> bool:
        self._enV5RCP.fpga_power(False)
        return self._enV5RCP.send_data_to_flash(flash_start_address, data, self.chunk_size)

    def read_data_from_flash(self, flash_start_address: int, num_bytes: int) -> bytearray:
        self._enV5RCP.fpga_power(False)
        return self._enV5RCP.read_data_from_flash(flash_start_address, num_bytes, self.chunk_size)

    def send_and_deploy_model(self,
                              binfile_path: Path,
                              flash_start_address: int,
                              skeleton_id: bytearray) -> bool:
        self._enV5RCP.fpga_power(on=False)
        sleep(0.1)
        with open(binfile_path, "rb") as file:
            binfile: bytes = file.read()
        finished = self._enV5RCP.send_data_to_flash(flash_start_address, bytearray(binfile), self.chunk_size)
        if finished:
            self._enV5RCP.fpga_power(on=True)
            sleep(0.1)
            return self._enV5RCP.deploy_model(flash_start_address, skeleton_id)
        return False

    def deploy_model(self, flash_start_address: int, skeleton_id: bytearray) -> bool:
        return self._enV5RCP.deploy_model(flash_start_address, skeleton_id)

    def inference_with_data(self,
                            data: bytearray,
                            num_bytes_outputs: int) -> bytearray:
        self._enV5RCP.fpga_power(on=True)
        sleep(0.1)
        return self._enV5RCP.inference_with_data(self.chunk_size, data, num_bytes_outputs)


