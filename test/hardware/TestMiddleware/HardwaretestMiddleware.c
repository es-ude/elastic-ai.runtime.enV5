/* IMPORTANT: This program only works with the logic design with middleware!
 *
 * IMPORTANT: To reach access to the wifi-network the network credentials have to be updated!
 *
 * NOTE: To run this test, a server that serves the HTTPGet request is required.
 *       This server can be started by running the `HW-Test_Webserver.py` script
 *       in the `bitfile_scripts` folder.
 *       After starting the server, it shows an IP-address where it can be reached.
 *       This IP address needs to be used for the `baseUrl` field.
 *
 * NOTE: If you update the echo_server binary file you have to update the `configSize` field
 *       with the correct size of the file in bytes.
 *       This size can be determined by running `du -b <path_to_file>`.
 */

#define SOURCE_FILE "MIDDLEWARE-HWTEST"

#include <stdint.h>
#include <stdlib.h>

#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"

#include "Common.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "Network.h"
#include "Spi.h"
#include "enV5HwController.h"
#include "middleware.h"

extern networkCredentials_t networkCredentials;

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

char baseUrl[] = "http://127.0.0.1:5000/getecho";
size_t echo_length = 175384;
uint32_t sectorIdForConfig = 1;

static void initHardware(void) {
    // Should always be called first thing to prevent unique behavior, like current leakage
    env5HwInit();

    /* Always release flash after use:
     *   -> FPGA and MCU share the bus to flash-memory.
     * Make sure this is only enabled while FPGA does not use it and release after use before
     * powering on, resetting or changing the configuration of the FPGA.
     * FPGA needs that bus during reconfiguration and **only** during reconfiguration.
     */
    flashInit(&spiConfiguration, csPin);

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }
}

static void downloadBinFile(void) {
    espInit();
    PRINT("Try Connecting to WiFi")
    networkTryToConnectToNetworkUntilSuccessful();

    PRINT("Downloading HW configuration...")
    fpgaConfigurationHandlerError_t error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(
        baseUrl, echo_length, sectorIdForConfig);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        PRINT("Download failed!")
        exit(EXIT_FAILURE);
    }
    PRINT("Download Successful.")
}

void getId(void) {
    PRINT("Request ID")
    uint8_t id = middlewareGetDesignId();
    PRINT("ID: 0x%02X", id)
}

void writeData(void) {
    uint8_t data_write[3] = {0x01, 0x02, 0x03};
    middlewareWriteBlocking(0, data_write, 3);
    PRINT("Data written")
    middlewareStartComputation();
    uint8_t data_read[3];
    middlewareReadBlocking(0, data_read, 1);
    PRINT_BYTE_ARRAY("DATA: ", data_read, 3)
}

_Noreturn static void runTest() {
    PRINT("===== START TEST =====")
    while (1) {
        char c = getchar_timeout_us(UINT32_MAX);

        switch (c) {
        case 'C':
            downloadBinFile();
            break;
        case 'P':
            env5HwFpgaPowersOn();
            PRINT("FPGA powered ON")
            break;
        case 'p':
            env5HwFpgaPowersOff();
            PRINT("FPGA powered OFF")
            break;
        case 'I':
            middlewareInit();
            PRINT("INIT")
            break;
        case 'i':
            middlewareDeinit();
            PRINT("DEINIT")
            break;
        case 'L':
            middlewareSetFpgaLeds(0x09); // ON
            PRINT("LEDS ON")
            break;
        case 'l':
            middlewareSetFpgaLeds(0x00); // OFF
            PRINT("LEDS OFF")
            break;
        case 'U':
            middlewareUserlogicEnable();
            PRINT("Userlogic enabled")
            break;
        case 'u':
            middlewareUserlogicDisable();
            PRINT("Userlogic disabled")
            break;
        case 'd':
            getId();
            break;
        case 'r':
            // TODO: run reconfigure test
            break;
        case 't':
            writeData();
            break;
        default:
            PRINT("Waiting ...")
        }
    }
}

int main() {
    initHardware();
    runTest();
}
