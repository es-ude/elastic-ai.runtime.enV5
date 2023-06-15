#define SOURCE_FILE "FLASH"

#include "Flash.h"
#include "FlashInternal.h"
#include "spi/Spi.h"
#include "Common.h"
#include "FreeRtosTaskWrapper.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static uint8_t flashCsPin;
static spi_inst_t *flashSpi;


void flashInit(uint8_t chip_select, spi_inst_t *spiInst) {
    flashCsPin = chip_select;
    flashSpi = spiInst;
}

void flashWriteEnable(void) {
    uint8_t cmd = 0x06;
    spiWriteSingleCmd(flashSpi, flashCsPin, &cmd, 1);
}

int flashReadId(uint8_t *data_buffer, uint16_t length) {
    uint8_t cmd = 0x9F;
    return spiReadBlocking(flashSpi, flashCsPin, &cmd, 1, data_buffer, length);
}

int flashReadData(uint32_t address, uint8_t *data_buffer, uint16_t length) {
    uint8_t cmd[4] = {0x03, address >> 16, address >> 8, address};
    return spiReadBlocking(flashSpi, flashCsPin, cmd, 4, data_buffer, length);
}

uint8_t flashReadStatusReg(void) {
    uint8_t statusReg;
    uint8_t readStatusRegister = 0x05;
    spiReadBlocking(flashSpi, flashCsPin, &readStatusRegister, 1, &statusReg, 1);
    return statusReg;
}

uint8_t flashEraseErrorOccurred(void) {
    flashEraseErrorCode_t flashEraseErrorCode = ((flashReadStatusReg() >> 5) & 1);
    return flashEraseErrorCode;
}

void flashWaitForDone(void) {
    uint8_t statusReg;
    do {
        statusReg = flashReadStatusReg();
    } while (statusReg & 1);
}
uint8_t flashEraseData(uint32_t address) {
    uint8_t cmd[4] = {0xD8, address >> 16, address >> 8, address};
    flashWriteEnable();
    spiWriteSingleCmd(flashSpi, flashCsPin, cmd, 4);

    flashWaitForDone();
    flashEraseErrorCode_t status = flashEraseErrorOccurred();
    return status;
}

flashWriteError_t flashWritePage(uint32_t address, uint8_t *data, uint16_t data_size) {
    PRINT("FLash write page address %u and size: %u",address, data_size)
    uint8_t cmd[4] = {0x02, address >> 16, address >> 8, address};
    flashWriteEnable();
    int dataWrote = spiWriteBlocking(flashSpi, flashCsPin, cmd, 4, data, data_size);
    flashWaitForDone();
    if(dataWrote!=data_size){
        return FLASH_WRITE_ERROR;
    }
    return FLASH_WRITE_SUCCEED;
}

flashWriteError_t flashWrite( uint32_t address, dataBlock_t* dataBlock) {
//    PRINT("flash write, address %u and datablock size %u", configuration->address,
//          configuration->dataBlock->size)
    file_t* file = initFile( address, dataBlock, FLASH_PAGE_SIZE);
    PRINT("file content: bytes left %u, address %u chunk size %u", file->bytes_left,
          file->address, file->chunk_size)

 
    while (!(file->isEOF(file))) {
        PRINT("First chunk")
        configurationBlock_t* chunk = file->next(file);
        flashWriteError_t writeError = flashWritePage(chunk->address,
                                                      chunk->dataBlock->data,
                                                      chunk->dataBlock->size);
        if (writeError == FLASH_WRITE_ERROR) {
            PRINT("Flash write error")
            freeConfBlock(chunk);
            return writeError;
        }
        freeConfBlock(chunk);
    }
    file->freeFile(file);
    return FLASH_WRITE_SUCCEED;
}

// flashWriteError_t flashWrite(uint32_t address, dataBlock_t * data){
//     uint32_t bytesWritten=0;
//
//     if(bytesWritten<data->size){
//         uint16_t bytes_left=(data->size-bytesWritten);
//         uint16_t writeLength=(bytes_left>=FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : bytes_left;
//        flashWriteError_t writeError=flashWritePage(address+bytesWritten,
//                                                       &data->data[bytesWritten],
//                                                       writeLength);
//        if(writeError==FLASH_WRITE_ERROR){
//            return writeError;
//        }
//
//     }
//}
