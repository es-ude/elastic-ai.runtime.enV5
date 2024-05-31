#ifndef ENV5_FPGACONFIG_H
#define ENV5_FPGACONFIG_H

#include "SpiTypedefs.h"
#include "hardware/spi.h"

#define SPI_FPGA_INSTANCE spi0
#define SPI_FPGA_SCK 18
#define SPI_FPGA_MISO 16
#define SPI_FPGA_MOSI 19
#define SPI_FPGA_BAUDRATE 62000000
#define SPI_FPGA_CS 17
extern spiConfiguration_t fpgaSpiConfiguration;


#endif // ENV5_FPGACONFIG_H
