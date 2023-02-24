#define SOURCE_FILE "FPGA-CONFIG-HTTP"

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "FpgaConfigurationHttp.h"
#include "FpgaConfiguration.h"
#include "Flash.h"

uint8_t* (*getData)(uint32_t)=NULL;
uint8_t data[256];
void setCommunication(uint8_t* (*getDataFun)(uint32_t)){
    getData=getDataFun;
}

uint32_t internalCalculateNumBlocks(uint32_t sizeOfConfiguration){
    return ceilf((float)(sizeOfConfiguration) / FLASH_PAGE_SIZE);
}
uint8_t* configure(uint32_t startAddress, uint32_t sizeOfConfiguration){
    flashEraseErrorCode_t status=fpgaConfigurationEraseSectors(startAddress, sizeOfConfiguration);
    uint32_t numBlocks= internalCalculateNumBlocks(sizeOfConfiguration);
    uint32_t currentAddress=startAddress;
    uint8_t* block;
    for(uint32_t numBlock=0; numBlock<numBlocks; numBlock++){
        uint8_t* block=getData(numBlock);
        flashWritePage(currentAddress, block, FLASH_PAGE_SIZE);
        currentAddress+=FLASH_PAGE_SIZE;
    }
    return block;

}

uint8_t* dataReceive(uint32_t address){
    for(uint16_t i=0; i<256; i++){
        data[i]=i;
    }
    return data;
}

