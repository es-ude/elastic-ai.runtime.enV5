#define SOURCE_FILE "FILESYSTEM"

#include "FileSystem.h"
#include "Common.h"
#include "FileSystemInternal.h"
#include "Flash.h"
#include "math.h"

#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_SECTORS_FOR_FILESYSTEM 5
#define START_FILE_ID 100
#define ENTRY_SIZE sizeof(fileSystemEntry_t)

// region PUBLIC HEADER FUNCTIONS

void filesystemInit(flashConfiguration_t *flashConfig,
                    filesystemConfiguration_t *filesystemConfig) {
    filesystemConfig->flash = flashConfig;

    filesystemConfig->filesystemEndSector = filesystemConfig->flash->numberOfSectors - 1;
    filesystemConfig->filesystemStartSector =
        filesystemConfig->filesystemEndSector - (NUMBER_OF_SECTORS_FOR_FILESYSTEM - 1);
    filesystemConfig->numberOfFreeSectors = filesystemConfig->filesystemStartSector;

    if (checkIfFileSystemExists(filesystemConfig)) {
        checkNumberOfEntries(filesystemConfig);
        existingFileSystemInit(filesystemConfig);
    }

    else {
        noExistingFileSystemInit(filesystemConfig);
    }
}

int32_t filesystemFindFittingStartSector(const filesystemConfiguration_t *filesystemConfig,
                                         uint32_t numberOfRequiredBytes) {
    int32_t newFileSector = FILESYSTEM_ERROR;
    uint32_t counter = 0;
    uint32_t numberOfRequiredSectors = (uint32_t)ceilf(
        (float)numberOfRequiredBytes / (float)flashGetBytesPerSector(filesystemConfig->flash));
    for (uint32_t i = 0; i < filesystemConfig->filesystemStartSector; i++) {
        if (filesystemConfig->sectorFree[i] == 1 && counter == 0) {
            newFileSector = (uint32_t)i;
            ++counter;
        } else if (filesystemConfig->sectorFree[i] == 1 && counter < numberOfRequiredSectors) {
            ++counter;
        } else if (counter == numberOfRequiredSectors) {
            return newFileSector;
        } else {
            counter = 0;
        }
    }
    return FILESYSTEM_ERROR;
}

void filesystemAddNewFileSystemEntry(filesystemConfiguration_t *filesystemConfig,
                                     uint32_t startSector, uint32_t size, isConfig_t isConfig) {
    size_t numberOfEntries = filesystemConfig->numberOfEntries;
    uint16_t id = filesystemConfig->fileID;
    filesystemConfig->fileID++;

    entrySetID(filesystemConfig, numberOfEntries, id);
    entrySetStartSector(filesystemConfig, numberOfEntries, startSector);
    entrySetSize(filesystemConfig, numberOfEntries, size);
    entrySetIsConfig(filesystemConfig, numberOfEntries, isConfig);

    uint32_t numberOfUsedSectors = (size_t)ceilf(
        (float)filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size /
        (float)flashGetBytesPerSector(filesystemConfig->flash));

    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors =
        numberOfUsedSectors;

    // set all used sectors to 0 (used)
    if (isConfig == BLOCKED_FOR_FPGA) {
        for (int i = 0;
             i <
             filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors;
             i++) {
            filesystemConfig->sectorFree[startSector + i] = BLOCKED_FOR_FPGA;
        }
    }

    else {
        for (int i = 0;
             i <
             filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors;
             i++) {
            filesystemConfig->sectorFree[startSector + i] = 0;
        }
    }

    // update number of entries
    ++filesystemConfig->numberOfEntries;

    filesystemConfig->numberOfFreeSectors =
        filesystemConfig->numberOfFreeSectors - numberOfUsedSectors;
    writeFileSystemToFlash(filesystemConfig);
}

