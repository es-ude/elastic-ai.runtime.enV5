#define SOURCE_FILE "FILESYSTEM"

#include "FileSystem.h"
#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "FileSystemInternal.h"
#include "Flash.h"
#include "math.h"
#include "middleware.h"

#include <stdlib.h>
#include <string.h>

static size_t entrySize = sizeof(fileSystemEntry_t);

// region PUBLIC HEADER FUNCTIONS

void filesystemInit(flashConfiguration_t *flashConfig,
                    filesystemConfiguration_t *filesystemConfig) {
    filesystemConfig->filesystemStartSector = 1019;
    filesystemConfig->filesystemEndSector = 1023;
    filesystemConfig->flash = flashConfig;

    if (checkIfFileSystemExists(filesystemConfig)) {
        checkNumberOfEntries(filesystemConfig);

        // initialize array to all 1 (free)
        for (int i = 0; i < sizeof(filesystemConfig->sectorFree); i++) {
            filesystemConfig->sectorFree[i] = 1;
        }

        // set sector of filesystem to 0 (used)
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector - 1] = 0;

        // for each entry, set all used sectors to 0
        for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
            fileSystemEntry_t entry = filesystemConfig->fileSystem[i];
            for (int j = 0; j < entry.entry.numberOfSectors; j++) {
                filesystemConfig->sectorFree[entry.entry.startSector + j] = 0;
            }
        }
    } else {
        filesystemConfig->nextFileSystemSector = filesystemConfig->filesystemStartSector;
        filesystemConfig->numberOfEntries = 0;
        filesystemConfig->fileID = 100;

        // all sectors are free except for the sector containing the file system
        for (int i = 0; i < sizeof(filesystemConfig->sectorFree); i++) {
            filesystemConfig->sectorFree[i] = 1;
        }
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;
    }
}

int32_t filesystemFindFittingStartSector(const filesystemConfiguration_t *filesystemConfig,
                                         uint32_t numberOfRequiredBytes) {
    int32_t newFileSector;
    uint8_t counter = 0;
    uint32_t numberOfRequiredSectors =
        (uint32_t)ceilf((float)numberOfRequiredBytes / (float)FLASH_BYTES_PER_SECTOR);

    for (size_t i = 0; i < filesystemConfig->filesystemStartSector; i++) {
        if (filesystemConfig->sectorFree[i] == 1 && counter == 0) {
            newFileSector = i;
            ++counter;
        } else if (filesystemConfig->sectorFree[i] == 1 && counter < numberOfRequiredSectors) {
            ++counter;
        } else if (counter == numberOfRequiredSectors) {
            return newFileSector;
        } else {
            counter = 0;
        }
    }
    return -1;
}

void filesystemAddNewFileSystemEntry(filesystemConfiguration_t *filesystemConfig,
                                     uint32_t startSector, uint32_t size, uint8_t isConfig) {
    uint8_t id = filesystemConfig->fileID;
    filesystemConfig->fileID++;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.id = id;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.startSector = startSector;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size = size;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.isConfig = isConfig;

    // Set number of used sectors for this file
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors =
        (size_t)ceilf(
            (float)filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size /
            FLASH_BYTES_PER_SECTOR);

    // set all used sectors to 0 (used)
    for (int i = 0;
         i < filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors;
         i++) {
        filesystemConfig->sectorFree[startSector + i] = 0;
    }

    // update number of entries
    ++filesystemConfig->numberOfEntries;
    writeFileSystemToFlash(filesystemConfig);
}

void filesystemMoveFileToSector(filesystemConfiguration_t *filesystemConfig, uint8_t ID,
                                uint8_t newSector) {
    fileSystemEntry_t *entry = filesystemGetEntryByID(filesystemConfig, ID);

    if (entry == NULL) {
        return;
    }
    uint8_t currentSector = entry->entry.startSector;
    int index = getIndexBySector(filesystemConfig, currentSector);
    if (index < 0) {
        return;
    }
    fileSystemEntry_t fileSystemEntry = filesystemConfig->fileSystem[index];

    uint8_t requiredSectors = fileSystemEntry.entry.numberOfSectors;
    PRINT_DEBUG("Number of required sectors: %d\n", requiredSectors);

    // Check what to do, if sector isn't empty
    for (int i = 0; i < requiredSectors; i++) {
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
}

bool filesystemEraseFileByID(filesystemConfiguration_t *filesystemConfig, uint8_t id) {
    fileSystemEntry_t *entry = filesystemGetEntryByID(filesystemConfig, id);
    if (entry == NULL) {
        PRINT_DEBUG("No entry with this ID. Aborting...\n");
        return false;
    }

    uint32_t startAddress = entry->entry.startSector * FLASH_BYTES_PER_SECTOR;

    for (int i = 0; i < entry->entry.numberOfSectors; i++) {
        startAddress += i * FLASH_BYTES_PER_SECTOR;
        flashEraseSector(filesystemConfig->flash, startAddress);
        filesystemConfig->sectorFree[entry->entry.startSector + i] = 1;
    }
    deleteFileSystemEntry(filesystemConfig,
                          getIndexBySector(filesystemConfig, entry->entry.startSector));
    writeFileSystemToFlash(filesystemConfig);
    return true;
}

uint8_t filesystemGetNumberOfFreeSectors(const filesystemConfiguration_t *filesystemConfig) {
    uint8_t count = 0;
    for (int i = 0; i < sizeof(filesystemConfig->sectorFree); i++) {
        if (filesystemConfig->sectorFree[i] == 1) {
            count++;
        }
    }
    return count;
}

fileSystemEntry_t *filesystemGetEntryByID(filesystemConfiguration_t *filesystemConfig, uint8_t id) {
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        if (filesystemConfig->fileSystem[i].entry.id == id) {
            return &filesystemConfig->fileSystem[i];
        }
    }
    return NULL;
}

