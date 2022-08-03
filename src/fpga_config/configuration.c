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
    readValue(&configAddress);
    printf("%lu\n",configAddress);




    //getting size of file
    readValue(&configSize);

    printf("%lu\n",configSize);
    buffer = (uint8_t *) malloc(BUFFER_SIZE);

    //flash_enable

    uint16_t blockSize = BUFFER_SIZE;



    uint16_t numBlocks64K = ceilf((float) (configSize) / 64000);
    //uint16_t numBlocks4K = ceil((float) (configSize) / 0x1000);

    uint8_t *eraseTest= (uint8_t *) malloc(BUFFER_SIZE);
    printf("%u\n",numBlocks64K);
    uint32_t blockAddress;
    for (uint16_t blockCounter = 0; blockCounter < numBlocks64K; blockCounter++) {
        blockAddress = configAddress + ((uint32_t) blockCounter) * 64000;
        uint8_t status=flash_erase_data(blockAddress);
        printf("error occured: %u , block: %u \n",status, blockCounter);
        flash_read_data(blockAddress, eraseTest, BUFFER_SIZE);
        for (int i = 0; i < BUFFER_SIZE; i++) {
            printf("%u", eraseTest[i]);

        }

        printf("\n");

        flash_read_data((blockAddress+32000-256), eraseTest, BUFFER_SIZE);
        for (int i = 0; i < BUFFER_SIZE; i++) {
            printf("%u", eraseTest[i]);
        }
        printf("\n");
    }





    printf("ack\n");

    uint32_t currentAddress = configAddress;

    configRemaining = configSize;


//    while (configRemaining > 0) {
//        if (configRemaining < BUFFER_SIZE) {
//            blockSize = configRemaining;
//        }
//        readData(buffer, blockSize);
//
//        flash_write_page(currentAddress, buffer, blockSize);
//    //    printf("%u\n", buffer[blockSize-1]);
//      //  debugAck(buffer[blockSize - 1]);
//        currentAddress += blockSize;
//        configRemaining -= blockSize;
//    //    debugDone();
//        printf("ack\n");
//
//    }
    free(buffer);
    printf("ack\n");
}


void verifyConfigurationFlash() {
    buffer = (uint8_t *) malloc(BUFFER_SIZE);

    //flash_enable

    uint16_t blockSize = BUFFER_SIZE;

    readValue(&configAddress);
    printf("%lu\n",configAddress);
    readValue(&configSize);
    printf("%lu\n",configSize);

    uint32_t currentAddress = configAddress;

    configRemaining = configSize;


    while (configRemaining > 0) {
        if (configRemaining < BUFFER_SIZE) {
            blockSize = configRemaining;
        }
        readData(buffer, blockSize);

        flash_read_data(currentAddress, buffer, blockSize);
        currentAddress += blockSize;
        configRemaining -= blockSize;
        for (uint32_t i=0; i<BUFFER_SIZE; i++){
            printf("%u",buffer[i]);
        }
        printf("\n");

    }
    free(buffer);
    printf("ack\n");

}
