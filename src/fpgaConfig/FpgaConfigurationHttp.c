#define SOURCE_FILE "FPGA-CONFIG-HTTP"

#include "FpgaConfigurationHttp.h"
#include "Common.h"
#include "Flash.h"
#include "FpgaConfiguration.h"
#include "pico/time.h"
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


HttpResponse_t *(*getData)(uint32_t) = NULL;
uint32_t currentAddress;
uint32_t receiveBufferSize = 1024;
//typedef struct dataBlock{
//    uint8_t * data;
//    uint32_t size;
//}dataBlock_t;
void writeBlock(uint32_t numBlocks, uint32_t numBlock, uint32_t numPageWrites,
                const HttpResponse_t *block);
void cleanup(dataBlock_t *block);
void setCommunication(HttpResponse_t *(*getDataFun)(uint32_t)) {
    getData = getDataFun;
}

uint32_t internalCalculateNumBlocks(uint32_t sizeOfConfiguration, uint32_t blockSize) {
    return ceilf((float)(sizeOfConfiguration) / blockSize);
}

configErrorCode_t configure(uint32_t startAddress, uint32_t sizeOfConfiguration) {
//    PRINT("configure address %u and size %u", startAddress, sizeOfConfiguration)
    if (fpgaConfigurationEraseSectors(startAddress, sizeOfConfiguration) == FLASH_ERASE_ERROR) {
        return CONFIG_ERASE_ERROR;
    }
    uint32_t numBlocks = internalCalculateNumBlocks(sizeOfConfiguration, receiveBufferSize);
//    PRINT("Num blocks %u", numBlocks)
    currentAddress = startAddress;
    uint32_t numBlock = 0;
    while (numBlock < numBlocks) {
        dataBlock_t * block;
        block =(dataBlock_t*) getData(numBlock);
//        PRINT("block size %u", block->size)

        if (numBlock != (numBlocks - 1) && block->size != receiveBufferSize) {
            cleanup(block);
            continue;
        }
        writeConfigurationBlock(currentAddress, block);
        currentAddress+=receiveBufferSize;
        cleanup(block);
        numBlock++;
    }
    return CONFIG_NO_ERROR;
}


void writeConfigurationBlock(uint32_t address, dataBlock_t* dataBlock){
//    PRINT("write configuration block, address %u and datablock size %u", address, dataBlock->size)
// configurationBlock_t configurationBlock={.address=address, .dataBlock=dataBlock};
    flashWrite( address, dataBlock);
}
//void writeBlock(uint32_t numBlocks, uint32_t numBlock, uint32_t numPageWrites,
//                const HttpResponse_t *block) {
//    for (uint32_t i = 0; i < numPageWrites; i++) {
//        if ((numBlock == numBlocks - 1) && (i == numPageWrites - 1)) {
//            uint32_t lastBlockSize = block->length % FLASH_PAGE_SIZE;
//            flashWritePage(currentAddress, &(block->response[i * FLASH_PAGE_SIZE]), lastBlockSize);
//            currentAddress += lastBlockSize;
//        } else {
//            flashWritePage(currentAddress, &(block->response[i * FLASH_PAGE_SIZE]),
//                           FLASH_PAGE_SIZE);
//            currentAddress += FLASH_PAGE_SIZE;
//        }
//    }
//}

void cleanup(dataBlock_t *block) {
    free(block->data);
    free(block);
    block = NULL;
}