bool filesystemMoveFileToSector(filesystemConfiguration_t *filesystemConfig, uint16_t ID,
                                uint16_t newSector) {

    if (newSector >= filesystemConfig->filesystemStartSector) {
        return false;
    }

    fileSystemEntry_t *entry = filesystemGetEntryByID(filesystemConfig, ID);

    if (entry == NULL) {
        PRINT_DEBUG("No entry found with matching ID. Aborting...");
        return false;
    }
    uint16_t currentSector = entry->entry.startSector;
    int index = getIndexBySector(filesystemConfig, currentSector);
    if (index < 0) {
        return false;
    }
    fileSystemEntry_t fileSystemEntry = filesystemConfig->fileSystem[index];

    uint16_t requiredSectors = fileSystemEntry.entry.numberOfSectors;
    PRINT_DEBUG("Number of required sectors: %d\n", requiredSectors);

    // Check what to do, if sector isn't empty
    for (int i = 0; i < requiredSectors; i++) {
        if (filesystemConfig->sectorFree[newSector + i] == BLOCKED_FOR_FPGA) {
            PRINT_DEBUG("ABORTING... SECTOR IS PROTECTED FOR FPGA USAGE ONLY!");
            return false;
        }

        if (filesystemConfig->sectorFree[newSector + i] == 0) {
            // check if entry with this startsector exists and remove it (as it is overwritten)
            index = getIndexBySector(filesystemConfig, newSector);
            if (index != -1) {
                deleteFileSystemEntry(filesystemConfig, index);
            }
        }
    }

    PRINT_DEBUG("Writing File with ID %d to sector %d... \n", ID, newSector);
    writeFileToSector(filesystemConfig, currentSector, newSector);
    updateEntrySector(filesystemConfig, currentSector, newSector);
    for (int i = 0; i < fileSystemEntry.entry.numberOfSectors; i++) {
        filesystemConfig->sectorFree[currentSector + i] = 1;
        filesystemConfig->sectorFree[newSector + i] = 0;
    }
    writeFileSystemToFlash(filesystemConfig);

    return true;
}

bool filesystemEraseFileByID(filesystemConfiguration_t *filesystemConfig, uint16_t id) {
    fileSystemEntry_t *entry = filesystemGetEntryByID(filesystemConfig, id);
    if (entry == NULL) {
        PRINT_DEBUG("No entry with this ID. Aborting...\n");
        return false;
    }
    if (entry->entry.isConfig == BLOCKED_FOR_FPGA) {
        PRINT_DEBUG("Sector is blocked for FPGA and can't be erased using this function!");
        return false;
    }

    uint32_t startAddress =
        entry->entry.startSector * flashGetBytesPerSector(filesystemConfig->flash);

    for (int i = 0; i < entry->entry.numberOfSectors; i++) {
        startAddress += i * flashGetBytesPerSector(filesystemConfig->flash);
        flashEraseSector(filesystemConfig->flash, startAddress);
        filesystemConfig->sectorFree[entry->entry.startSector + i] = 1;
        ++filesystemConfig->numberOfFreeSectors;
    }
    deleteFileSystemEntry(filesystemConfig,
                          getIndexBySector(filesystemConfig, entry->entry.startSector));
    writeFileSystemToFlash(filesystemConfig);
    return true;
}

uint32_t filesystemGetNumberOfFreeSectors(const filesystemConfiguration_t *filesystemConfig) {
    uint32_t count = 0;
    for (int i = 0; i < sizeof(filesystemConfig->sectorFree); i++) {
        if (filesystemConfig->sectorFree[i] == 1) {
            count++;
        }
    }
    return count;
}

fileSystemEntry_t *filesystemGetEntryByID(filesystemConfiguration_t *filesystemConfig,
                                          uint16_t id) {
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        if (filesystemConfig->fileSystem[i].entry.id == id) {
            return &filesystemConfig->fileSystem[i];
        }
    }
    return NULL;
}

fileSystemEntry_t *filesystemGetEntryByIndex(filesystemConfiguration_t *filesystemConfig,
                                             uint8_t index) {
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        PRINT_DEBUG("%d, %d\n", i, index);
        if (i == index) {
            return &filesystemConfig->fileSystem[i];
        }
    }
    PRINT_DEBUG("No entry found at this index.");
    return NULL;
}

