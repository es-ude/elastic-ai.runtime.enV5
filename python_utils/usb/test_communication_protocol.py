import pytest
from python_utils.usb.communication_protocol import *

def test_xor_byte():
    a = 255
    b = 254
    actual = xor_byte(a, b)
    expected = 1
    assert actual == expected

def test_checksum():
    actual = calculate_checksum(bytearray(b'\xFF\xFE'))
    expected = bytes(b'\x01')
    assert actual == expected

def test_build_message():
