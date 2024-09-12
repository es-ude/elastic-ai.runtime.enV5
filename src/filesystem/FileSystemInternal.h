#ifndef ENV5_FILESYSTEM_INTERNAL_HEADER
#define ENV5_FILESYSTEM_INTERNAL_HEADER

static void writeFileToSector(filesystemConfiguration_t *filesystemConfig, uint8_t currentSector,
                              uint8_t newSector);
static void fileSystemEntryToByteArray(const fileSystemEntry_t *data, uint8_t *byteArray, uint8_t index);
static void byteArrayToFileSystemEntry(uint8_t *array[]);
static void reconstructEntry(filesystemConfiguration_t *filesystemConfig, uint8_t id, uint8_t sector, uint32_t size,
                             uint8_t isConfig, uint8_t numberOfSectors);
static void updateEntrySector(filesystemConfiguration_t *filesystemConfig, uint8_t oldSector, uint8_t newSector);
static void inc();
static void writeFileSystemToFlash(filesystemConfiguration_t *filesystemConfig);
static int getIndexBySector(const filesystemConfiguration_t *filesystemConfig, uint8_t sector);
static bool checkIfFileSystemExists(filesystemConfiguration_t *filesystemConfig);
static void checkNumberOfEntries(filesystemConfiguration_t *filesystemConfig);
static void deleteFileSystemEntry(filesystemConfiguration_t *filesystemConfig, uint8_t index);
static int compareStartSectors(const void *a, const void *b);
static int compareIDs(const void *a, const void *b);

#endif
