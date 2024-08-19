#define SOURCE_FILE "USB-PROTOCOL_DEFAULT_COMMANDS"

#include "internal/DefaultCommands.h"
#include "CException.h"
#include "EnV5HwConfiguration.h"
#include "Gpio.h"
#include "UsbProtocolCustomCommands.h"

//! number of bytes always present in response (command + payload length + checksum)
#define BASE_LENGTH (sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t))

void readSkeletonId(__attribute__((unused)) const uint8_t *data,
                    __attribute((unused)) size_t length) {
    // TODO: implement
}

void getChunkSize(__attribute__((unused)) const uint8_t *data,
                  __attribute((unused)) size_t length) {
    uint32_t payload = __builtin_bswap32(FLASH_BYTES_PER_PAGE);

    usbProtocolMessage_t *msg = usbProtocolCreateMessage(3, (uint8_t *)&payload, sizeof(payload));
    sendHandle(msg->message, msg->length);
    usbProtocolFreeMessageBuffer(msg);
    if (usbProtocolWaitForAcknowledgement()) {
        Throw(USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED);
    }
}

void sendDataToFlash(const uint8_t *data, size_t length) {
    // TODO: implement
}

void readDataFromFlash(__attribute__((unused)) const uint8_t *data,
                       __attribute((unused)) size_t length) {
    // TODO: implement
}

void setFpgaPower(const uint8_t *data, size_t length) {
    // TODO: implement
}

void setFpgaLeds(const uint8_t *data, size_t length) {
    // TODO: implement
}

void setMcuLeds(const uint8_t *data, __attribute((unused)) size_t length) {
    gpioSetPin(LED0_GPIO, data[0] & 0x01 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    gpioSetPin(LED1_GPIO, data[0] & 0x02 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    gpioSetPin(LED2_GPIO, data[0] & 0x04 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
}

void runInference(const uint8_t *data, size_t length) {
    // TODO: implement
}
