#ifndef ENV5_FILESYSTEM_HEADER
#define ENV5_FILESYSTEM_HEADER

#include <stdbool.h>
#include <stdint.h>

#include "FlashTypedefs.h"

typedef struct fileSystemEntry {
    uint8_t id;
    uint8_t startSector;
    uint32_t size;
    uint8_t isConfig;
    uint8_t numberOfSectors;
} fileSystemEntry;

extern uint32_t nextFileSector;
extern uint8_t sectorFree[10];

void filesystemInit(flashConfiguration_t flashConfig);
bool filesystemFindFittingStartSector(uint8_t numberOfRequiredSectors);
void filesystemAddNewFileSystemEntry(flashConfiguration_t *flashConfig, uint32_t size,
                                     uint8_t isConfig);
void filesystemMoveFileToSector(flashConfiguration_t *flashConfig, uint8_t ID, uint8_t newSector);
bool filesystemEraseFileByID(flashConfiguration_t *flashConfig, uint8_t id);
void filesystemPrintFileSystem();

uint8_t filesystemGetNumberOfFreeSectors();
fileSystemEntry *filesystemGetEntryByID(uint8_t id);
fileSystemEntry *filesystemGetEntryByIndex(uint8_t index);
fileSystemEntry *filesystemGetEntryBySector(uint8_t sector);
void filesystemSortFileSystemByStartSector();
void filesystemSortFileSystemByID();

#endif // FILESYSTEM_H
