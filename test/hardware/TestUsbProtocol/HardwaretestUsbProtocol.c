#define SOURCE_FILE "HW-TEST_USB-PROTOCOL"

#include "CException.h"
#include "pico/stdlib.h"

#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Gpio.h"
#include "Sleep.h"
#include "UsbProtocolBase.h"

usbProtocolErrorCodes_t readByteForProtocol(uint8_t *readBuffer, size_t numOfBytes) {
    for (size_t index = 0; index < numOfBytes; index++) {
        readBuffer[index] = stdio_getchar();
    }
    return USB_PROTOCOL_OKAY;
}

usbProtocolErrorCodes_t sendBytesForProtocol(uint8_t *sendBuffer, size_t numOfBytes) {
    for (size_t index = 0; index < numOfBytes; index++) {
        stdio_putchar_raw(sendBuffer[index]);
    }
    return USB_PROTOCOL_OKAY;
}

static void blinkLED(uint exception) {
    env5HwControllerLedsAllOff();
    env5HwControllerLedsAllOn();
    sleep_for_ms(2000);
    env5HwControllerLedsAllOff();

    for (uint index = 0; index < exception; index++) {
        sleep_for_ms(500);
        gpioSetPin(LED2_GPIO, GPIO_PIN_HIGH);
        sleep_for_ms(500);
        gpioSetPin(LED2_GPIO, GPIO_PIN_LOW);
    }
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
            blinkLED(exception);
        }
    }
}

int main(void) {
    initialize();
    loop();
}
