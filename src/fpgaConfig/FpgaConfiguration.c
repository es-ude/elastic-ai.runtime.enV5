#define SOURCE_File "FPGA-CONFIG"

#include "FpgaConfiguration.h"
#include "Flash.h"
#include "FpgaConfigDatahandler.h"
#include "FpgaConfigurationInternal.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
#define SECTOR_SIZE 65536

uint32_t fpgaConfigurationConfigAddress, fpgaConfigurationConfigSize;
uint8_t *fpgaConfigurationBuffer, *fpgaConfigurationVerifyBuffer;

void fpgaConfigurationFlashConfiguration() {
    // getting address
    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigAddress);
    printf("%u\n", fpgaConfigurationConfigAddress);

    uint32_t configRemaining;

    // getting size of file
    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigSize);
    printf("%u\n", fpgaConfigurationConfigSize);
    fpgaConfigurationBuffer = (uint8_t *)malloc(BUFFER_SIZE);

    uint16_t blockSize = BUFFER_SIZE;
    fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationBuffer, blockSize);
    uint8_t erase_error=  fpgaConfigurationInternalEraseSectors();
    if(erase_error){
        printf("Erase failed\n");
    }else{
        printf("Erase succeeded\n");
    }
    printf("ack\n");

    uint32_t currentAddress = fpgaConfigurationConfigAddress;
    configRemaining = fpgaConfigurationConfigSize;

    while (configRemaining > 0) {
        if (configRemaining < BUFFER_SIZE) {
            blockSize = configRemaining;
            printf("last block address: %x, blockSize: %u\n", currentAddress, blockSize);
        }
        fpgaConfigHandlerReadData(fpgaConfigurationBuffer, blockSize);
        for (uint32_t index = 0; index < blockSize; index++) {
            printf("%u", fpgaConfigurationBuffer[index]);
        }
        printf("\n");
        flashWritePage(currentAddress, fpgaConfigurationBuffer, blockSize);

        currentAddress += blockSize;
        configRemaining -= blockSize;

        fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationBuffer, BUFFER_SIZE);
        printf("ack\n");
    }
    free(fpgaConfigurationBuffer);
    printf("ack\n");
}

void fpgaConfigurationVerifyConfiguration() {
    fpgaConfigurationConfigAddress = 0;
    uint32_t configRemaining;
    fpgaConfigurationVerifyBuffer = (uint8_t *)malloc(256);
    fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationVerifyBuffer, BUFFER_SIZE);

    uint16_t blockSize = BUFFER_SIZE;

    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigAddress);
    printf("%u\n", fpgaConfigurationConfigAddress);
    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigSize);
    printf("%u\n", fpgaConfigurationConfigSize);

    uint32_t currentAddress = fpgaConfigurationConfigAddress;

    configRemaining = fpgaConfigurationConfigSize;

    while (configRemaining > 0) {
        if (configRemaining < BUFFER_SIZE) {
            blockSize = configRemaining;
        }
        printf("address %x ; blockSize: %u\n", currentAddress, blockSize);
        flashReadData(currentAddress, fpgaConfigurationVerifyBuffer, blockSize);
        currentAddress += blockSize;
        configRemaining -= blockSize;

        // freeRtosTaskWrapperTaskSleep(100);
        for (uint32_t i = 0; i < blockSize; i++) {
            printf("%u###", fpgaConfigurationVerifyBuffer[i]);
        }
        printf("\n");
        printf("ack\n");
        fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationVerifyBuffer,
                                                         BUFFER_SIZE);
    }
    free(fpgaConfigurationVerifyBuffer);
    printf("ack\n");
}

void fpgaConfigurationInternalPrintBuffer(uint8_t *buffer, uint16_t length) {
    for (int index = 0; index < length; index++) {
        printf("%u", buffer[index]);
    }
    printf("\n");
}

void fpgaConfigurationInternalDebugPrintFlashAfterErase(uint8_t eraseStatus, uint16_t blockCounter,
                                                        uint32_t blockAddress) {
    uint8_t *eraseTest = (uint8_t *)malloc(BUFFER_SIZE);
    printf("error occurred: %u , block: %u , address: %x\n", eraseStatus, blockCounter,
           blockAddress);
    flashReadData(blockAddress, eraseTest, BUFFER_SIZE);
    fpgaConfigurationInternalPrintBuffer(eraseTest, BUFFER_SIZE);

    flashReadData((blockAddress + SECTOR_SIZE - BUFFER_SIZE), eraseTest, BUFFER_SIZE);
    fpgaConfigurationInternalPrintBuffer(eraseTest, BUFFER_SIZE);
    free(eraseTest);
}

uint8_t fpgaConfigurationInternalEraseSectors() {
    uint16_t numSectors = ceilf((float)(fpgaConfigurationConfigSize) / SECTOR_SIZE);
    uint8_t status=2;
    printf("%u\n", numSectors);
    uint32_t blockAddress;
    for (uint16_t blockCounter = 0; blockCounter < numSectors; blockCounter++) {
        blockAddress = fpgaConfigurationConfigAddress + ((uint32_t)blockCounter) * SECTOR_SIZE;
        status = flashEraseData(blockAddress);
        if(status==1){
            return 1;
        }

      // fpgaConfigurationInternalDebugPrintFlashAfterErase(status, blockCounter, blockAddress);
    }
    return status;
}

void fpgaConfigurationInternalFillBufferWithDebugData(uint8_t *buffer, uint16_t buffer_length) {
    for (uint32_t index = 0; index < buffer_length; index = index + 4) {
        buffer[index] = 0xD;
        buffer[index + 1] = 0xE;
        buffer[index + 2] = 0xA;
        buffer[index + 3] = 0xD;
    }
}


