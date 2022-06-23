//
// Created by Natalie Maman on 25.05.22.
//
#include <math.h>
#include <stdint.h>
#include <malloc.h>
#include "pico/stdlib.h"
#include "flash/flash.h"
#include"config_data_handler.h"
#include "configuration.h"
#include <stdio.h>

#define BUFFER_SIZE 256

uint32_t configAddress, configSize, configRemaining;
uint8_t *buffer;

void configurationFlash() {

    // getting address
  //  readValue(&configAddress);
    uint32_t test =12;
   // printf("%u\n", sizeof(uint32_t));
    //printf("x\n", test);
    printf("%s%s%s%s\n",(char *)(test & 0xff),(char *)((test >> 8) & 0xff),
           (char *)((test >> 16) & 0xff), (char *)((test >> 24) & 0xff));

    //printf("%lu\n",test);




    //getting size of file
   // readValue(&configSize);
//    printf("%u",configSize);
//    buffer = (uint8_t *) malloc(BUFFER_SIZE);
//
//    //flash_enable
//
//    uint16_t blockSize = BUFFER_SIZE;
//
//
//
//    uint16_t numBlocks4K = ceilf((float) (configSize) / 0x1000);
//    //uint16_t numBlocks4K = ceil((float) (configSize) / 0x1000);
//
//
//
//    uint32_t blockAddress;
//    for (uint16_t blockCounter = 0; blockCounter < numBlocks4K; blockCounter++) {
//        blockAddress = configAddress + ((uint32_t) blockCounter) * 0x1000;
//        flash_erase_data(blockAddress);
//
//    }
//  //  debugReady();
//    uint32_t currentAddress = configAddress;
//
//    configRemaining = configSize;
//
//
//    while (configRemaining > 0) {
//        if (configRemaining < BUFFER_SIZE) {
//            blockSize = configRemaining;
//        }
//        readData(buffer, blockSize);
//
//        flash_write_page(currentAddress, buffer, blockSize);
//      //  debugAck(buffer[blockSize - 1]);
//        currentAddress += blockSize;
//        configRemaining -= blockSize;
//    //    debugDone();
//
//    }
//    free(buffer);
 //   debugDone();
}


//void verifyConfigurationFlash(uint8_t mcuFlash) {
//
//    flashEnableInterface();
//
//
//    readValue_internal(&configAddress);  // getting address
//
//    readValue_internal(&configSize);     // getting size
//
//    buffer = readDataFlash(configAddress, configSize, mcuFlash, NULL, NULL);
//
//    debugWriteStringLength(buffer, configSize);
//
//}
