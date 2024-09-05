#ifndef ENV5_FLASH_HEADER
#define ENV5_FLASH_HEADER

#include <stdint.h>

#include "FlashTypedefs.h"
#include "SpiTypedefs.h"

// tag::prototypes[]


/*! @brief initialize all needed definitions when working with the flash
 *
 * @param config configuration of the Flash Chip
 * @return none
 */
void flashInit(spiConfiguration_t *spiToFlashConfig);

uint32_t flashGetBytesPerSector();
uint32_t flashGetBytesPerPage();
uint32_t flashGetNumberOfBytes();
uint32_t flashGetNumberOfSectors();


/*! @brief read number of bytes from the flash config and store inside
 *
 * @param config configuration of the Flash Chip
 * @return none
 */
uint8_t* readConfigByLength(spiConfiguration_t *spiToFlashConfig, uint8_t registerToRead, uint8_t length);

/*! @brief read a configuration register of the flash
 *
 * @param config configuration of the Flash Chip
 * @param registerToRead configuration register to read
 * @param dataBuffer buffer to store read out data
 * @return number of read bytes
 */
int flashReadConfig(spiConfiguration_t *spiToFlashConfig, commands_t registerToRead, data_t *dataBuffer);

/*! @brief write to the config register of the flash
 *
 * @param config configuration of the Flash Chip
 * @param configToWrite configuration to write
 * @param bytesToWrite length of the configuration to write
 * @return number of written bytes
 */
int flashWriteConfig(flashConfiguration_t *config, uint8_t *configToWrite, size_t bytesToWrite);

/*! @brief read data from the flash storage
 *
 * @param config configuration of the Flash Chip
 * @param startAddress address where the read starts
 * @param dataBuffer buffer to store read out data
 * @return number of read bytes
 */
int flashReadData(const flashConfiguration_t *config, uint32_t startAddress, data_t *dataBuffer);

/*! \brief erases the whole flash memory
 *
 * @param config configuration of the Flash Chip
 *
 * @return of if noe error occurred
 */
flashErrorCode_t flashEraseAll(flashConfiguration_t *config);

/*! @brief erases sector of the flash
 *
 * @param config configuration of the Flash Chip
 * @param address address where the sector starts
 * @return 0 if no error occurred
 */
flashErrorCode_t flashEraseSector(flashConfiguration_t *config, uint32_t address);

/*! @brief writes up to one page of bytes to the flash
 *
 * @param config configuration of the Flash Chip
 * @param startAddress address where the page starts
 *                     -> least significant 9 bits are always zero
 * @param data pointer to the buffer where the data is stored
 * @param bytesToWrite length of the data buffer
 * @return number of bytes written to the flash
 */
int flashWritePage(flashConfiguration_t *config, uint32_t startAddress, uint8_t *data,
                   size_t bytesToWrite);
// end::prototypes[]
#endif /* ENV5_FLASH_HEADER */