fileSystemEntry_t *filesystemGetEntryByIndex(filesystemConfiguration_t *filesystemConfig,
                                             uint8_t index) {
    for (int i = 0; i <= filesystemConfig->numberOfEntries; i++) {
        PRINT_DEBUG("%d, %d\n", i, index);
        if (i == index) {
            return &filesystemConfig->fileSystem[i];
        }
    }
    PRINT_DEBUG("No entry found at this index.");
    return NULL;
}

fileSystemEntry_t *filesystemGetEntryBySector(filesystemConfiguration_t *filesystemConfig,
                                              uint8_t sector) {
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

// endregion PUBLIC HEADER FUNCTIONS

// region INTERNAL HEADER FUNCTIONS

void inc(filesystemConfiguration_t *filesystemConfig) {
    if (filesystemConfig->nextFileSystemSector == filesystemConfig->filesystemEndSector) {
        filesystemConfig->nextFileSystemSector = filesystemConfig->filesystemStartSector;
    } else {
        filesystemConfig->nextFileSystemSector = filesystemConfig->nextFileSystemSector + 1;
    }
}

void updateEntrySector(filesystemConfiguration_t *filesystemConfig, uint8_t oldSector,
                       uint8_t newSector) {
    fileSystemEntry_t *fse = filesystemGetEntryBySector(filesystemConfig, oldSector);
    fse->entry.startSector = newSector;
}

void reconstructEntry(filesystemConfiguration_t *filesystemConfig, uint8_t id, uint8_t sector,
                      uint32_t size, uint8_t isConfig, uint8_t numberOfSectors) {
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.id = id;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.startSector = sector;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size = size;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.isConfig = isConfig;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors =
        numberOfSectors;
    ++filesystemConfig->numberOfEntries;
}

int getIndexBySector(const filesystemConfiguration_t *filesystemConfig, uint8_t sector) {
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        if (filesystemConfig->fileSystem[i].entry.startSector == sector) {
            return i;
        }
    }
    PRINT_DEBUG("No entry found with this address...\n");
    return -1;
}

void writeFileToSector(filesystemConfiguration_t *filesystemConfig, uint8_t currentSector,
                       uint8_t newSector) {
    fileSystemEntry_t fse = *filesystemGetEntryBySector(filesystemConfig, currentSector);

    uint32_t currentAddress = (currentSector) * (filesystemConfig->flash->flashBytesPerSector);
    uint32_t newAddress = (newSector) * (filesystemConfig->flash->flashBytesPerSector);

    // clear new sectors before writing
    for (int i = 0; i < fse.entry.numberOfSectors; i++) {
        flashEraseSector(filesystemConfig->flash, newAddress + (i * FLASH_BYTES_PER_SECTOR));
    }

    // Write file to new sectors
    size_t numberOfPages =
        (size_t)ceilf((float)fse.entry.size) / filesystemConfig->flash->flashBytesPerPage;

    size_t page = 0;
    uint8_t data[filesystemConfig->flash->flashBytesPerPage];
    data_t pageBuffer = {.data = data, .length = filesystemConfig->flash->flashBytesPerPage};

    while (page < numberOfPages) {
        flashReadData(filesystemConfig->flash,
                      currentAddress + (page * filesystemConfig->flash->flashBytesPerPage),
                      &pageBuffer);

        flashWritePage(filesystemConfig->flash,
                       newAddress + (page * filesystemConfig->flash->flashBytesPerPage),
                       pageBuffer.data, filesystemConfig->flash->flashBytesPerPage);
        page++;
    }

    // clear old sectors
    for (int i = 0; i < fse.entry.numberOfSectors; i++) {
        flashEraseSector(filesystemConfig->flash, currentAddress + (i * FLASH_BYTES_PER_SECTOR));
    }
}

