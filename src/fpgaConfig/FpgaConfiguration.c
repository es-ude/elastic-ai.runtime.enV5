#define SOURCE_File "FPGA-CONFIG"

#include "FpgaConfiguration.h"
#include "Flash.h"
#include "FpgaConfigDatahandler.h"
#include "FpgaConfigurationInternal.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "Common.h"

/*
 * todo:
 * Different name: BitfileHandler
 *  different name test
 * move usb stuff into serialCom
 * write send function
 * */


uint8_t *fpgaConfigurationBuffer, *fpgaConfigurationVerifyBuffer;

static void fpgaConfigurationInternalGetBitfileWriteArguments(uint32_t *flashAddress, uint32_t *sizeToReceive){
    // getting address
    fpgaConfigHandlerReadValue(flashAddress);
    PRINT_SYNC( "%u", *flashAddress)
    // getting size of file
    fpgaConfigHandlerReadValue(sizeToReceive);
    PRINT_SYNC("%u", *sizeToReceive)
}

void fpgaConfigurationFlashConfiguration() { //TODO: communicate errors back
    uint32_t flashAddress, configSize;
    fpgaConfigurationInternalGetBitfileWriteArguments(&flashAddress, &configSize);

    fpgaConfigurationBuffer = (uint8_t *)malloc(FLASH_PAGE_SIZE); // TODO: turn into array, remove free down below

    uint16_t blockSize = FLASH_PAGE_SIZE;
    fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationBuffer, blockSize);

    if(fpgaConfigurationInternalEraseSectors()){
        PRINT("Erasing the flash sectors has failed");
    }
    PRINT("Erasing the flash sectors succeeded");
    PRINT_SYNC("ack")

    uint32_t currentAddress = flashAddress;
    uint32_t configRemaining = configSize; //TODO: rename amountRemaining

    while (configRemaining > 0) { //TODO: extract into func pipeConfigIntoFlash
        if (configRemaining < FLASH_PAGE_SIZE) {
            blockSize = configRemaining;
            PRINT("last block address: %x, blockSize: %u", currentAddress, blockSize)
        }
        fpgaConfigHandlerReceiveData(fpgaConfigurationBuffer, blockSize);
        flashWritePage(currentAddress, fpgaConfigurationBuffer, blockSize);

        currentAddress += blockSize;
        configRemaining -= blockSize;

        fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationBuffer, FLASH_PAGE_SIZE);
        PRINT_SYNC("ack")
    }
    free(fpgaConfigurationBuffer);
    PRINT_SYNC("ack")
}

void fpgaConfigurationVerifyConfiguration() {
    uint32_t fpgaConfigurationConfigAddress, fpgaConfigurationConfigSize;
    fpgaConfigurationConfigAddress = 0;
    uint32_t configRemaining;
    fpgaConfigurationVerifyBuffer = (uint8_t *)malloc(256);
    fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationVerifyBuffer, FLASH_PAGE_SIZE);

    uint16_t blockSize = FLASH_PAGE_SIZE;

    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigAddress);
    PRINT_SYNC("%u", fpgaConfigurationConfigAddress);
    fpgaConfigHandlerReadValue(&fpgaConfigurationConfigSize);
    PRINT_SYNC("%u", fpgaConfigurationConfigSize);

    uint32_t currentAddress = fpgaConfigurationConfigAddress;

    configRemaining = fpgaConfigurationConfigSize;

    while (configRemaining > 0) {
        if (configRemaining < FLASH_PAGE_SIZE) {
            blockSize = configRemaining;
        }
        PRINT_DEBUG("address %x ; blockSize: %u", currentAddress, blockSize);
        flashReadData(currentAddress, fpgaConfigurationVerifyBuffer, blockSize);
        currentAddress += blockSize;
        configRemaining -= blockSize;

        for (uint32_t i = 0; i < blockSize; i++) {
            SEND_DATA("%u###", fpgaConfigurationVerifyBuffer[i]);
        }
        SEND_DATA("\n");
        PRINT_SYNC("ack");
        fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationVerifyBuffer,
                                                         FLASH_PAGE_SIZE);
    }
    free(fpgaConfigurationVerifyBuffer);
    PRINT_SYNC("ack")
}
void fpgaConfigurationInternalFormatFlashDataForSending(uint16_t blockSize, uint8_t *flashDataBuffer){
    char *formattedFlashData= (char*)malloc(blockSize*sizeof(char));

    for (uint32_t i = 0; i < blockSize; i++) {
        SEND_DATA("%u###", flashDataBuffer[i]);
    }
    SEND_DATA("\n");

}
void fpgaConfigurationInternalPrintBuffer(uint8_t *buffer, uint16_t length) {
    for (int index = 0; index < length; index++) {
        printf("%u", buffer[index]);
    }
    printf("\n");
}

void fpgaConfigurationInternalDebugPrintFlashAfterErase(uint8_t eraseStatus, uint16_t blockCounter,
                                                        uint32_t blockAddress) {
    uint8_t *eraseTest = (uint8_t *)malloc(FLASH_PAGE_SIZE);
    printf("error occurred: %u , block: %u , address: %x\n", eraseStatus, blockCounter,
           blockAddress);
    flashReadData(blockAddress, eraseTest, FLASH_PAGE_SIZE);
    fpgaConfigurationInternalPrintBuffer(eraseTest, FLASH_PAGE_SIZE);

    flashReadData((blockAddress + FLASH_SECTOR_SIZE - FLASH_PAGE_SIZE), eraseTest, FLASH_PAGE_SIZE);
    fpgaConfigurationInternalPrintBuffer(eraseTest, FLASH_PAGE_SIZE);
    free(eraseTest);
}

uint8_t fpgaConfigurationInternalEraseSectors() {
    uint16_t numSectors = ceilf((float)(fpgaConfigurationConfigSize) / FLASH_SECTOR_SIZE);
    uint8_t status=2;
    printf("Number of Sectors:\t\t %u\n", numSectors);
    uint32_t blockAddress;
    for (uint16_t blockCounter = 0; blockCounter < numSectors; blockCounter++) {
        blockAddress = fpgaConfigurationConfigAddress + ((uint32_t)blockCounter) * FLASH_SECTOR_SIZE;
        status = flashEraseData(blockAddress);
        if(status==1){
            return 1;
        }
      // fpgaConfigurationInternalDebugPrintFlashAfterErase(status, blockCounter, blockAddress);
    }
    return status;
}

void fpgaConfigurationInternalFillBufferWithDebugData(uint8_t *buffer, uint16_t buffer_length) {
#ifdef DEBUG
    for (uint32_t index = 0; index < buffer_length; index = index + 4) {
        buffer[index] = 0xD;
        buffer[index + 1] = 0xE;
        buffer[index + 2] = 0xA;
        buffer[index + 3] = 0xD;
    }
#endif
}