fileSystemEntry_t *filesystemGetEntryBySector(filesystemConfiguration_t *filesystemConfig,
                                              uint16_t sector) {
    for (int i = 0; i <= filesystemConfig->numberOfEntries; i++) {
        if (filesystemConfig->fileSystem[i].entry.startSector == sector) {
            return &filesystemConfig->fileSystem[i];
        }
    }
    PRINT_DEBUG("No entry found with this address...\n");
    return NULL;
}

void filesystemSortFileSystemByStartSector(filesystemConfiguration_t *filesystemConfig) {
    qsort(filesystemConfig->fileSystem, filesystemConfig->numberOfEntries,
          sizeof(fileSystemEntry_t), compareStartSectors);
}

void filesystemSortFileSystemByID(filesystemConfiguration_t *filesystemConfig) {
    qsort(filesystemConfig->fileSystem, filesystemConfig->numberOfEntries,
          sizeof(fileSystemEntry_t), compareIDs);
}

bool filesystemBlockBytesForFPGA(filesystemConfiguration_t *filesystemConfig, uint16_t startSector,
                                 uint32_t numberOfBytes) {
    uint16_t numberOfRequiredSectors =
        ceil((double)numberOfBytes / flashGetBytesPerSector(filesystemConfig->flash));

    // check if required sectors already contain data. If so, abort and return false.
    for (uint16_t i = startSector; i < startSector + numberOfRequiredSectors; i++) {
        fileSystemEntry_t *entry = filesystemGetEntryBySector(filesystemConfig, i);

        if (entry == NULL || entry->entry.size == 0) {
            for (uint16_t i = startSector; i < startSector + numberOfRequiredSectors; i++) {
                filesystemConfig->sectorFree[i] = BLOCKED_FOR_FPGA;
            }
            filesystemAddNewFileSystemEntry(filesystemConfig, startSector, numberOfBytes, 2);
            ++filesystemConfig->numberOfBlockedSectors;
            return true;
        } else {
            PRINT_DEBUG("ID: %i", entry->entry.id);
            PRINT_DEBUG("Size: %i", entry->entry.size);
            PRINT_DEBUG("isConfig: %i", entry->entry.isConfig);
            PRINT_DEBUG("Start Sector: %i", entry->entry.startSector);
            PRINT_DEBUG("Number of Sectors: %i", entry->entry.numberOfSectors);
            PRINT_DEBUG("Sector already contains data. Aborting...");
            return false;
        }
    }
}

void filesystemFreeBlockedFPGASectors(filesystemConfiguration_t *filesystemConfig) {
    PRINT_DEBUG("%i", filesystemConfig->numberOfEntries);
    for (int i = filesystemConfig->numberOfEntries; i >= 0; i--) {
        fileSystemEntry_t *entry = &filesystemConfig->fileSystem[i];
        if (entry->entry.isConfig == BLOCKED_FOR_FPGA) {
            uint32_t startAddress =
                entry->entry.startSector * flashGetBytesPerSector(filesystemConfig->flash);
            for (int j = 0; j < entry->entry.numberOfSectors; j++) {
                flashEraseSector(filesystemConfig->flash,
                                 startAddress +
                                     j * flashGetBytesPerSector(filesystemConfig->flash));
                filesystemConfig->sectorFree[entry->entry.startSector + j] = 1;
                --filesystemConfig->numberOfBlockedSectors;
            }
            deleteFileSystemEntry(filesystemConfig, i);
        }
    }
    writeFileSystemToFlash(filesystemConfig);
}

void filesystemEraseAllEntries(filesystemConfiguration_t *filesystemConfig) {
    int i = 0;
    while (filesystemConfig->numberOfEntries > 0) {
        deleteFileSystemEntry(filesystemConfig, i);
        i++;
    }

    if (filesystemConfig->nextFileSystemSector == filesystemConfig->filesystemStartSector) {
        uint32_t address =
            filesystemConfig->filesystemEndSector * flashGetBytesPerSector(filesystemConfig->flash);
        flashEraseSector(filesystemConfig->flash, address);
    } else {
        uint32_t address = (filesystemConfig->nextFileSystemSector - 1) *
                           flashGetBytesPerSector(filesystemConfig->flash);
        flashEraseSector(filesystemConfig->flash, address);
    }
    filesystemConfig->numberOfFreeSectors = 1019;
    filesystemConfig->fileID = 100;
}

