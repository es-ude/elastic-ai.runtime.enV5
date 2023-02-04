#define SOURCE_File "FPGA-CONFIG"

#include "FpgaConfiguration.h"
#include "FpgaConfigDatahandler.h"
#include "FpgaConfigurationInternal.h"
#include "Flash.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

uint32_t fpgaConfigurationConfigAddress, fpgaConfigurationConfigSize;
uint8_t *fpgaConfigurationBuffer, *fpgaConfigurationVerifyBuffer;

void fpgaConfigurationInternalPrintBuffer(uint8_t *buffer, uint16_t length) {
    for (int index = 0; index < length; index++) {
        printf("%u", buffer[index]);
    }
    printf("\n");
}

void fpgaConfigurationInternalDebugPrintFlashAfterErase(uint8_t eraseStatus, uint16_t blockCounter,
                                                        uint32_t blockAddress) {
    uint8_t *eraseTest = (uint8_t *)malloc(BUFFER_SIZE);
    printf("error occurred: %u , block: %u , address: %lx\n", eraseStatus, blockCounter,
           blockAddress);
    flashReadData(blockAddress, eraseTest, BUFFER_SIZE);
    fpgaConfigurationInternalPrintBuffer(eraseTest, BUFFER_SIZE);

    flashReadData((blockAddress + 65536 - 256), eraseTest, BUFFER_SIZE);
    fpgaConfigurationInternalPrintBuffer(eraseTest, BUFFER_SIZE);
    free(eraseTest);
}

void fpgaConfigurationInternalEraseSectors() {
    uint16_t numBlocks64K = ceilf((float)(fpgaConfigurationConfigSize) / 65536.0f);

    printf("%u\n", numBlocks64K);
    uint32_t blockAddress;
    for (uint16_t blockCounter = 0; blockCounter < numBlocks64K; blockCounter++) {
        blockAddress = fpgaConfigurationConfigAddress + ((uint32_t)blockCounter) * 65536;
        uint8_t status = flashEraseData(blockAddress);
        fpgaConfigurationInternalDebugPrintFlashAfterErase(status, blockCounter, blockAddress);
    }
}

void fpgaConfigurationInternalFillBufferWithDebugData(uint8_t *buffer, uint16_t buffer_length) {
    for (uint32_t index = 0; index < buffer_length; index = index + 4) {
        buffer[index] = 0xD;
        buffer[index + 1] = 0xE;
        buffer[index + 2] = 0xA;
        buffer[index + 3] = 0xD;
    }
}

void fpgaConfigurationFlashConfiguration() {
    // getting address
    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigAddress);
    printf("%lu\n", fpgaConfigurationConfigAddress);

    uint32_t configRemaining;

    // getting size of file
    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigSize);
    printf("%lu\n", fpgaConfigurationConfigSize);
    fpgaConfigurationBuffer = (uint8_t *)malloc(BUFFER_SIZE);

    uint16_t blockSize = BUFFER_SIZE;
    fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationBuffer, blockSize);
    fpgaConfigurationInternalEraseSectors();
    printf("ack\n");

    uint32_t currentAddress = fpgaConfigurationConfigAddress;
    configRemaining = fpgaConfigurationConfigSize;

    while (configRemaining > 0) {
        if (configRemaining < BUFFER_SIZE) {
            blockSize = configRemaining;
            printf("last block address: %lx, blockSize: %u\n", currentAddress, blockSize);
        }
        fpgaConfigHandlerReadData(fpgaConfigurationBuffer, blockSize);
        for (uint32_t index = 0; index < blockSize; index++) {
            // putchar(fpgaConfigurationVerifyBuffer[index]);
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
    printf("%lu\n", fpgaConfigurationConfigAddress);
    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigSize);
    printf("%lu\n", fpgaConfigurationConfigSize);

    uint32_t currentAddress = fpgaConfigurationConfigAddress;

    configRemaining = fpgaConfigurationConfigSize;

    while (configRemaining > 0) {
        if (configRemaining < BUFFER_SIZE) {
            blockSize = configRemaining;
        }
        printf("address %lx ; blockSize: %u\n", currentAddress, blockSize);
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
