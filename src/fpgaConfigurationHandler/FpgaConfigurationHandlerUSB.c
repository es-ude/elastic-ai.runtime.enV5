#define SOURCE_FILE "FPGA-CONFIGRATION-HANDLER-USB"

#include <math.h>
#include <stdio.h>

#include "pico/stdio.h"
#include "pico/stdio_usb.h"

#include "Flash.h"
#include "FpgaConfigurationHandlerInternal.h"
#include "include/FpgaConfigurationHandler.h"

/* ```mermaid
 *  sequenceDiagram
 *      participant c as computer
 *      participant e as enV4
 *      Note over e: wait for input
 *      c ->> e: 's'
 *      e ->> c: 'ack'
 *      c ->> e: total fragment length (uint31_t, big endian)
 *      e ->> c: 'ack'
 *      loop Until Completed
 *          Note over c: wait for fragment ID
 *          e ->> c: fragment ID (uint15_t, hex encoded)
 *          c ->> e: fragment length (uint15_t, big endian)
 *          e ->> c: 'ack'
 *          c ->> e: fragment data
 *          Note over e: store fragement in flash
 *          e ->> c: 'ack'
 *      end
 *      e ->> c: 'o'
 *      Note over e,c: transmission finished
 *  ```
 */

/* region PUBLIC FUNCTION IMPLEMENTATIONS */

fpgaConfigurationHandlerError_t
fpgaConfigurationHandlerDownloadConfigurationViaUsb(uint32_t startAddress) {
    stdio_usb_init();
    while (!stdio_usb_connected()) {}

    fpgaConfigurationHandlerWaitForStartRequest();
    uint32_t totalLength = fpgaConfigurationHandlerGetFileLength();
    fpgaConfigurationHandlerGetChunks(totalLength, startAddress);

    printf("o");
    return FPGA_RECONFIG_NO_ERROR;
}

/* endregion PUBLIC FUNCTION IMPLEMENTATIONS */

/* region INTERNAL FUNCTION IMPLEMENTATIONS */

static void fpgaConfigurationHandlerWaitForStartRequest() {
    char input;
    do {
        input = getchar_timeout_us(UINT32_MAX);
    } while (input != 's');
    printf("ack");
}

static uint32_t fpgaConfigurationHandlerGetFileLength() {
    uint8_t lengthBytes[4];
    for (size_t index = 0; index < sizeof(lengthBytes); index++) {
        lengthBytes[index] = getchar_timeout_us(UINT32_MAX);
    }
    uint32_t length = (uint32_t)lengthBytes[0] << 24 | (uint32_t)lengthBytes[1] << 16 |
                      (uint32_t)lengthBytes[2] << 8 | (uint32_t)lengthBytes[3];
    printf("ack");
    return length;
}

static void fpgaConfigurationHandlerGetChunks(uint32_t totalLength, uint32_t startAddress) {
    size_t numberOfPages = (size_t)ceilf((float)totalLength / FLASH_BYTES_PER_PAGE);
    size_t page = 0;
    while (page < numberOfPages) {
        // send id of fragment
        printf("%04x", page);

        // receive fragment length
        uint8_t fragmentLengthBytes[2];
        for (size_t index = 0; index < sizeof(fragmentLengthBytes); index++) {
            fragmentLengthBytes[index] = getchar_timeout_us(UINT32_MAX);
        }
        uint16_t fragmentLength =
            (uint16_t)fragmentLengthBytes[0] << 8 | (uint16_t)fragmentLengthBytes[1];
        printf("ack");

        // delete page if not all bytes are overwritten
        if (fragmentLength < FLASH_BYTES_PER_PAGE) {
            flashErasePage(startAddress + (page * FLASH_BYTES_PER_PAGE));
        }

        // receive data of fragment
        uint8_t data[fragmentLength];
        for (size_t index = 0; index < fragmentLength; index++) {
            data[index] = getchar_timeout_us(UINT32_MAX);
        }
        // store data to flash
        flashWritePage(startAddress + (page * FLASH_BYTES_PER_PAGE), data, fragmentLength);
        printf("ack");

        page++;
    }
}

/* endregion INTERNAL FUNCTION IMPLEMENTATIONS */
