#define SOURCE_FILE "FPGA-CONFIG"


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
    uint8_t fpgaConfigurationBuffer[FLASH_PAGE_SIZE];

    fpgaConfigurationInternalGetBitfileWriteArguments(&flashAddress, &configSize);
    fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationBuffer, FLASH_PAGE_SIZE);

    if(fpgaConfigurationInternalEraseSectors(flashAddress, configSize)){
        PRINT("Erasing the flash sectors has failed")
        return;
    }
    PRINT("Erasing the flash sectors succeeded");
    fpgaConfigHandlerSendAck();

    fpgaConfigurationInternalPipeConfigToAndFromFlash( &fpgaConfigurationInternalReceiveAndWriteDataOnFlash, fpgaConfigurationBuffer,flashAddress, configSize);
    fpgaConfigHandlerSendAck();
}

void fpgaConfigurationVerifyConfiguration() {
    uint32_t flashAddress, configSize;
    uint8_t fpgaConfigurationVerifyBuffer[FLASH_PAGE_SIZE];

    fpgaConfigurationInternalGetBitfileWriteArguments(&flashAddress, &configSize);
    fpgaConfigurationInternalFillBufferWithDebugData(fpgaConfigurationVerifyBuffer, FLASH_PAGE_SIZE);
    fpgaConfigurationInternalPipeConfigToAndFromFlash( &fpgaConfigurationInternalReadDataOnFlash, fpgaConfigurationVerifyBuffer,flashAddress, configSize);
    fpgaConfigHandlerSendAck();
}


void fpgaConfigurationInternalReceiveAndWriteDataOnFlash(uint32_t currentAddress, uint8_t* configurationBuffer, uint16_t blockSize){
    fpgaConfigHandlerReceiveData ( configurationBuffer, blockSize );
    flashWritePage ( currentAddress, configurationBuffer, blockSize );
}

void fpgaConfigurationInternalReadDataOnFlash(uint32_t currentAddress, uint8_t* configurationBuffer, uint16_t blockSize){
    flashReadData(currentAddress, configurationBuffer, blockSize);
    PRINT_DEBUG("address %x ; blockSize: %u", currentAddress, blockSize)
    fpgaConfigHandlerSendData (configurationBuffer,blockSize);
}
void fpgaConfigurationInternalPipeConfigToAndFromFlash( void (*readerWriter) (uint32_t, uint8_t*, uint16_t),
                                                        uint8_t * configurationBuffer,
                                                        uint32_t flashAddress, uint32_t configSize)
  {
    uint32_t currentAddress  = flashAddress;
    uint32_t amountRemaining = configSize;
    uint16_t blockSize = FLASH_PAGE_SIZE;
    while (amountRemaining > 0) {
        if (amountRemaining < FLASH_PAGE_SIZE) {
            blockSize = amountRemaining;
            PRINT("last block address: %x, blockSize: %u", currentAddress, blockSize)
        }
        
        readerWriter(currentAddress, configurationBuffer, blockSize);
        currentAddress += blockSize;
        amountRemaining -= blockSize;
        fpgaConfigHandlerSendAck();
        
        fpgaConfigurationInternalFillBufferWithDebugData(configurationBuffer, FLASH_PAGE_SIZE);
    }
  }

uint8_t fpgaConfigurationInternalEraseSectors( uint32_t fpgaConfigurationConfigAddress,uint32_t fpgaConfigurationConfigSize) {
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

void fpgaConfigurationInternalPrintBuffer(uint8_t *buffer, uint16_t length) {
    for (int index = 0; index < length; index++) {
        printf("%u", buffer[index]);
    }
    printf("\n");
}


