from collections.abc import Iterator
import pytest
import io
from experiment_protocol import CommandFactory, UnexpectedCommandIdError
from experiment_protocol.experiment_protocol import get_packet_sized_byte_chunks


@pytest.fixture
def reader():
    return io.BytesIO(b"\x05\x01\xde\x05\x01\xad\x05\x01\xbe\x05\x01\xef")


@pytest.fixture
def reader_providing_different_command_id():
    return io.BytesIO(b"\x06\x01\xde\x06\x01\xad\x06\x01\xbe\x06\x01\xef")


@pytest.fixture
def writer():
    return io.BytesIO()


@pytest.fixture
def response():
    return []


@pytest.fixture
def handle_response(response):
    def handle(data):
        response.append(data)

    return handle


@pytest.fixture
def raw_byte_string():
    return b"my raw byte data stream"


@pytest.fixture
def command(reader, writer, handle_response, raw_byte_string):
    _command_factory = CommandFactory(writer, reader)

    my_command = _command_factory.create_command(
        5, len(raw_byte_string) + 3, handle_response
    )
    return my_command


def provide_data_n_times(n: int) -> Iterator[bytes]:
    data = b"my raw byte data stream"
    for _ in range(n):
        yield data


def test_command_reads_response_completely(command, response):
    command(provide_data_n_times(4))
    assert response == [b"\xde", b"\xad", b"\xbe", b"\xef"]


def test_get_byte_chunks_when_packet_size_is_smaller():
    data = b"my raw byte data stream"
    packet_size = 4
    excepted_chunks = [
        b"my r",
        b"aw b",
        b"yte ",
        b"data",
        b" str",
        b"eamm",
        b"y ra",
        b"w by",
        b"te d",
        b"ata ",
        b"stre",
        b"am",
    ]
    chunks = list(get_packet_sized_byte_chunks(iter([data, data]), packet_size))
    assert chunks == excepted_chunks


def test_get_chunks_when_packet_size_is_larger():
    data = b"my raw byte data stream"
    packet_size = 100
    excepted_chunks = [b"my raw byte data streammy raw byte data stream"]
    chunks = list(get_packet_sized_byte_chunks(iter([data, data]), packet_size))
    assert chunks == excepted_chunks


def test_receiving_different_command_id_in_response_raises_exception(
    reader_providing_different_command_id, writer, handle_response
):
    _command_factory = CommandFactory(writer, reader_providing_different_command_id)

    my_command = _command_factory.create_command(
        command_id=5, packet_size=10, on_receive=handle_response
    )

    with pytest.raises(UnexpectedCommandIdError):
        my_command(provide_data_n_times(1))
