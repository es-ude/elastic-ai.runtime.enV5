#define SOURCE_FILE "HW-TEST_USB-PROTOCOL"

#include "CException.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "EnV5HwController.h"
#include "Sleep.h"
#include "UsbProtocol.h"
#include "UsbProtocolTypedefs.h"

usbProtocolErrorCodes_t readByteForProtocol(uint8_t *readBuffer) {
    *readBuffer = stdio_getchar();
    PRINT_DEBUG("Received: 0x%02X", *readBuffer);
    return USB_PROTOCOL_OKAY;
}

usbProtocolErrorCodes_t sendBytesForProtocol(uint8_t *sendBuffer, size_t numOfBytes) {
    for (size_t index = 0; index < numOfBytes; index++) {
        stdio_putchar_raw(sendBuffer[index]);
        PRINT_DEBUG("Send: 0x%02X", sendBuffer[index]);
    }
    return USB_PROTOCOL_OKAY;
}

static void blinkLED(void) {
    env5HwControllerLedsAllOn();
    sleep_for_ms(500);
    env5HwControllerLedsAllOff();

    sleep_for_ms(500);
    env5HwControllerLedsAllOn();
    sleep_for_ms(500);
    env5HwControllerLedsAllOff();

    sleep_for_ms(500);
    env5HwControllerLedsAllOn();
    sleep_for_ms(500);
    env5HwControllerLedsAllOff();

    sleep_for_ms(500);
    env5HwControllerLedsAllOn();
    sleep_for_ms(500);
    env5HwControllerLedsAllOff();
}

static void initialize(void) {
    env5HwControllerInit();

    stdio_init_all();
    while (!stdio_usb_connected()) {}

    usbProtocolInit(readByteForProtocol, sendBytesForProtocol);
}

_Noreturn static void loop(void) {
    while (true) {
        CEXCEPTION_T exception;
        Try {
            usbProtocolReceiveBuffer received = usbProtocolWaitForCommand();
            usbProtocolHandleCommand(received);
        }
        Catch(exception) {
            blinkLED();
        }
    }
}

int main(void) {
    initialize();
    loop();
}
