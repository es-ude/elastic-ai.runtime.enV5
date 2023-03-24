#define SOURCE_FILE "FPGA-CONFIG-HTTP"

#include "FpgaConfigurationHttp.h"
#include "Common.h"
#include "Flash.h"
#include "FpgaConfiguration.h"
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

HttpResponse_t *(*getData)(uint32_t) = NULL;

void setCommunication(HttpResponse_t *(*getDataFun)(uint32_t)) {
    getData = getDataFun;
}

uint32_t internalCalculateNumBlocks(uint32_t sizeOfConfiguration) {
    return ceilf((float)(sizeOfConfiguration) / FLASH_PAGE_SIZE);
}
configErrorCode_t configure(uint32_t startAddress, uint32_t sizeOfConfiguration) {
    printf("test beginning config\n");
    if (fpgaConfigurationEraseSectors(startAddress, sizeOfConfiguration) == FLASH_ERASE_ERROR) {
        printf("flash erase error\n");
        return CONFIG_ERASE_ERROR;
    }
    printf("Erased\n");
    uint32_t numBlocks = internalCalculateNumBlocks(sizeOfConfiguration);
    PRINT_DEBUG("Number of Blocks: %lu", numBlocks);
    uint32_t currentAddress = startAddress;

    for (uint32_t numBlock = 0; numBlock < numBlocks; numBlock++) {
        HttpResponse_t *block;
        block = getData(numBlock);
        //        if (numBlock == 67) {
        //            printf("current address: %04X\ndata:\n", currentAddress);
        //            for (uint32_t i = 0; i < block->length; i++) {
        //                printf("%02X ", block->response[i]);
        //                if (i != 0 && i % 16 == 0) {
        //                    printf("\n");
        //                }
        //            }
        //            printf("\n");
        //        }
        flashWritePage(currentAddress, block->response, block->length);
        currentAddress += block->length;
        free(block->response);
        free(block);
        block = NULL;
    }
    return CONFIG_NO_ERROR;
}
