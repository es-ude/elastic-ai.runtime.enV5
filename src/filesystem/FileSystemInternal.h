#ifndef ENV5_FILESYSTEM_INTERNAL_HEADER
#define ENV5_FILESYSTEM_INTERNAL_HEADER

static void writeFileToSector(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig, uint8_t currentSector,
                              uint8_t newSector);
static void fileSystemEntryToByteArray(filesystemConfiguration_t *filesystemConfig, fileSystemEntry_t *data, uint8_t *byteArray, uint8_t index);
static void byteArrayToFileSystemEntry(uint8_t *array[]);
static void printFileSystemEntry(fileSystemEntry_t entry);
static void reconstructEntry(filesystemConfiguration_t *filesystemConfig, uint8_t id, char name[], uint8_t sector, uint32_t size,
                             uint8_t isConfig, uint8_t numberOfSectors);
static void updateEntrySector(filesystemConfiguration_t *filesystemConfig, uint8_t oldSector, uint8_t newSector);
static void inc();
static void printBinary(uint8_t byte);
static void writeFileSystemToFlash(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig);
static int getIndexBySector(filesystemConfiguration_t *filesystemConfig, uint8_t sector);
static bool checkIfFileSystemExists(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig);
static void checkNumberOfEntries(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig);
static void deleteFileSystemEntry(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig, uint8_t index);
static int compareStartSectors(const void *a, const void *b);
static int compareIDs(const void *a, const void *b);

#endif
