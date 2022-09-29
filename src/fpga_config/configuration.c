
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "flash.h"
#include"config_data_handler.h"
#include "configuration.h"
#include <stdio.h>

#define BUFFER_SIZE 256

uint32_t configAddress, configSize;
uint8_t *buffer, *verifyBuffer;
void printBuffer(uint8_t* buffer, uint16_t length){
    for (int i = 0; i < length; i++) {
        printf("%u", buffer[i]);

    }
    printf("\n");

}
void debugPrintFlashAfterErase(uint8_t eraseStatus, uint16_t blockCounter, uint32_t blockAddress){
    uint8_t *eraseTest= (uint8_t *) malloc(BUFFER_SIZE);
    printf("error occured: %u , block: %u , address: %x\n",eraseStatus, blockCounter, blockAddress);
    flash_read_data(blockAddress, eraseTest, BUFFER_SIZE);
    printBuffer(eraseTest, BUFFER_SIZE);

    flash_read_data((blockAddress+65536-256), eraseTest, BUFFER_SIZE);
    printBuffer(eraseTest, BUFFER_SIZE);
    free(eraseTest);
}
void eraseSectors(){
    uint16_t numBlocks64K = ceilf((float) (configSize) / 65536.0);

    printf("%u\n",numBlocks64K);
    uint32_t blockAddress;
    for (uint16_t blockCounter = 0; blockCounter < numBlocks64K; blockCounter++) {
        blockAddress = configAddress + ((uint32_t) blockCounter) * 65536;
        uint8_t status=flash_erase_data(blockAddress);
        debugPrintFlashAfterErase(status, blockCounter,blockAddress);
    }

}
void fillBufferWithDebugData(uint8_t* buffer, uint16_t buffer_length){
    for(uint32_t i=0; i<buffer_length;i=i+4){
        buffer[i]=0xD;
        buffer[i+1]=0xE;
        buffer[i+2]=0xA;
        buffer[i+3]=0xD;

    }
}

void configurationFlash() {

    // getting address
 //   printf("readValue\n");
    readValue(&configAddress);
  //  printf("after read value\n");
    printf("%lu\n",configAddress);

    uint32_t configRemaining;


    //getting size of file
    readValue(&configSize);

    printf("%lu\n",configSize);
    buffer = (uint8_t *) malloc(BUFFER_SIZE);

    //flash_enable

    uint16_t blockSize = BUFFER_SIZE;


    fillBufferWithDebugData(buffer, blockSize);


    eraseSectors();



    printf("ack\n");

    uint32_t currentAddress = configAddress;

    configRemaining = configSize;


    while (configRemaining > 0) {
        if (configRemaining < BUFFER_SIZE) {
            blockSize = configRemaining;
            printf("last block address: %x, blockSize: %u\n", currentAddress, blockSize);
        }
        readData(buffer, blockSize);
        for (uint32_t i=0; i<blockSize; i++) {
            // putchar(verifyBuffer[i]);
            printf("%u", buffer[i]);
        }
        printf("\n");
        flash_write_page(currentAddress, buffer, blockSize);

        currentAddress += blockSize;
        configRemaining -= blockSize;

        fillBufferWithDebugData(buffer, BUFFER_SIZE);
        printf("ack\n");

    }
    free(buffer);
    printf("ack\n");
}


void verifyConfigurationFlash() {
    configAddress=0;
    uint32_t configRemaining;
    verifyBuffer = (uint8_t *) malloc(256);
    fillBufferWithDebugData(verifyBuffer, BUFFER_SIZE);
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
        printf("address %lx ; blockSize: %u\n",currentAddress, blockSize);
        flash_read_data(currentAddress, verifyBuffer, blockSize);
        currentAddress += blockSize;
        configRemaining -= blockSize;

        //TaskSleep(100);
        for (uint32_t i=0; i<blockSize; i++){
           printf("%u###",verifyBuffer[i]);

        }
        printf("\n");
        printf("ack\n");
        fillBufferWithDebugData(verifyBuffer, BUFFER_SIZE);
    }
    free(verifyBuffer);
    printf("ack\n");

}

