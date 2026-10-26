/* Remote control firmware for the elastic-node.
 *
 * Implements the USB protocol described in `python_utils/usb-protocol/README.md`.
 * A host application (see `../cli`) can then
 *   - power the FPGA on and off,
 *   - switch the MCU and FPGA LEDs,
 *   - write data to and read data from the flash.
 *
 * Own commands can be added with `usbProtocolRegisterCommand` (command ids 128 to 255).
 */

#define SOURCE_FILE "EXAMPLE-USB-PROTOCOL"

#include "hardware/spi.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"

#include "CException.h"

#include "eai/com/UsbProtocolBase.h"
#include "eai/flash/Flash.h"
#include "eai/hal/EnV5HwConfiguration.h"
#include "eai/hal/EnV5HwController.h"
#include "eai/hal/Gpio.h"
#include "eai/hal/Sleep.h"

static spiConfiguration_t spiToFlash = {.spiInstance = FLASH_SPI_MODULE,
                                        .baudrate = FLASH_SPI_BAUDRATE,
                                        .sckPin = FLASH_SPI_CLOCK,
                                        .misoPin = FLASH_SPI_MISO,
                                        .mosiPin = FLASH_SPI_MOSI,
                                        .csPin = FLASH_SPI_CS};
static flashConfiguration_t flashConfig = {.spiConfiguration = &spiToFlash};

/* region USB-Protocol read/send functions */
static usbProtocolErrorCodes_t readByteForProtocol(uint8_t *readBuffer, size_t numOfBytes) {
    for (size_t index = 0; index < numOfBytes; index++) {
        readBuffer[index] = stdio_getchar();
    }
    return USB_PROTOCOL_OKAY;
}

static usbProtocolErrorCodes_t sendBytesForProtocol(uint8_t *sendBuffer, size_t numOfBytes) {
    for (size_t index = 0; index < numOfBytes; index++) {
        stdio_putchar_raw(sendBuffer[index]);
    }
    return USB_PROTOCOL_OKAY;
}
/* endregion USB-Protocol read/send functions */

/* Signals a failed command: all LEDs on for two seconds, then the third LED blinks once per
 * exception code. */
static void signalException(uint exception) {
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
    while (!stdio_usb_connected()) {
        // wait for the host to open the serial connection
    }

    flashInit(&flashConfig);

    usbProtocolInit(readByteForProtocol, sendBytesForProtocol, &flashConfig);
}

_Noreturn static void loop(void) {
    while (true) {
        CEXCEPTION_T exception;
        Try {
            usbProtocolReceiveBuffer received = usbProtocolWaitForCommand();
            usbProtocolHandleCommand(received);
        }
        Catch(exception) {
            signalException(exception);
        }
    }
}

int main(void) {
    initialize();
    loop();
}
