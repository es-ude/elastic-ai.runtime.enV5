#ifndef ENV5_FLASH_HEADER
#define ENV5_FLASH_HEADER

#include <stdint.h>

#include "FlashTypedefs.h"
#include "SpiTypedefs.h"

// tag::prototypes[]
//! depends on part number
#define FLASH_BYTES_PER_PAGE 512

//! Each sector consists of 256kB (= 262144B)
#define FLASH_BYTES_PER_SECTOR 262144

void flashInit(spi_t *spiConfiguration, uint8_t chipSelectPin);

/*! @brief read the device/manufacturer ID and Common Flash Interface ID
 * \n\n
 * -> 1-2 Byte: Manufacturer ID \n
 * -> 3-4 Byte: Device ID \n
 * -> 5-?  Byte: CFI ID \n
 *
 * @param dataBuffer buffer to store read out data
 * @return number of read bytes
 */
int flashReadId(data_t *dataBuffer);

/*! @brief read data from the flash storage
 *
 * @param startAddress address where the read starts
 * @param dataBuffer buffer to store read out data
 * @return number of read bytes
 */
int flashReadData(uint32_t startAddress, data_t *dataBuffer);

/*! \brief erases the whole flash memory
 *
 * @return of if noe error occurred
 */
flashErrorCode_t flashEraseAll(void);

/*! @brief erases sector of the flash
 *
 * @param address address where the sector starts
 * @return 0 if no error occurred
 */
flashErrorCode_t flashEraseSector(uint32_t address);

/*! @brief writes up to one page of bytes to the flash
 *
 * @param startAddress address where the page starts
 *                     -> least significant 9 bits are always zero
 * @param data pointer to the buffer where the data is stored
 * @param bytesToWrite length of the data buffer
 * @return number of bytes written to the flash
 */
int flashWritePage(uint32_t startAddress, uint8_t *data, size_t bytesToWrite);
// end::prototypes[]
#endif /* ENV5_FLASH_HEADER */