// endregion PUBLIC HEADER FUNCTIONS

// region INTERNAL HEADER FUNCTIONS

static void initSectorFree(filesystemConfiguration_t *filesystemConfig) {
    for (int i = 0; i < sizeof(filesystemConfig->sectorFree); i++) {
        filesystemConfig->sectorFree[i] = 1;
    }
}

static void sectorFreeSetSectorsUsedByEntries(filesystemConfiguration_t *filesystemConfig) {
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        fileSystemEntry_t entry = filesystemConfig->fileSystem[i];
        for (int j = 0; j < entry.entry.numberOfSectors; j++) {
            if (entry.entry.isConfig == BLOCKED_FOR_FPGA) {
                filesystemConfig->sectorFree[entry.entry.startSector + j] = BLOCKED_FOR_FPGA;
            } else {
                filesystemConfig->sectorFree[entry.entry.startSector + j] = 0;
            }
        }
    }
}

static void existingFileSystemInit(filesystemConfiguration_t *filesystemConfig) {
    initSectorFree(filesystemConfig);
    filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector - 1] = 0;
    sectorFreeSetSectorsUsedByEntries(filesystemConfig);
}

static void noExistingFileSystemInit(filesystemConfiguration_t *filesystemConfig) {
    filesystemConfig->nextFileSystemSector = filesystemConfig->filesystemStartSector;
    filesystemConfig->numberOfEntries = 0;
    filesystemConfig->fileID = START_FILE_ID;

    initSectorFree(filesystemConfig);
    filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;
}

static void entrySetID(filesystemConfiguration_t *filesystemConfig, uint32_t index, uint32_t id) {
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.id = id;
}
static void entrySetStartSector(filesystemConfiguration_t *filesystemConfig, uint32_t index,
                                uint32_t startSector) {
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.startSector = startSector;
}

static void entrySetSize(filesystemConfiguration_t *filesystemConfig, uint32_t index, size_t size) {
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size = size;
}

static void entrySetIsConfig(filesystemConfiguration_t *filesystemConfig, uint32_t index,
                             isConfig_t isConfig) {
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.isConfig = isConfig;
}

void inc(filesystemConfiguration_t *filesystemConfig) {
    if (filesystemConfig->nextFileSystemSector == filesystemConfig->filesystemEndSector) {
        filesystemConfig->nextFileSystemSector = filesystemConfig->filesystemStartSector;
    } else {
        filesystemConfig->nextFileSystemSector = filesystemConfig->nextFileSystemSector + 1;
    }
}

void updateEntrySector(filesystemConfiguration_t *filesystemConfig, uint16_t oldSector,
                       uint16_t newSector) {
    fileSystemEntry_t *fse = filesystemGetEntryBySector(filesystemConfig, oldSector);
    fse->entry.startSector = newSector;
}

void reconstructEntry(filesystemConfiguration_t *filesystemConfig, uint16_t id, uint16_t sector,
                      uint32_t size, isConfig_t isConfig, uint16_t numberOfSectors) {
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.id = id;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.startSector = sector;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size = size;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.isConfig = isConfig;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors =
        numberOfSectors;
    ++filesystemConfig->numberOfEntries;
}

int getIndexBySector(const filesystemConfiguration_t *filesystemConfig, uint16_t sector) {
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        if (filesystemConfig->fileSystem[i].entry.startSector == sector) {
            return i;
        }
    }
    PRINT_DEBUG("No entry found with this address...\n");
    return -1;
}

