#ifndef ENV5_FLASH_INTERNAL_HEADER
#define ENV5_FLASH_INTERNAL_HEADER

#include "FlashTypedefs.h"

static size_t calculateBytesPerSector(const uint8_t *config);

static size_t calculateBytesPerPage(const uint8_t *config);

static size_t calculateBytesInFlash(const uint8_t *config);

/*!
 * \brief flash enable write has to be sent before every write/erase operation
 *        to enable modifications to the data
 */
static void flashEnableWrite(spiConfiguration_t *spiConfig);

static flashErrorCode_t flashEraseErrorOccurred(flashConfiguration_t *flashConfig);

static void flashWaitForDone(flashConfiguration_t *flashConfig);

#endif /* ENV5_FLASH_INTERNAL_HEADER */
