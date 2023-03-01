#define SOURCE_FILE "FPGA-CONFIG-HTTP"

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "FpgaConfigurationHttp.h"
#include "FpgaConfiguration.h"
#include "Flash.h"


HttpResponse_t* (*getData)(uint32_t)=NULL;

void setCommunication(HttpResponse_t* (*getDataFun)(uint32_t)){
    getData=getDataFun;
}

uint32_t internalCalculateNumBlocks(uint32_t sizeOfConfiguration){
    return ceilf((float)(sizeOfConfiguration) / FLASH_PAGE_SIZE);
}
configErrorCode_t configure(uint32_t startAddress, uint32_t sizeOfConfiguration){
    printf("test beginning config\n");
    if(fpgaConfigurationEraseSectors(startAddress, sizeOfConfiguration)==FLASH_ERASE_ERROR){
        return CONFIG_ERASE_ERROR;
    }
    printf("Erased\n");
    uint32_t numBlocks= internalCalculateNumBlocks(sizeOfConfiguration);
    uint32_t currentAddress=startAddress;
    
    for(uint32_t numBlock=0; numBlock<numBlocks; numBlock++){
        HttpResponse_t * block;
        block=getData(numBlock);
        printf("Of response: %u\n" ,block->response[1] );
        flashWritePage(currentAddress, block->response, block->length);
        currentAddress+=block->length;
        free(block);
    }
    return CONFIG_NO_ERROR;

}