void writeFileToSector(filesystemConfiguration_t *filesystemConfig, uint16_t currentSector,
                       uint16_t newSector) {
    fileSystemEntry_t fse = *filesystemGetEntryBySector(filesystemConfig, currentSector);

    uint32_t currentAddress = (currentSector) * (filesystemConfig->flash->bytesPerSector);
    uint32_t newAddress = (newSector) * (filesystemConfig->flash->bytesPerSector);

    // clear new sectors before writing
    for (int i = 0; i < fse.entry.numberOfSectors; i++) {
        flashEraseSector(filesystemConfig->flash,
                         newAddress + (i * flashGetBytesPerSector(filesystemConfig->flash)));
    }

    // Write file to new sectors
    size_t numberOfPages =
        (size_t)ceilf((float)fse.entry.size) / filesystemConfig->flash->bytesPerPage;

    size_t page = 0;
    uint8_t data[filesystemConfig->flash->bytesPerPage];
    data_t pageBuffer = {.data = data, .length = filesystemConfig->flash->bytesPerPage};

    while (page < numberOfPages) {
        flashReadData(filesystemConfig->flash,
                      currentAddress + (page * filesystemConfig->flash->bytesPerPage), &pageBuffer);

        flashWritePage(filesystemConfig->flash,
                       newAddress + (page * filesystemConfig->flash->bytesPerPage), pageBuffer.data,
                       filesystemConfig->flash->bytesPerPage);
        page++;
    }

    // clear old sectors
    for (int i = 0; i < fse.entry.numberOfSectors; i++) {
        flashEraseSector(filesystemConfig->flash,
                         currentAddress + (i * flashGetBytesPerSector(filesystemConfig->flash)));
    }
}

void fileSystemEntryToByteArray(const fileSystemEntry_t *data, uint8_t *byteArray, uint8_t index) {
    size_t offset = index * ENTRY_SIZE;

    // shift to write 8 bits after each other to the array
    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        byteArray[offset] = (uint8_t)(data->entry.size >> i * 8 & 0xFF);
        ++offset;
    }

    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        byteArray[offset] = (uint8_t)(data->entry.id >> i * 8 & 0xFF);
        ++offset;
    }

    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        byteArray[offset] = (uint8_t)(data->entry.startSector >> i * 8 & 0xFF);
        ++offset;
    }

    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        byteArray[offset] = (uint8_t)(data->entry.isConfig >> i * 8 & 0xFF);
        ++offset;
    }

    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        byteArray[offset] = (uint8_t)(data->entry.numberOfSectors >> i * 8 & 0xFF);
        ++offset;
    }
}

bool checkIfFileSystemExists(filesystemConfiguration_t *filesystemConfig) {

    uint8_t data[ENTRY_SIZE];
    data_t entryBuffer = {.data = data, .length = ENTRY_SIZE};

    for (int i = filesystemConfig->filesystemStartSector;
         i <= filesystemConfig->filesystemEndSector; i++) {
        uint32_t sectorStartAddress = i * flashGetBytesPerSector(filesystemConfig->flash);
        flashReadData(filesystemConfig->flash, sectorStartAddress, &entryBuffer);
        if (entryBuffer.data[0] != 255) {
            PRINT_DEBUG("\n");
            PRINT_DEBUG("FileSystem found in sector %d.\n", i);
            filesystemConfig->nextFileSystemSector = i;
            inc(filesystemConfig);
            return true;
        }
    }
    PRINT_DEBUG("\n");
    PRINT_DEBUG("No FileSystem found...\n");
    PRINT_DEBUG("\n");
    return false;
}

void checkNumberOfEntries(filesystemConfiguration_t *filesystemConfig) {
    uint32_t sector;
    if (filesystemConfig->nextFileSystemSector == filesystemConfig->filesystemStartSector) {
        sector = filesystemConfig->filesystemEndSector;
    } else {
        sector = filesystemConfig->nextFileSystemSector - 1;
    }

    uint32_t fileSystemStartAddress = sector * flashGetBytesPerSector(filesystemConfig->flash);
    uint8_t currentNumberOfEntries = 0;

    uint8_t data[ENTRY_SIZE];
    data_t entryBuffer = {.data = data, .length = ENTRY_SIZE};

    while (true) {
        flashReadData(filesystemConfig->flash,
                      fileSystemStartAddress + (currentNumberOfEntries * ENTRY_SIZE), &entryBuffer);
        fileSystemEntry_t *currentEntry = (fileSystemEntry_t *)entryBuffer.data;

        // 65535 = 0xFFFF = unwritten sector
        if (currentEntry->entry.id == 65535) {
            break;
        }

        reconstructEntry(filesystemConfig, currentEntry->entry.id, currentEntry->entry.startSector,
                         currentEntry->entry.size, currentEntry->entry.isConfig,
                         currentEntry->entry.numberOfSectors);
        filesystemConfig->fileID = currentEntry->entry.id + 1;
        currentNumberOfEntries++;
    }
    PRINT_DEBUG("Number of entries: %d\n", currentNumberOfEntries);
    filesystemConfig->numberOfEntries = currentNumberOfEntries;
}

