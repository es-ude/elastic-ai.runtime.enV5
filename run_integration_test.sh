#!/usr/bin/env bash

cmake --build build/release_rev2/ --target hardware-test_usb-protocol
cp build/release_rev2/test/hardware/TestUsbProtocol/hardware-test_usb-protocol.uf2 /Volumes/RPI-RP2/
sleep 2
python -m pytest python_utils/elasticai/runtime/env5/usb/test/
