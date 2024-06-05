#ifndef ENV5_FLASH_HEADER
#define ENV5_FLASH_HEADER

#include <stdint.h>

#include "FlashTypedefs.h"

// tag::prototypes[]
/*! @brief read the device/manufacturer ID and Common Flash Interface ID
 * \n\n
 * -> 1-2 Byte: Manufacturer ID \n
 * -> 3-4 Byte: Device ID \n
 * -> 5-?  Byte: CFI ID \n
 *
 * @param dataBuffer buffer to store read out data
 * @return number of read bytes
 */
int flashReadId(flashConfiguration_t *flashConfiguration, data_t *dataBuffer);

/*! @brief read data from the flash storage
 *
 * @param startAddress address where the read starts
 * @param dataBuffer buffer to store read out data
 * @return number of read bytes
 */
int flashReadData(flashConfiguration_t *flashConfiguration, uint32_t startAddress, data_t *dataBuffer);

/*! \brief erases the whole flash memory
 *
 * @return of if noe error occurred
 */
flashErrorCode_t flashEraseAll(flashConfiguration_t *flashConfiguration);

/*! @brief erases sector of the flash
 *
 * @param address address where the sector starts
 * @return 0 if no error occurred
 */
flashErrorCode_t flashEraseSector(flashConfiguration_t *flashConfiguration, uint32_t address);

/*! @brief writes up to one page of bytes to the flash
 *
 * @param startAddress address where the page starts
 *                     -> least significant 9 bits are always zero
 * @param data pointer to the buffer where the data is stored
 * @param bytesToWrite length of the data buffer
 * @return number of bytes written to the flash
 */
int flashWritePage(flashConfiguration_t *flashConfiguration, uint32_t startAddress, uint8_t *data,
                   size_t bytesToWrite);
// end::prototypes[]
#endif /* ENV5_FLASH_HEADER */
