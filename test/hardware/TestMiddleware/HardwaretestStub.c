// This program only works with the logic design with middleware

#define SOURCE_FILE "HWTEST-MIDDLEWARE"

#include "Common.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "echo_server.h"
#include "enV5HwController.h"
#include "middleware.h"

#include <pico/stdlib.h>
#include "hardware/spi.h"

#include <stdbool.h>
#include <stdio.h>

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

static void initHardware() {
    // enable QXI interface to the FPGA
    middlewareInit();

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    // initialize the Flash and FPGA
    flashInit(&spiConfiguration, csPin);
    env5HwInit();
    fpgaConfigurationHandlerInitialize();
}
static void loadConfigToFlash() {
    // TODO: load config to flash
}
static void deployConfig() {
    if (!echo_server_deploy()) {
        while (true) {
            PRINT("Deploy failed!")
            sleep_ms(3000);
        }
    }
}
_Noreturn static void runTest() {
    int8_t counter = 0;
    while (true) {
        uint8_t return_val = (uint8_t)echo_server_echo(counter);

        if (return_val == counter + 1) {
            middlewareSetFpgaLeds(0xFF);
            sleep_ms(500);
        }

        middlewareSetFpgaLeds(0x00);
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
