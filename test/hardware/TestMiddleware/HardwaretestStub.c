/* IMPORTANT: This program only works with the logic design with middleware!
 *
 * IMPORTANT: To reach access the wifi-network the network credentials have to be updated!
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

#define SOURCE_FILE "HWTEST-MIDDLEWARE"

#include "Common.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "Network.h"
#include "echo_server.h"
#include "enV5HwController.h"
#include "middleware.h"

#include <hardware/spi.h>
#include <pico/stdlib.h>

#include <stdbool.h>
#include <stdio.h>

networkCredentials_t networkCredentials = {.ssid = "SSID", .password = "PASSWD"};

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

char baseUrl[] = "http://192.168.203.99:5000/getfast";
size_t configSize = 86116;
uint32_t configStartAddress = 0x00000000;

static void initHardware() {
    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    // connect to Wi-Fi network
    espInit();
    networkTryToConnectToNetworkUntilSuccessful(networkCredentials);

    // enable QXI interface to the FPGA
    middlewareInit();

    // initialize the Flash and FPGA
    flashInit(&spiConfiguration, csPin);
    env5HwInit();
    fpgaConfigurationHandlerInitialize();
    env5HwFpgaPowersOff();
}
static void loadConfigToFlash() {
    fpgaConfigurationHandlerError_t error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(
        baseUrl, configSize, configStartAddress);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        while (true) {
            PRINT("Download failed!")
            sleep_ms(3000);
        }
    }
}
static void deployConfig() {
    env5HwFpgaPowersOn();
    if (!echo_server_deploy()) {
        while (true) {
            PRINT("Deploy failed!")
            sleep_ms(3000);
        }
    }
}
_Noreturn static void runTest() {
    env5HwLedsAllOn();
    int8_t counter = 0;
    while (true) {
        uint8_t return_val = (uint8_t)echo_server_echo(counter);

        if (return_val == counter + 1) {
            env5HwLedsAllOff();
            sleep_ms(500);
        }

        env5HwLedsAllOn();
        sleep_ms(500);

        counter++;
    }
}

int main() {
    initHardware();
    loadConfigToFlash();
    deployConfig();
    runTest();
}
