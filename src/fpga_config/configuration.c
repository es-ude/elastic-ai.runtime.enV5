//
// Created by Natalie Maman on 25.05.22.
//
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "flash/flash.h"
#include"config_data_handler.h"
#include "configuration.h"
#define BUFFER_SIZE 256

uint32_t configAddress, configSize, configRemaining;
uint8_t *buffer;

void configurationFlash() {

    // getting address
    readValue(&configAddress);
    //getting size of file
    readValue(&configSize);

    buffer = (uint8_t *) malloc(BUFFER_SIZE);

    //flash_enable

    uint16_t blockSize = BUFFER_SIZE;



    uint16_t numBlocks4K = ceilf((float) (configSize) / 0x1000);
    //uint16_t numBlocks4K = ceil((float) (configSize) / 0x1000);



    uint32_t blockAddress;
    for (uint16_t blockCounter = 0; blockCounter < numBlocks4K; blockCounter++) {
        blockAddress = configAddress + ((uint32_t) blockCounter) * 0x1000;
        flash_erase_data(blockAddress);

    }

    uint32_t currentAddress = configAddress;

    configRemaining = configSize;


    while (configRemaining > 0) {
        if (configRemaining < BUFFER_SIZE) {
            blockSize = configRemaining;
        }
        readData(buffer, blockSize);

        flash_write_page(currentAddress, buffer, blockSize);

        currentAddress += blockSize;
        configRemaining -= blockSize;

    }
    free(buffer);
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
