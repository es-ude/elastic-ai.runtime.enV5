//! This HW-test requires matching fpga configurations and middleware implementations

#define SOURCE_FILE "MIDDLEWARE-HWTEST"

#include <stdint.h>

#include "pico/stdio.h"
#include "pico/stdio_usb.h"

#include "Common.h"
#include "middleware.h"

int main() {
    stdio_init_all();
    while ((!stdio_usb_connected())) {}

    // TODO: setup Hardware

    while (1) {
        char c = getchar_timeout_us(UINT32_MAX);

        switch (c) {
        case 'i':
            // TODO: init FPGA (power on)
            break;
        case 'd':
            // TODO: deinit FPGA (power off)
            break;
        case 'r':
            // TODO: reset FPGA
            break;
        case 'p':
            // TODO: reconfigure FPGA from 0x000000
            break;
        case 'P':
            // TODO: reconfigure FPGA from 0x001000
            break;
        case 'L':
            // TODO: enable FPGA LEDs
            break;
        case 'l':
            // TODO: disable FPGA LEDs
            break;
        case 't':
            // TODO: run receive data test
            break;
        default:
            PRINT("Waiting ...")
        }
    }
}
