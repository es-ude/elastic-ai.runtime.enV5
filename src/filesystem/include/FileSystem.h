#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <FlashTypedefs.h>
#include <SpiTypedefs.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <hardware/spi.h>

#include "EnV5HwConfiguration.h"

extern uint8_t numberOfEntries;
extern uint8_t sectorFree[64];
extern uint8_t currentID;

//char name[64];
typedef struct fileSystemEntry {
    uint8_t id;
    uint8_t startSector;
    uint32_t size;
    uint8_t isConfig;
    uint8_t numberOfSectors;
}fileSystemEntry;
extern fileSystemEntry fileSystem[];
extern size_t fileSystemLength;

void inc();

void initFileSystem(flashConfiguration_t flashConfig);
void addNewFileSystemEntry(char name[], uint8_t sector,  uint32_t size, uint8_t isConfig);
void deleteFileSystemEntry(uint8_t index);
void updateFileSystemEntry(uint8_t index, uint8_t id, char name[], uint32_t address,  size_t size, bool isConfig);
void printFileSystem();
void writeFileSystemToFlash(flashConfiguration_t* flashConfig);
uint8_t getNumberOfFreeSectors();
fileSystemEntry *readByID(uint8_t id);
fileSystemEntry *readByIndex(uint8_t index);
bool checkIfFileFits(size_t size);

void moveFileToSector(flashConfiguration_t* flashConfig, uint8_t currentSector, uint8_t newSector);


//TEMPORARY
bool checkIfFileSystemExists(flashConfiguration_t* flashConfig);
void checkNumberOfEntries(flashConfiguration_t* flashConfig);

void printFSS();
void writeFileToFlash();
#endif //FILESYSTEM_H
