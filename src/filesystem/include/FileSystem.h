#ifndef ENV5_FILESYSTEM_HEADER
#define ENV5_FILESYSTEM_HEADER

#include <stdbool.h>
#include <stdint.h>

#include "FlashTypedefs.h"

typedef enum isConfig {
    NO_CONFIG = 0,
    CONFIG = 1,
    BLOCKED_FOR_FPGA = 2,
} isConfig_t;

typedef enum filesystemError {
    NO_ERROR = 1,
    FILESYSTEM_ERROR = -1,
} filesystem_error_t;

typedef union fileSystemEntry {
    uint8_t raw[12];
    struct {
        uint32_t size;
        uint16_t id;
        uint16_t startSector;
        isConfig_t isConfig;
        uint16_t numberOfSectors;
    } entry;
} fileSystemEntry_t;

typedef struct filesystemConfiguration {
    flashConfiguration_t *flash;

    uint16_t filesystemStartSector;
    uint16_t filesystemEndSector;
    uint16_t nextFileSystemSector;

    fileSystemEntry_t fileSystem[1020];
    uint8_t sectorFree[1024];

    uint16_t numberOfEntries;
    uint16_t fileID;
    uint16_t numberOfFreeSectors;
    uint16_t numberOfBlockedSectors;
} filesystemConfiguration_t;

/*! @brief Checks if filesystem already exists. If not, a new one is initialized.
 *
 * @param flashConfig Config of used flash
 * @param filesystemConfig Config of used filesystem
 */
void filesystemInit(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig);

/*! @brief Finds fitting start sector for given file length.
 *
 * @param filesystemConfig Config of used filesystem
 * @param numberOfRequiredBytes Length of file in bytes
 * @return Either returns a positive start sector to be used, or returns -1. In this case, there are
 * not enough consecutive free sectors.
 */
int32_t filesystemFindFittingStartSector(const filesystemConfiguration_t *filesystemConfig,
                                         uint32_t numberOfRequiredBytes);

/*! @brief Adds new entry to filesystem and writes updated filesystem to the flash.
 *
 * @param filesystemConfig Config of used filesystem
 * @param startSector Sector where file starts
 * @param size Size of new file. Same value as in FindFittingStartSector.
 * @param isConfig Shows whether this is a config. 0 = no, 1 = yes, 2 = blocked for FPGA.
 * @return Returns pointer to newly added entry.
 */
fileSystemEntry_t* filesystemAddNewFileSystemEntry(filesystemConfiguration_t *filesystemConfig,
                                     uint16_t startSector, uint32_t size, isConfig_t isConfig);

/*! @brief Moves file to new sector and writes updated filesystem to flash.
 *
 * @param filesystemConfig Config of used filesystem
 * @param ID Handle of file to be moved
 * @param newSector Sector the file should be moved to
 */
bool filesystemMoveFileToSector(filesystemConfiguration_t *filesystemConfig, uint16_t ID,
                                uint16_t newSector);

/*! @brief If there is an entry matching the given ID, the corresponding file is deleted and the
 * updated filesystem is written to the flash.
 *
 * @param filesystemConfig Config of used filesystem
 * @param id Handle of file to be deleted
 * @return Returns true if matching entry to given id exists, otherwise returns false.
 */
bool filesystemEraseFileByID(filesystemConfiguration_t *filesystemConfig, uint16_t id);

/*! @brief Returns current number of free sectors.
 *
 * @param filesystemConfig Config of used filesystem
 * @return Number of free sectors
 */
uint32_t filesystemGetNumberOfFreeSectors(const filesystemConfiguration_t *filesystemConfig);

/*! @brief Returns pointer to entry matching given ID.
 *
 * @param filesystemConfig Config of used filesystem
 * @param id Handle of wanted entry
 * @return Returns pointer to filesystemEntry_t
 */
fileSystemEntry_t *filesystemGetEntryByID(filesystemConfiguration_t *filesystemConfig, uint16_t id);

/*! @brief Returns pointer to entry matching given index.
 *
 * @param filesystemConfig Config of used filesystem
 * @param index Index of wanted entry
 * @return Returns pointer to filesystemEntry_t
 */
fileSystemEntry_t *filesystemGetEntryByIndex(filesystemConfiguration_t *filesystemConfig,
                                             uint8_t index);
/*! @brief Returns pointer to entry matching given start sector.
 *
 * @param filesystemConfig Config of used filesystem
 * @param sector Start sector of wanted entry
 * @return Returns pointer to filesystemEntry_t
 */
fileSystemEntry_t *filesystemGetEntryBySector(filesystemConfiguration_t *filesystemConfig,
                                              uint16_t sector);

/*! @brief Sorts filesystem by start sectors in ascending order.
 *
 */
void filesystemSortFileSystemByStartSector();

/*! @brief Sorts filesystem by their IDs in ascending order.
 *
 */
void filesystemSortFileSystemByID();

/*! @brief Blocks consecutive number of bytes, starting from given start sector.
 *
 * @param filesystemConfig Config of used filesystem
 * @param startSector Startpoint where blocked flash area should start
 * @param numberOfBytes Number of bytes to be blocked
 * @return Returns false, if sectors already contain data. Otherwise, returns true.
 */
bool filesystemBlockBytesForFPGA(filesystemConfiguration_t *filesystemConfig, uint16_t startSector,
                                 uint32_t numberOfBytes);

/*! @brief Frees sectors blocked for FPGA.
 *
 * @param filesystemConfig Config of used filesystem
 */
void filesystemFreeBlockedFPGASectors(filesystemConfiguration_t *filesystemConfig);

/*! @brief Erases all entries of current filesystem. Important: This does NOT erase the actual data!
 *
 * @param filesystemConfig Config of used filesystem
 */
void filesystemEraseAllEntries(filesystemConfiguration_t *filesystemConfig);

bool filesystemCheckIfFilesystemExists(filesystemConfiguration_t *filesystemConfig);

#endif // FILESYSTEM_H
