#define SOURCE_FILE "FPGA-CONFIGRATION-HANDLER-HTTP"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CException.h"

#include "FpgaConfigurationHandlerInternal.h"
#include "eai/Common.h"
#include "eai/flash/Flash.h"
#include "eai/fpga/FpgaConfigurationHandler.h"
#include "eai/network/HTTP.h"

/* region PUBLIC FUNCTION IMPLEMENTATIONS */

fpgaConfigurationHandlerError_t fpgaConfigurationHandlerDownloadConfigurationViaHttp(
    flashConfiguration_t *flashConfiguration, char *baseUrl, size_t length, uint32_t sectorID) {
    PRINT_DEBUG("LENGTH: %zu", length);
    PRINT_DEBUG("SECTOR 0: %u", sectorID);

    uint32_t startAddress = (sectorID) * (flashConfiguration->bytesPerSector);

    size_t totalNumberOfOccupiedSectors =
        (size_t)ceilf((float)length / (float)(flashConfiguration->bytesPerSector));
    for (size_t sector = 0; sector < totalNumberOfOccupiedSectors; sector++) {
        uint32_t sectorStartAddress = startAddress + sector * (flashConfiguration->bytesPerSector);
        flashEraseSector(flashConfiguration, sectorStartAddress);
    }

    CEXCEPTION_T exception;
    size_t numberOfPages = (size_t)ceilf((float)length / (float)(flashConfiguration->bytesPerPage));
    PRINT_DEBUG("TOTAL PAGES: %zu", numberOfPages);
    size_t page = 0;
    do {
        PRINT_DEBUG("PAGE: %zu", page);

        char *url = fpgaConfigurationHandlerGenerateUrl(flashConfiguration, baseUrl, page);
        PRINT_DEBUG("URL: %s", url);

        HttpResponse_t *httpResponse = NULL;
        Try {
            HTTPGet(url, &httpResponse);

            uint8_t *bitfileChunk = httpResponse->response;
            size_t chunkLength = httpResponse->length;
            uint32_t pageStartAddress = startAddress + (page * (flashConfiguration->bytesPerPage));
            if (flashWritePage(flashConfiguration, pageStartAddress, bitfileChunk, chunkLength) !=
                chunkLength) {
                Throw(FLASH_ERASE_ERROR);
            }

            fpgaConfigurationHandlerFreeUrl(url);
            HTTPCleanResponseBuffer(httpResponse);
            page++;
        }
        Catch(exception) {
            PRINT("Error during Download occurred (0x%02X)", exception);
            fpgaConfigurationHandlerFreeUrl(url);
            HTTPCleanResponseBuffer(httpResponse);
            if (exception == HTTP_CONNECTION_FAILED || exception == HTTP_URL_TO_LONG) {
                return FPGA_RECONFIG_NETWORK_ERROR;
            }
        }
    } while (page < numberOfPages);

    return FPGA_RECONFIG_NO_ERROR;
}

/* endregion PUBLIC FUNCTION IMPLEMENTATIONS */

/* region INTERNAL FUNCTION IMPLEMENTATIONS */

static char *fpgaConfigurationHandlerGenerateUrl(flashConfiguration_t *flashConfiguration,
                                                 char *baseUrl, size_t page) {
    char *url = malloc(strlen(baseUrl) + 36 * sizeof(char));
    sprintf(url, "%s?chunkNumber=%zu&chunkMaxSize=%u", baseUrl, page,
            flashConfiguration->bytesPerPage);
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
