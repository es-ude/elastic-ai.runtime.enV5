#ifndef ENV5_FLASH_INTERNAL_HEADER
#define ENV5_FLASH_INTERNAL_HEADER

#include "FlashTypedefs.h"

static uint32_t FLASH_BYTES_PER_PAGE;
static uint32_t FLASH_BYTES_PER_SECTOR;
static uint32_t FLASH_NUMBER_OF_SECTORS;
static uint32_t FLASH_NUMBER_OF_BYTES;

static void calculateBytesPerSector(const uint8_t *config);
static void calculateBytesPerPage(const uint8_t *config);
static void calculateBytesInFlash(const uint8_t *config);
static void calculateNumberOfSectors();

/*!
 * \brief flash enable write has to be sent before every write/erase operation
 *        to enable modifications to the data
 */
static void flashEnableWrite(spiConfiguration_t *spiConfig);

static flashErrorCode_t flashEraseErrorOccurred(flashConfiguration_t *flashConfig);

static void flashWaitForDone(flashConfiguration_t *flashConfig);

#endif /* ENV5_FLASH_INTERNAL_HEADER */
