#ifndef ENV5_FILESYSTEM_INTERNAL_HEADER
#define ENV5_FILESYSTEM_INTERNAL_HEADER

#include "eai/flash/FileSystem.h"

/*! @brief Sets all sectors in sectorFree to 1 (free).
 *
 * @param filesystemConfig Config of used filesystem
 */
static void initSectorFree(filesystemConfiguration_t *filesystemConfig);

/*! @brief Sets all used sectors that are occupied by entries in sectorFree to 0 (occupied).
 *
 * @param filesystemConfig Config of used filesystem
 */
static void sectorFreeSetSectorsUsedByEntries(filesystemConfiguration_t *filesystemConfig);

/*! @brief Filesystem init that is used, if existing filesystem is found after reset.
 *
 * @param filesystemConfig Config of used filesystem
 */
static void existingFileSystemInit(filesystemConfiguration_t *filesystemConfig);

/*! @brief Filesystem init that is used, if no filesystem is found after reset.
 *
 * @param filesystemConfig Config of used filesystem
 */
static void noExistingFileSystemInit(filesystemConfiguration_t *filesystemConfig);

/*! @brief Moves file from current sector to new sector.
 *
 * @param filesystemConfig Config of used filesystem
 * @param index Index of entry
 * @param id ID for entry
 */
static void entrySetID(filesystemConfiguration_t *filesystemConfig, uint32_t index, uint32_t id);

/*! @brief Moves file from current sector to new sector.
 *
 * @param filesystemConfig Config of used filesystem
 * @param index Index of entry
 * @param startSector Startsector of file
 */
static void entrySetStartSector(filesystemConfiguration_t *filesystemConfig, uint32_t index,
                                uint32_t startSector);

/*! @brief Moves file from current sector to new sector.
 *
 * @param filesystemConfig Config of used filesystem
 * @param index Index of entry
 * @param size Size of file
 */
static void entrySetSize(filesystemConfiguration_t *filesystemConfig, uint32_t index, size_t size);

/*! @brief Moves file from current sector to new sector.
 *
 * @param filesystemConfig Config of used filesystem
 * @param index Index of entry
 * @param isConfig Set according to file. Check isConfig_t for correct values.
 */
static void entrySetIsConfig(filesystemConfiguration_t *filesystemConfig, uint32_t index,
                             isConfig_t isConfig);

/*! @brief Moves file from current sector to new sector.
 *
 * @param filesystemConfig Config of used filesystem
 * @param currentSector Current sector of file to be moved
 * @param newSector Sector file should be moved to
 */
static void writeFileToSector(filesystemConfiguration_t *filesystemConfig, uint16_t currentSector,
                              uint16_t newSector);

/*! @brief Converts filesystem entry to byte array
 *
 * @param data Pointer to filesystemEntry_t to be converted to byte array
 * @param byteArray Pointer to array where byte representation should be written to
 * @param index Index of entry to be converted
 */
static void fileSystemEntryToByteArray(const fileSystemEntry_t *data, uint8_t *byteArray,
                                       uint8_t index);

/*! @brief If existing filesystem was found, this fills filesystem datastructure with found entry.
 *
 * @param filesystemConfig Config of used filesystem
 * @param id ID of found entry
 * @param sector Start sector of file
 * @param size Size of file in bytes
 * @param isConfig Flag if file is config (0 = no, 1 = yes, 2 = blocked memory for FPGA
 * @param numberOfSectors Number of sectors required to store file
 */
static void reconstructEntry(filesystemConfiguration_t *filesystemConfig, uint16_t id,
                             uint16_t sector, uint32_t size, isConfig_t isConfig,
                             uint16_t numberOfSectors);

/*! @brief If file is moved to new sector, this updates the corresponding entry in the filesystem.
 *
 * @param filesystemConfig Config of used filesystem
 * @param oldSector Old sector of stored file
 * @param newSector Sector file is stored in now
 */
static void updateEntrySector(filesystemConfiguration_t *filesystemConfig, uint16_t oldSector,
                              uint16_t newSector);

/*! @brief Handles the sector rotation of the filesystem to ensure wear balancing.
 */
static void inc();

/*! @brief Stores filesystem in flash memory.
 *
 * @param filesystemConfig Config of used filesystem
 */
static void writeFileSystemToFlash(filesystemConfiguration_t *filesystemConfig);

/*! @brief Given a start sector of a file, this returns the corresponding index in the filesystem.
 *
 * @param filesystemConfig Config of used filesystem
 * @param sector Start sector of file
 */
static int getIndexBySector(const filesystemConfiguration_t *filesystemConfig, uint16_t sector);

/*! @brief Checks if a filesystem already exists in the flash.
 *
 * @param filesystemConfig Config of used filesystem
 */
static bool checkIfFileSystemExists(filesystemConfiguration_t *filesystemConfig);

/*! @brief If an existing filesystem is found, this checks the number of entries and reconstructs
 * the filesystem datastructure.
 *
 * @param filesystemConfig Config of used filesystem
 */
static void checkNumberOfEntries(filesystemConfiguration_t *filesystemConfig);

/*! @brief Given the index of an entry, this drops that entry and writes the updated filesystem to
 * the flash.
 *
 * @param filesystemConfig Config of used filesystem
 */
static void deleteFileSystemEntry(filesystemConfiguration_t *filesystemConfig, uint8_t index);

/*! @brief Compares start sectors. Used to access the filesystem in different ordering structures.
 *
 * @param a First entry to be compared
 * @param b Second entry to be compared
 */
static int compareStartSectors(const void *a, const void *b);

/*! @brief Compares IDs. Used to access the filesystem in different ordering structures.
 *
 * @param a First entry to be compared
 * @param b Second entry to be compared
 */
static int compareIDs(const void *a, const void *b);

#endif
