#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <FlashTypedefs.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct fileSystemEntry {
    uint8_t id;
    uint8_t startSector;
    uint32_t size;
    uint8_t isConfig;
    uint8_t numberOfSectors;
} fileSystemEntry;

extern fileSystemEntry fileSystem[];
extern size_t fileSystemLength;
extern uint8_t numberOfEntries;
extern uint8_t sectorFree[10];
extern uint8_t currentID;

extern uint32_t nextFileSector;

void initFileSystem(flashConfiguration_t flashConfig);
bool findFittingStartSector(uint8_t numberOfRequiredSectors);
void addNewFileSystemEntry(flashConfiguration_t *flashConfig, uint32_t size, uint8_t isConfig);
void moveFileToSector(flashConfiguration_t *flashConfig, uint8_t ID, uint8_t newSector);
bool eraseFileByID(flashConfiguration_t *flashConfig, uint8_t id);
void printFileSystem();

uint8_t getNumberOfFreeSectors();
fileSystemEntry *getEntryByID(uint8_t id);
fileSystemEntry *getEntryByIndex(uint8_t index);
fileSystemEntry *getEntryBySector(uint8_t sector);
void sortFileSystemByStartSector();
void sortFileSystemByID();

#endif // FILESYSTEM_H