void deleteFileSystemEntry(filesystemConfiguration_t *filesystemConfig, uint8_t index) {
    for (int i = index; i < filesystemConfig->numberOfEntries; i++) {
        filesystemConfig->fileSystem[i] = filesystemConfig->fileSystem[i + 1];
    }
    memset(&filesystemConfig->fileSystem[filesystemConfig->numberOfEntries - 1], 0xFF,
           sizeof(fileSystemEntry_t));
    --filesystemConfig->numberOfEntries;
    filesystemConfig->sectorFree[index] = 1;
}

void writeFileSystemToFlash(filesystemConfiguration_t *filesystemConfig) {
    // Clear old data from pages to be used by new file system
    PRINT_DEBUG("Writing new File System to Sector %lu\n", filesystemConfig->nextFileSystemSector);

    // Erase new sector before writing
    uint32_t newAddress =
        filesystemConfig->nextFileSystemSector * flashGetBytesPerSector(filesystemConfig->flash);
    flashEraseSector(filesystemConfig->flash, newAddress);

    // Convert fileEntry Structs to 8-bit array to use flashWritePage()
    uint8_t dataByteArray[filesystemConfig->numberOfEntries * ENTRY_SIZE];
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        fileSystemEntry_t *ptr = &filesystemConfig->fileSystem[i];
        // printFileSystemEntry(fileSystem[i]);
        fileSystemEntryToByteArray(ptr, dataByteArray, i);
    }

    // Write Byte Array to Flash
    flashWritePage(filesystemConfig->flash, newAddress, dataByteArray,
                   filesystemConfig->numberOfEntries * ENTRY_SIZE);

    // update Free Sector Array and erase old sector
    if (filesystemConfig->nextFileSystemSector == filesystemConfig->filesystemStartSector) {
        filesystemConfig->sectorFree[filesystemConfig->filesystemEndSector] = 1;
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;

        uint32_t address =
            filesystemConfig->filesystemEndSector * filesystemConfig->flash->bytesPerSector;
        flashEraseSector(filesystemConfig->flash, address);
    } else {
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector - 1] = 1;
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;

        uint32_t currentFileSystemSector = filesystemConfig->nextFileSystemSector - 1;
        uint32_t address = currentFileSystemSector * filesystemConfig->flash->bytesPerSector;
        flashEraseSector(filesystemConfig->flash, address);
    }
    inc(filesystemConfig);

    PRINT_DEBUG("DONE...\n");
}

int compareStartSectors(const void *a, const void *b) {
    fileSystemEntry_t *entryA = (fileSystemEntry_t *)a;
    fileSystemEntry_t *entryB = (fileSystemEntry_t *)b;

    if (entryA->entry.startSector < entryB->entry.startSector) {
        return -1; // a kommt vor b
    }
    if (entryA->entry.startSector > entryB->entry.startSector) {
        return 1; // b kommt vor a
    }

    return 0; // a und b sind gleich
}

int compareIDs(const void *a, const void *b) {
    fileSystemEntry_t *entryA = (fileSystemEntry_t *)a;
    fileSystemEntry_t *entryB = (fileSystemEntry_t *)b;

    if (entryA->entry.id < entryB->entry.id) {
        return -1; // a kommt vor b
    }
    if (entryA->entry.id > entryB->entry.id) {
        return 1; // b kommt vor a
    }
    return 0; // a und b sind gleich
}

// endregion INTERNAL HEADER FUNCTIONS