void fileSystemEntryToByteArray(const fileSystemEntry_t *data, uint8_t *byteArray, uint8_t index) {
    size_t offset = index * entrySize;

    // shift to write 8 bits after each other to the array
    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        byteArray[offset] = (uint8_t)((data->entry.size >> (i * 8)) & 0xFF);
        ++offset;
    }

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset] = (uint8_t)(data->entry.id & 0xFF);
        ++offset;
    }

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset] = (uint8_t)(data->entry.startSector & 0xFF);
        ++offset;
    }

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset] = (uint8_t)((data->entry.isConfig) & 0xFF);
        ++offset;
    }

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset] = (uint8_t)((data->entry.numberOfSectors >> (i * 8)) & 0xFF);
        ++offset;
    }
}

bool checkIfFileSystemExists(filesystemConfiguration_t *filesystemConfig) {

    uint8_t data[entrySize];
    data_t entryBuffer = {.data = data, .length = entrySize};

    for (int i = filesystemConfig->filesystemStartSector;
         i <= filesystemConfig->filesystemEndSector; i++) {
        uint32_t sectorStartAddress = i * FLASH_BYTES_PER_SECTOR;
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

    uint32_t fileSystemStartAddress = sector * FLASH_BYTES_PER_SECTOR;
    uint8_t currentNumberOfEntries = 0;

    uint8_t data[entrySize];
    data_t entryBuffer = {.data = data, .length = entrySize};

    while (true) {
        flashReadData(filesystemConfig->flash,
                      fileSystemStartAddress + (currentNumberOfEntries * entrySize), &entryBuffer);
        fileSystemEntry_t *currentEntry = (fileSystemEntry_t *)entryBuffer.data;

        if (currentEntry->entry.id == 255) {
            break;
        }

        reconstructEntry(filesystemConfig, currentEntry->entry.id, currentEntry->entry.startSector,
                         currentEntry->entry.size, currentEntry->entry.isConfig,
                         currentEntry->entry.numberOfSectors);
        filesystemConfig->fileID = currentEntry->entry.id + 1;
        currentNumberOfEntries++;
    }
    PRINT_DEBUG("Number of entries: %d\n", currentNumberOfEntries);
    PRINT_DEBUG("\n");
    filesystemConfig->numberOfEntries = currentNumberOfEntries;
}

void deleteFileSystemEntry(filesystemConfiguration_t *filesystemConfig, uint8_t index) {
    for (int i = index + 1; i < filesystemConfig->numberOfEntries; i++) {
        filesystemConfig->fileSystem[i - 1] = filesystemConfig->fileSystem[i];
    }
    memset(&filesystemConfig->fileSystem[filesystemConfig->numberOfEntries - 1], 0xFF,
           sizeof(fileSystemEntry_t));
    --filesystemConfig->numberOfEntries;
}

void writeFileSystemToFlash(filesystemConfiguration_t *filesystemConfig) {
    // Clear old data from pages to be used by new file system
    PRINT_DEBUG("Writing new File System to Sector %i\n", filesystemConfig->nextFileSystemSector);

    // Erase new sector before writing
    uint32_t newAddress = filesystemConfig->nextFileSystemSector * FLASH_BYTES_PER_SECTOR;
    flashEraseSector(filesystemConfig->flash, newAddress);

    // Convert fileEntry Structs to 8-bit array to use flashWritePage()
    uint8_t dataByteArray[filesystemConfig->numberOfEntries * entrySize];
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        fileSystemEntry_t *ptr = &filesystemConfig->fileSystem[i];
        // printFileSystemEntry(fileSystem[i]);
        fileSystemEntryToByteArray(ptr, dataByteArray, i);
    }

    // Write Byte Array to Flash
    flashWritePage(filesystemConfig->flash, newAddress, dataByteArray,
                   filesystemConfig->numberOfEntries * entrySize);

    // update Free Sector Array and erase old sector
    if (filesystemConfig->nextFileSystemSector == filesystemConfig->filesystemStartSector) {
        filesystemConfig->sectorFree[filesystemConfig->filesystemEndSector] = 1;
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;

        uint32_t address =
            filesystemConfig->filesystemEndSector * filesystemConfig->flash->flashBytesPerSector;
        flashEraseSector(filesystemConfig->flash, address);
    } else {
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector - 1] = 1;
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;

        uint32_t currentFileSystemSector = filesystemConfig->nextFileSystemSector - 1;
        uint32_t address = currentFileSystemSector * filesystemConfig->flash->flashBytesPerSector;
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
    } else if (entryA->entry.startSector > entryB->entry.startSector) {
        return 1; // b kommt vor a
    } else {
        return 0; // a und b sind gleich
    }
}

int compareIDs(const void *a, const void *b) {
    fileSystemEntry_t *entryA = (fileSystemEntry_t *)a;
    fileSystemEntry_t *entryB = (fileSystemEntry_t *)b;

    if (entryA->entry.id < entryB->entry.id) {
        return -1; // a kommt vor b
    } else if (entryA->entry.id > entryB->entry.id) {
        return 1; // b kommt vor a
    } else {
        return 0; // a und b sind gleich
    }
}

// endregion INTERNAL HEADER FUNCTIONS
