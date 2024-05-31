#define SOURCE_FILE "FPGA-CONFIGRATION-HANDLER-HTTP"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CException.h"

#include "Common.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "FpgaConfigurationHandlerInternal.h"
#include "HTTP.h"

/* region PUBLIC FUNCTION IMPLEMENTATIONS */

fpgaConfigurationHandlerError_t
fpgaConfigurationHandlerDownloadConfigurationViaHttp(char *baseUrl, size_t length,
                                                     uint32_t sectorID) {
    PRINT_DEBUG("LENGTH: %zu", length);
    PRINT_DEBUG("SECTOR 0: %lu", sectorID);

    uint32_t startAddress = (sectorID - 1) * FLASH_BYTES_PER_SECTOR;

    size_t totalNumberOfOccupiedSectors = (size_t)ceilf((float)length / FLASH_BYTES_PER_SECTOR);
    for (size_t sector = 0; sector < totalNumberOfOccupiedSectors; sector++) {
        uint32_t sectorStartAddress = startAddress + sector * FLASH_BYTES_PER_SECTOR;
        flashEraseSector(NULL, sectorStartAddress);
    }

    CEXCEPTION_T exception;
    size_t numberOfPages = (size_t)ceilf((float)length / FLASH_BYTES_PER_PAGE);
    PRINT_DEBUG("TOTAL PAGES: %zu", numberOfPages);
    size_t page = 0;
    do {
        PRINT_DEBUG("PAGE: %zu", page);

        char *url = fpgaConfigurationHandlerGenerateUrl(baseUrl, page);
        PRINT_DEBUG("URL: %s", url);

        HttpResponse_t *httpResponse = NULL;
        Try {
            HTTPGet(url, &httpResponse);

            uint8_t *bitfileChunk = httpResponse->response;
            size_t chunkLength = httpResponse->length;
            uint32_t pageStartAddress = startAddress + (page * FLASH_BYTES_PER_PAGE);
            if (flashWritePage(NULL, pageStartAddress, bitfileChunk, chunkLength) != chunkLength) {
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

static char *fpgaConfigurationHandlerGenerateUrl(char *baseUrl, size_t page) {
    char *url = malloc(strlen(baseUrl) + 11 * sizeof(char));
    sprintf(url, "%s/%zu", baseUrl, page);
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
