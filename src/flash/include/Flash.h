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

/*! \brief erases 64kB sector of the flash
 *
 * @param address address where the sector starts
 * @return 0 if no error occurred
 */
flashErrorCode_t flashEraseSector(uint32_t address);

/*! \brief erases one page from the flash
 *
 * @param address address of the page
 * @return 0 if no error occurred
 */
flashErrorCode_t flashErasePage(uint32_t address);

int flashWritePage(uint32_t startAddress, uint8_t *data, size_t bytesToWrite);

#endif /* ENV5_FLASH_HEADER */
