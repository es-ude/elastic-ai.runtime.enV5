import serial

from .recommended_commands import EnV5RecommendedRemoteControlProtocol


class UserRemoteControl:
    def __init__(self, device: serial.Serial, logging_level: str = "INFO"):
        self.enV5RCP = EnV5RecommendedRemoteControlProtocol(device)
        self.enV5RCP.set_logger_level(logging_level)
        self.chunk_size = self.enV5RCP.get_chunk_size_for_flash()

    def fpga_leds(self, led1: bool, led2: bool, led3: bool, led4: bool) -> None:
        self.enV5RCP.fpga_leds(led1, led2, led3,
                               led4)

    def fpga_power_on(self, on: bool) -> None:
        self.enV5RCP.fpga_power(on)

    def mcu_leds(self, led1: bool, led2: bool, led3: bool) -> None:
        self.enV5RCP.mcu_leds(led1, led2, led3)

    def send_data_to_flash(self, flash_start_address: int, data: bytearray) -> bool:
        self.enV5RCP.fpga_power(False)
        return self.enV5RCP.send_data_to_flash(flash_start_address, data, self.chunk_size)

    def read_data_from_flash(self, flash_start_address: int, num_bytes: int) -> bytearray:
        self.enV5RCP.fpga_power(False)
        return self.enV5RCP.read_data_from_flash(flash_start_address, num_bytes, self.chunk_size)

    def inference_with_data(self,
                            data: bytearray,
                            num_bytes_outputs: int,
                            bin_file_address: int,
                            skeleton_id: bytearray) -> bytearray:
        self.enV5RCP.fpga_power(on=True)
        return self.enV5RCP.inference_with_data(self.chunk_size, data, num_bytes_outputs, bin_file_address, skeleton_id)


