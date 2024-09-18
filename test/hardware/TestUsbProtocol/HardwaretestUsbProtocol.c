#define SOURCE_FILE "HW-TEST_USB-PROTOCOL"

#include "CException.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Flash.h"
#include "Gpio.h"
#include "Sleep.h"
#include "UsbProtocolBase.h"
#include "UsbProtocolCustomCommands.h"

spiConfiguration_t spiToFlash = {.spiInstance = FLASH_SPI_MODULE,
                                 .baudrate = FLASH_SPI_BAUDRATE,
                                 .sckPin = FLASH_SPI_CLOCK,
                                 .misoPin = FLASH_SPI_MISO,
                                 .mosiPin = FLASH_SPI_MOSI,
                                 .csPin = FLASH_SPI_CS};
flashConfiguration_t flashConfig = {.spiConfiguration = &spiToFlash};

/* region USB-Protocol Read/Send Function */
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
/* endregion USB-Protocol Read/Send Function */

/* region USB-Protocol Custom Countdown Function */

void countdownHandle(__attribute((unused)) const uint8_t *payload,
                     __attribute((unused)) size_t payloadLength) {
    env5HwControllerLedsAllOff();
    gpioSetPin(LED0_GPIO, GPIO_PIN_HIGH);
    sleep_for_ms(1000);
    gpioSetPin(LED1_GPIO, GPIO_PIN_HIGH);
    sleep_for_ms(1000);
    gpioSetPin(LED2_GPIO, GPIO_PIN_HIGH);
    sleep_for_ms(1000);
    env5HwControllerLedsAllOff();
    sleep_for_ms(100);
    env5HwControllerLedsAllOn();
    sleep_for_ms(100);
    env5HwControllerLedsAllOff();
}

/* endregion USB-Protocol Custom Countdown Function */

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

    flashInit(&flashConfig);

    usbProtocolInit(readByteForProtocol, sendBytesForProtocol, &flashConfig);
    usbProtocolRegisterCommand(241, &countdownHandle);
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
