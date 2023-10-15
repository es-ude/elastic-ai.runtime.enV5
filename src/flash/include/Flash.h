#ifndef ENV5_FLASH_HEADER
#define ENV5_FLASH_HEADER

#include <stdint.h>

#include "FlashTypedefs.h"
#include "SpiTypedefs.h"

//! depends on part number
#define FLASH_BYTES_PER_PAGE 512

//! Each sector consists of 256kB (= 262144B)
#define FLASH_BYTES_PER_SECTOR 262144

void flashInit(spi_t *spiConfiguration, uint8_t chipSelectPin);

int flashReadId(data_t *dataBuffer);
int flashReadData(uint32_t startAddress, data_t *dataBuffer);

/*! \brief erases the whole flash memory
 *
 * @return of if noe error occurred
 */
flashErrorCode_t flashEraseAll(void);

/*! \brief erases sector of the flash
 *
 * @param address address where the sector starts
 * @return 0 if no error occurred
 */
flashErrorCode_t flashEraseSector(uint32_t address);

/*! \brief writes up to one page of bytes to the flash
 *
 * @param startAddress address where the page starts
 *                     -> least significant 9 bits are always zero
 * @param data pointer to the buffer where the data is stored
 * @param bytesToWrite length of the data buffer
 * @return number of bytes written to the flash
 */
int flashWritePage(uint32_t startAddress, uint8_t *data, size_t bytesToWrite);

#endif /* ENV5_FLASH_HEADER */
