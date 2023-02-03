#include "Spi.h"
#include "SpiHardwareAdapter.h"
#include <stdint.h>

void spiInit(spi_inst_t *spi, uint32_t baudrate, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
             uint8_t misoPin) {
    spiHardwareAdapterInit(spi, baudrate, csPin, sckPin, mosiPin, misoPin);
}

void spiDeinit(spi_inst_t *spi, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
               uint8_t misoPin) {
    spiHardwareAdapterDeinit(spi, csPin, sckPin, mosiPin, misoPin);
}
int spiWriteBlocking(spi_inst_t *spi, uint8_t csPin, uint8_t* cmd,uint8_t cmd_length, uint8_t*
                                                                                           data,
                     uint16_t
                                                                                           len) {
    spiHardwareAdapterEnable(csPin);
    spiHardwareAdapterWriteBlocking(spi, cmd, cmd_length);
    int numBytesWritten=spiHardwareAdapterWriteBlocking(spi, data, len);
    spiHardwareAdapterDisable(csPin);
    return numBytesWritten;
}

int spiReadBlocking(spi_inst_t *spi,uint8_t csPin, uint8_t* cmd, uint8_t cmd_length, uint8_t*
                                                                      dataRead,
                    uint16_t lengthRead){
    spiHardwareAdapterEnable(csPin);
    spiHardwareAdapterWriteBlocking(spi, cmd, cmd_length);
    int numberOfBlocksRead=spiHardwareAdapterReadBlocking(spi, dataRead,lengthRead);
    spiHardwareAdapterDisable(csPin);
    return numberOfBlocksRead;
}

void spiWriteSingleCmd(spi_inst_t *spi,uint8_t csPin, uint8_t* cmd, uint8_t cmd_length){
    spiHardwareAdapterEnable(csPin);
    spiHardwareAdapterWriteBlocking(spi, cmd, cmd_length);
    spiHardwareAdapterDisable(csPin);
    

}