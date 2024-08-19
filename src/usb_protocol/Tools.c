#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "CException.h"

#include "UsbProtocolBase.h"
#include "UsbProtocolCustomCommands.h"
#include "internal/Tools.h"

void readBytes(uint8_t *data, size_t numberOfBytes) {
    if (readHandle(data, numberOfBytes) != USB_PROTOCOL_OKAY) {
        Throw(USB_PROTOCOL_ERROR_READ_FAILED);
    }
}

uint8_t calculateChecksum(int numberOfArguments, va_list data) {
    uint8_t checksum = 0x00;
    for (size_t input = 0; input < numberOfArguments; input += 2) {
        uint8_t *byteArray = va_arg(data, uint8_t *);
        size_t byteArrayLength = va_arg(data, size_t);

        for (size_t index = 0; index < byteArrayLength; index++) {
            checksum ^= byteArray[index];
        }
    }

    return checksum;
}
uint8_t getChecksum(int numberOfArguments, ...) {
    va_list data;
    va_start(data, numberOfArguments);
    uint8_t actualChecksum = calculateChecksum(numberOfArguments, data);
    va_end(data);

    return actualChecksum;
}
