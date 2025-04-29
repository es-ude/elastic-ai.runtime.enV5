
def test_xor_byte():
    a = 255
    b = 254
    actual = xor_byte(a, b)
    expected = 1
    assert actual == expected


def test_checksum_1():
    actual = calculate_checksum(bytearray(b'\xFF\xFE\xFF'))
    expected = bytes(b'\xFE')
    assert actual == expected


def test_checksum_2():
    actual = calculate_checksum(bytearray(b'\xFF\xFE\xFE'))
    expected = bytes(b'\xFF')
    assert actual == expected


def test_build_message_1():
    command = 1
    data = bytearray()
    device = serial.Serial()
    comm = EnV5BaseProtocolSerialCommunication(device)

    actual = comm._build_message(command, data)
    expected = bytearray(b'\x01\x00\x00\x00\x00\x01')
    assert actual == expected


def test_build_message_1():
    command = 1
    data = bytearray(b'\x01\x00\x01\x00')
    device = serial.Serial()
    comm = EnV5BaseProtocolSerialCommunication(device)

    actual = comm._build_message(command, data)
    expected = bytearray(b'\x01\x04\x00\x00\x00\x01\x00\x01\x00\x05')
    assert actual == expected



