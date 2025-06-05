import io

from collections.abc import Callable, Iterator, Generator
from itertools import chain


class UnexpectedCommandIdError(Exception):
    def __init__(self, expected: int, actual: int):
        super().__init__(f"Expected command ID {expected}, but got {actual}.")
        self.expected = expected
        self.actual = actual


class PacketHeader:
    def __init__(self, command_id: int, packet_length: int):
        self.command_id = command_id
        self.packet_length = packet_length

    def tobytes(self) -> bytes:
        return bytes([self.command_id, self.packet_length])

    @classmethod
    def frombytes(cls, data: bytes) -> "PacketHeader":
        command_id = data[0]
        packet_length = data[1]
        return cls(command_id, packet_length)


class Packet:
    def __init__(self, hdr: PacketHeader, data: bytes):
        self.hdr = hdr
        self.data = data

    @property
    def command_id(self) -> int:
        return self.hdr.command_id

    @property
    def packet_length(self) -> int:
        return self.hdr.packet_length

    @classmethod
    def new(cls, command_id: int, data: bytes) -> "Packet":
        packet_length = len(data)
        hdr = PacketHeader(command_id, packet_length)
        return cls(hdr, data)

    def tobytes(self) -> bytes:
        return bytes(chain(self.hdr.tobytes(), self.data))


class CommandFactory:
    def __init__(self, writer: io.BufferedWriter, reader: io.BufferedReader) -> None:
        self._reader = reader
        self._writer = writer

    def create_command(
        self, command_id: int, packet_size: int, on_receive: Callable[[bytes], None]
    ) -> Callable[[Iterator[bytes]], None]:
        data_handler = create_data_handler(packet_size, on_receive)
        transmitter = PacketTransmitter(command_id, self._writer, self._reader)

        def call(data: Iterator[bytes]) -> None:
            transmitter.transmit(data_handler(data))

        return call


def create_data_handler(
    packet_size: int, on_receive: Callable[[bytes], None]
) -> Callable[[Iterator[bytes]], Generator[bytes, bytes, None]]:
    def handle_data(data: Iterator[bytes]) -> Generator[bytes, bytes, None]:
        for chunk in get_packet_sized_byte_chunks(data, packet_size):
            result = yield chunk
            on_receive(result)

    return handle_data


class PacketTransmitter:
    MAX_TOTAL_PACKET_SIZE = 256

    def __init__(
        self, command_id: int, writer: io.BufferedWriter, reader: io.BufferedReader
    ):
        self.command_id = command_id
        self.reader = reader
        self.writer = writer

    def _transmit_bytes(self, data: bytes) -> bytes:
        packet = Packet.new(self.command_id, data).tobytes()
        if len(packet) > self.MAX_TOTAL_PACKET_SIZE:
            raise ValueError("exceeding maximum packet size")
        self.writer.write(packet)
        self.writer.flush()
        response_hdr = PacketHeader.frombytes(self.reader.read(2))
        response = Packet(response_hdr, self.reader.read(response_hdr.packet_length))
        if response.command_id != self.command_id:
            raise UnexpectedCommandIdError(self.command_id, response.command_id)
        return response.data

    def transmit(self, generator: Generator[bytes, bytes, None]):
        try:
            data = next(generator)
            while True:
                data = generator.send(self._transmit_bytes(data))
        except StopIteration:
            pass


def get_packet_sized_byte_chunks(
    input_data: Iterator[bytes], packet_size: int
) -> Iterator[bytes]:
    num_packets = 0
    remainder = bytearray()

    def send_remainder() -> Iterator[bytes]:
        nonlocal num_packets
        num_packets = len(remainder) // packet_size
        for i in range(num_packets):
            start_index = i * packet_size
            end_index = start_index + packet_size
            yield bytes(remainder[start_index:end_index])

    for byte_chunk in input_data:
        remainder.extend(byte_chunk)
        yield from send_remainder()
        remainder = bytearray(remainder[num_packets * packet_size :])

    if len(remainder) > 0:
        yield bytes(remainder)
