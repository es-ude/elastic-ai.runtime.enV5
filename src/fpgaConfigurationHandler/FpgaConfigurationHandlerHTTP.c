#define SOURCE_FILE "FPGA-CONFIGRATION-HANDLER-HTTP"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CException.h"

#include "Common.h"
#include "Flash.h"
#include "FpgaConfigurationHandlerInternal.h"
#include "HTTP.h"
#include "include/FpgaConfigurationHandler.h"

/* region PUBLIC FUNCTION IMPLEMENTATIONS */

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

            uint8_t *bitfileChunk = httpResponse->response;
            size_t chunkLength = httpResponse->length;
            flashWritePage(startAddress + (page * FLASH_BYTES_PER_PAGE), bitfileChunk, chunkLength);

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
