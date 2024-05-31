#ifndef ENV5_FLASHCONFIG_H
#define ENV5_FLASHCONFIG_H

#include "Flash.h"
#include "hardware/spi.h"

//TODO: checken welches fuer welche Version benoetigt wird
//! depends on part number
#define FLASH_BYTES_PER_PAGE 512
//#define FLASH_BYTES_PER_PAGE 256

//! Each sector consists of 256kB (= 262144B)
#define FLASH_BYTES_PER_SECTOR 262144

#define SPI_FLASH_INSTANCE spi0
#define SPI_FLASH_SCK 2
#define SPI_FLASH_MISO 0
#define SPI_FLASH_MOSI 3
#define SPI_FLASH_BAUDRATE 1000000
#define SPI_FLASH_CS 1
extern const spiConfiguration_t flashSpiConfiguration;
extern const flashConfiguration_t flashConfiguration;



#endif // ENV5_FLASHCONFIG_H
