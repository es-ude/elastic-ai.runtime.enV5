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

#include <hardware/spi.h>
#include <pico/stdlib.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>


spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

char baseUrl[] = "http://192.168.203.22:5000/getecho";
size_t configSize = 219412;
uint32_t configStartAddress = 0x00000000;

static void connectToWifi() {
    PRINT("connecting to wi-fi...")
    espInit();
    static networkCredentials_t networkCredentials = {.ssid = "ES-Stud", .password = "curjeq343j"};
    networkTryToConnectToNetworkUntilSuccessful(networkCredentials);
    PRINT("done.")
}

static void initHardware() {
    env5HwInit(); // should always be called first thing in your main to prevent weird behaviour,
                  // like current leakage
    
    // configure the Flash and FPGA
    flashInit(&spiConfiguration, csPin); // Always release flash after use,FPGA and MCU share the
    //    bus to
    //    flash memory, make sure
    // this is only enabled while FPGA does not use it and release after use before powering on
    // ,resetting or changing the configuration of the FPGA. FPGA needs that bus during
    // reconfiguration and only during reconfiguration.
    
    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }
}

static void loadConfigToFlash() {
    PRINT("Downloading HW configuration...")
    fpgaConfigurationHandlerError_t error = fpgaConfigurationHandlerDownloadConfigurationViaUsb(
        configStartAddress);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        while (true) {
            PRINT("Download failed!")
            sleep_ms(3000);
        }
    }
    PRINT("done.")
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

static void runTest() {
    env5HwLedsAllOn();
    env5HwFpgaPowersOn();
    sleep_ms(1000);
    echo_server_deploy();
    for(int8_t counter = 0; counter < 10; counter++) {
        int32_t in_value = 1 << 18;
        in_value = counter + in_value;
        PRINT("calling HW function with 0x%02lx, %li", in_value, in_value)
        int32_t return_val = echo_server_echo(in_value);
        PRINT("HW function returned 0x%02lx, %li", return_val, return_val)

        if (in_value == counter) {
            env5HwLedsAllOff();
            sleep_ms(500);
        }

        env5HwLedsAllOn();
        sleep_ms(500);
    }
}

int main() {
    initHardware();
//    loadConfigToFlash();
//    deployConfig();
    runTest();
    while (true) {};
}
