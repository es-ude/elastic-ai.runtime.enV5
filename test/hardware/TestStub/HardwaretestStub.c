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
#include "Sleep.h"
#include "echo_server.h"
#include "enV5HwController.h"
#include "middleware.h"

#include <hardware/spi.h>
#include <pico/stdlib.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static networkCredentials_t networkCredentials = {.ssid = "ES-Stud", .password = "curjeq343j"};

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

char baseUrl[] = "http://192.168.203.99:5000/getecho";
size_t configSize = 219412;
uint32_t configStartAddress = 0x00000000;

static void initHardware() {
    // should always be called first thing to prevent unique behavior, like current leakage
    env5HwInit();

    // Always release flash after use.
    // FPGA and MCU share the bus to flash-memory.
    // Make sure this is only enabled while FPGA does not use it and
    // release after use before powering on,
    // resetting or changing the configuration of the FPGA.
    // FPGA needs that bus during reconfiguration and only during reconfiguration.
    flashInit(&spiConfiguration, csPin);

    // initialize the serial output
    stdio_init_all();
    // wait for serial connection
    while ((!stdio_usb_connected())) {}
}

static void connectToWifi() {
    espInit();
    PRINT("Try Connecting to WiFi")
    networkTryToConnectToNetworkUntilSuccessful(networkCredentials);
}

static void loadConfigToFlashViaUSB() {
    PRINT("Downloading HW configuration...")
    fpgaConfigurationHandlerError_t error =
        fpgaConfigurationHandlerDownloadConfigurationViaUsb(configStartAddress);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        while (true) {
            PRINT("Download failed!")
            sleep_ms(3000);
        }
    }
    PRINT("done.")
}
static void loadConfigToFlashViaHttp() {
    PRINT("Downloading HW configuration...")
    fpgaConfigurationHandlerError_t error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(
        baseUrl, configSize, configStartAddress);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        while (true) {
            PRINT("Download failed!")
            sleep_for_ms(3000);
        }
    }
    PRINT("done.")
}

static void deployConfig() {
    env5HwFpgaPowersOn();
    sleep_ms(1000);
    //    bool deploySuccessful = echo_server_deploy();
    //    if (!deploySuccessful) {
    //        while (true) {
    //            PRINT("Deploy failed!")
    //            sleep_for_ms(3000);
    //        }
    //    }
}

static void runTest() {
    env5HwLedsAllOn();
    for (int8_t counter = 0; counter < 10; counter++) {
        int32_t in_value = 1 << 18;
        in_value = counter + in_value;
        PRINT("Calling HW function with 0x%08lX, %li", in_value, in_value)
        int32_t return_val = echo_server_echo(in_value);
        PRINT("HW function returned 0x%08lX, %li", return_val, return_val)

        if (return_val == in_value + counter) {
            env5HwLedsAllOff();
            sleep_for_ms(500);
        }
        env5HwLedsAllOn();
        sleep_for_ms(500);
    }
}

int main() {
    initHardware();
    //    connectToWifi();
    //    loadConfigToFlashViaHttp();
    //    loadConfigToFlashViaUSB();
    deployConfig();

    runTest();

    while (true) {}
}
