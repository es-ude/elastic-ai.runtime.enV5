// FIXME: Update Demo.c

#define SOURCE_FILE "FPGA-CONFIGRATION-HANDLER"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CException.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"

#include "Common.h"
#include "Flash.h"
#include "FpgaConfigurationHandlerInternal.h"
#include "HTTP.h"
#include "include/FpgaConfigurationHandler.h"
#include "middleware.h"

/* region PUBLIC FUNCTION IMPLEMENTATIONS */

void fpgaConfigurationHandlerInitialize() {}

fpgaConfigurationHandlerError_t
fpgaConfigurationHandlerDownloadConfigurationViaHttp(char *baseUrl, size_t length,
                                                     uint32_t startAddress) {
    CEXCEPTION_T exception;

    HttpResponse_t *httpResponse = NULL;
    char *url = NULL;

    size_t numberOfPages = (size_t)ceilf((float)length / FLASH_BYTES_PER_PAGE);
    size_t page = 0;
    do {
        Try {
            url = fpgaConfigurationHandlerGenerateUrl(baseUrl, page);
            PRINT_DEBUG("URL: %s", url)

            HTTPGet(url, &httpResponse);
            flashWritePage(startAddress + (page * FLASH_BYTES_PER_PAGE), httpResponse->response,
                           httpResponse->length);
            fpgaConfigurationHandlerFreeUrl(url);
            HTTPCleanResponseBuffer(httpResponse);
            page++;
        }
        Catch(exception) {
            PRINT_DEBUG("Error during Download occurred (0x%02X)", exception)
            fpgaConfigurationHandlerFreeUrl(url);
            HTTPCleanResponseBuffer(httpResponse);
            if (exception == HTTP_CONNECTION_FAILED || exception == HTTP_URL_TO_LONG) {
                return FPGA_RECONFIG_NETWORK_ERROR;
            }
        }
    } while (page < numberOfPages);

    return FPGA_RECONFIG_NO_ERROR;
}

fpgaConfigurationHandlerError_t
fpgaConfigurationHandlerDownloadConfigurationViaUsb(uint32_t startAddress) {
    /* Protocol:
     *   enV5 -> e
     *   computer -> c
     *
     *   e: wait for input
     *   c: send `s`
     *   e: receives `s` => send `ack`
     *   c: receives `ack`
     *   c: sends length as uint32_t (4 bytes, big endian)
     *   e: receives total length of config file => send `ack`
     *   c: receives `ack`
     *   -- send fragment
     *   c: wait for fragment id
     *   e: send fragment id as uint16_t (hex encoded)
     *   c: sends length of fragment as uint16_t (2 bytes, big endian)
     *   e: receives length => send `ack`
     *   c: receives `ack`
     *   c: send config fragment
     *   -- repeat: send fragment
     *   e: send `ack`
     *   c: receives `ack`
     */

    stdio_usb_init();
    while (!stdio_usb_connected()) {}

    // wait for init of data exchange
    char input;
    do {
        input = getchar_timeout_us(UINT32_MAX);
    } while (input != 's');
    printf("ack");

    // receive length of config file
    uint8_t lengthBytes[4];
    for (size_t index = 0; index < sizeof(lengthBytes); index++) {
        lengthBytes[index] = getchar_timeout_us(UINT32_MAX);
    }
    uint32_t length = (uint32_t)lengthBytes[0] << 24 | (uint32_t)lengthBytes[1] << 16 |
                      (uint32_t)lengthBytes[2] << 8 | (uint32_t)lengthBytes[3];
    printf("ack");

    // receive fragments and store to flash
    size_t numberOfPages = (size_t)ceilf((float)length / FLASH_BYTES_PER_PAGE);
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

    printf("o");
    return FPGA_RECONFIG_NO_ERROR;
}

fpgaConfigurationHandlerError_t fpgaConfigurationFlashFpga(uint32_t startAddress) {
    middlewareConfigureFpga(startAddress);
    return FPGA_RECONFIG_NO_ERROR;
}

/* endregion PUBLIC FUNCTION IMPLEMENTATIONS */

/* region INTERNAL FUNCTION IMPLEMENTATIONS */

static char *fpgaConfigurationHandlerGenerateUrl(char *baseUrl, size_t page) {
    char *url = malloc(strlen(baseUrl) + 11 * sizeof(char));
    sprintf(url, "%s/%i", baseUrl, page);
    return url;
}

static void fpgaConfigurationHandlerFreeUrl(char *url) {
    if (url == NULL) {
        return;
    }

    free(url);
    url = NULL;
}

/* endregion INTERNAL FUNCTION IMPLEMENTATIONS */
