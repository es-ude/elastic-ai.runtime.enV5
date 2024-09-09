#ifndef ENV5_FILESYSTEM_HEADER
#define ENV5_FILESYSTEM_HEADER

#include <stdbool.h>
#include <stdint.h>

#include "FlashTypedefs.h"

typedef union fileSystemEntry {
    //check new union for constintency
    uint8_t raw[8];
    struct {
        uint32_t size;
        uint8_t id;
        uint8_t startSector;
        uint8_t isConfig;
        uint8_t numberOfSectors;
    }entry;
}fileSystemEntry_t;

typedef struct filesystemConfiguration {
    uint8_t numberOfEntries;
    uint8_t currentID;

    uint32_t nextFileSystemSector;
    fileSystemEntry_t fileSystem[60];
    uint8_t entrySize;

    uint32_t nextFileSector;
    uint8_t sectorFree[10];
}filesystemConfiguration_t;

void filesystemInit(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig);
int32_t filesystemFindFittingStartSector(filesystemConfiguration_t *filesystemConfig, uint8_t numberOfRequiredSectors);
void filesystemAddNewFileSystemEntry(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig, uint32_t size,
                                     uint8_t isConfig);
void filesystemMoveFileToSector(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig, uint8_t ID, uint8_t newSector);
bool filesystemEraseFileByID(flashConfiguration_t *flashConfig, filesystemConfiguration_t *filesystemConfig, uint8_t id);
void filesystemPrintFileSystem(filesystemConfiguration_t *filesystemConfig);

uint8_t filesystemGetNumberOfFreeSectors();
fileSystemEntry_t *filesystemGetEntryByID(filesystemConfiguration_t *filesystemConfig, uint8_t id);
fileSystemEntry_t *filesystemGetEntryByIndex(filesystemConfiguration_t *filesystemConfig, uint8_t index);
fileSystemEntry_t *filesystemGetEntryBySector(filesystemConfiguration_t *filesystemConfig, uint8_t sector);
void filesystemSortFileSystemByStartSector();
void filesystemSortFileSystemByID();

#endif // FILESYSTEM_H
