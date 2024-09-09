#define SOURCE_FILE "FILESYSTEM"

#include "math.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "FileSystem.h"
#include "FileSystemInternal.h"
#include "Flash.h"
#include "middleware.h"

#include <stdlib.h>
#include <string.h>

/*static uint8_t numberOfEntries = 0;
static uint8_t currentID = 100;

static uint32_t nextFileSystemSector;
static fileSystemEntry_t fileSystem[60];
static uint8_t entrySize = 8;

static uint32_t nextFileSector = 0;
static uint8_t sectorFree[10];*/

void filesystemInit(flashConfiguration_t *flashConfig,
                    filesystemConfiguration_t *filesystemConfig) {
    if (checkIfFileSystemExists(flashConfig, filesystemConfig)) {
        checkNumberOfEntries(flashConfig, filesystemConfig);

        // initialize array to all 1
        for (int i = 0; i < sizeof(filesystemConfig->sectorFree); i++) {
            filesystemConfig->sectorFree[i] = 1;
        }

        // set sector of filesystem to 0
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector - 1] = 0;

        // for each entry, set all used sectors to 0
        for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
            fileSystemEntry_t entry = filesystemConfig->fileSystem[i];
            for (int j = 0; j < entry.entry.numberOfSectors; j++) {
                filesystemConfig->sectorFree[entry.entry.startSector + j] = 0;
            }
        }

        for (int i = 0; i < sizeof(filesystemConfig->sectorFree); i++) {
            if (filesystemConfig->sectorFree[i] == 1) {
                filesystemConfig->nextFileSector = i;
                break;
            }
        }

        return;
    }

    filesystemConfig->nextFileSystemSector = 6;
    // initially all sectors are free except for the sector containing the file system
    for (int i = 0; i < sizeof(filesystemConfig->sectorFree); i++) {
        filesystemConfig->sectorFree[i] = 1;
    }
    filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;
}

int32_t filesystemFindFittingStartSector(filesystemConfiguration_t *filesystemConfig,
                                         uint8_t numberOfRequiredBytes) {
    uint32_t tempNewFileSector = filesystemConfig->nextFileSector;
    uint8_t counter = 0;
    uint8_t numberOfRequiredSectors =
        (uint8_t)ceilf((float)numberOfRequiredBytes / (float)FLASH_BYTES_PER_SECTOR);

    for (size_t i = 0; i < 6; i++) {
        if (filesystemConfig->sectorFree[i] == 1 && counter == 0) {
            tempNewFileSector = i;
            ++counter;
        } else if (filesystemConfig->sectorFree[i] == 1 && counter < numberOfRequiredSectors) {
            ++counter;
        } else if (counter == numberOfRequiredSectors) {
            filesystemConfig->nextFileSector = tempNewFileSector;
            return filesystemConfig->nextFileSector;
        } else {
            counter = 0;
        }
    }
    return -1;
}

void filesystemAddNewFileSystemEntry(flashConfiguration_t *flashConfig,
                                     filesystemConfiguration_t *filesystemConfig, uint32_t size,
                                     uint8_t isConfig) {
    uint8_t sector = filesystemConfig->nextFileSector;
    uint8_t id = filesystemConfig->currentID;
    filesystemConfig->currentID++;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.id = id;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.startSector = sector;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size = size;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.isConfig = isConfig;

    // Set number of used sectors for this file
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors =
        (size_t)ceilf(
            (float)filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size /
            FLASH_BYTES_PER_SECTOR);

    // set all used sectors to used
    for (int i = 0;
         i < filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors;
         i++) {
        filesystemConfig->sectorFree[sector + i] = 0;
    }

    // update number of entries
    ++filesystemConfig->numberOfEntries;
    writeFileSystemToFlash(flashConfig, filesystemConfig);
}

void filesystemMoveFileToSector(flashConfiguration_t *flashConfig,
                                filesystemConfiguration_t *filesystemConfig, uint8_t ID,
                                uint8_t newSector) {
    fileSystemEntry_t *entry = filesystemGetEntryByID(filesystemConfig, ID);
    uint8_t currentSector = entry->entry.startSector;
    int index = getIndexBySector(filesystemConfig, currentSector);
    if (index == -1) {
        return;
    }
    fileSystemEntry_t fileSystemEntry = filesystemConfig->fileSystem[index];

    uint8_t requiredSectors = fileSystemEntry.entry.numberOfSectors;
    char input;
    PRINT_DEBUG("Number of required sectors: %d\n", requiredSectors);

    // Check what to do, if sector isn't empty
    for (int i = 0; i < requiredSectors; i++) {
        if (filesystemConfig->sectorFree[newSector + i] == 0) {
            PRINT_DEBUG("Sector %d already contains data. Do you want to overwrite? y/n\n",
                        newSector + i);
            scanf("%c", &input);

            if (input == 'n') {
                PRINT_DEBUG("Aborting...\n");
                return;
            }

            // check if entry with this startsector exists and remove it (as it is overwritten)
            index = getIndexBySector(filesystemConfig, newSector);
            if (index != -1) {
                deleteFileSystemEntry(flashConfig, filesystemConfig, index);
            }
        }
    }

    PRINT_DEBUG("Writing File with ID %d to sector %d... \n", ID, newSector);
    writeFileToSector(flashConfig, filesystemConfig, currentSector, newSector);
    updateEntrySector(filesystemConfig, currentSector, newSector);
    for (int i = 0; i < fileSystemEntry.entry.numberOfSectors; i++) {
        filesystemConfig->sectorFree[currentSector + i] = 1;
        filesystemConfig->sectorFree[newSector + i] = 0;
    }
    writeFileSystemToFlash(flashConfig, filesystemConfig);
}

bool filesystemEraseFileByID(flashConfiguration_t *flashConfig,
                             filesystemConfiguration_t *filesystemConfig, uint8_t id) {
    fileSystemEntry_t *entry = filesystemGetEntryByID(filesystemConfig, id);
    if (entry == NULL) {
        PRINT_DEBUG("No entry with this ID. Aborting...\n");
        return false;
    }

    for (int i = 0; i < entry->entry.numberOfSectors; i++) {
        uint32_t address = entry->entry.startSector + i * FLASH_BYTES_PER_SECTOR;
        flashEraseSector(flashConfig, address);
        filesystemConfig->sectorFree[entry->entry.startSector + i] = 1;
    }
    deleteFileSystemEntry(flashConfig, filesystemConfig,
                          getIndexBySector(filesystemConfig, entry->entry.startSector));
    writeFileSystemToFlash(flashConfig, filesystemConfig);
    return true;
}

void filesystemPrintFileSystem(filesystemConfiguration_t *filesystemConfig) {
    PRINT_DEBUG("\n");
    PRINT_DEBUG("---File System---\n");
    PRINT_DEBUG("Number of entries: %i\n", filesystemConfig->numberOfEntries);
    PRINT_DEBUG("\n");
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        printFileSystemEntry(filesystemConfig->fileSystem[i]);
    }
    PRINT_DEBUG("\n");
}

uint8_t filesystemGetNumberOfFreeSectors(filesystemConfiguration_t *filesystemConfig) {
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

// ----------------------------------------------------------------------------------------------------

// Internal Functions

void inc(filesystemConfiguration_t *filesystemConfig) {
    if (filesystemConfig->nextFileSystemSector == 9) {
        filesystemConfig->nextFileSystemSector = 6;
    }

    else {
        filesystemConfig->nextFileSystemSector = filesystemConfig->nextFileSystemSector + 1;
    }
}

void updateEntrySector(filesystemConfiguration_t *filesystemConfig, uint8_t oldSector,
                       uint8_t newSector) {
    fileSystemEntry_t *fse = filesystemGetEntryBySector(filesystemConfig, oldSector);
    fse->entry.startSector = newSector;
}

void reconstructEntry(filesystemConfiguration_t *filesystemConfig, uint8_t id, char name[],
                      uint8_t sector, uint32_t size, uint8_t isConfig, uint8_t numberOfSectors) {
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.id = id;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.startSector = sector;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.size = size;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.isConfig = isConfig;
    filesystemConfig->fileSystem[filesystemConfig->numberOfEntries].entry.numberOfSectors =
        numberOfSectors;
    ++filesystemConfig->numberOfEntries;
}

/*void byteArrayToFileSystemEntry(uint8_t *array[entrySize]) {
    uint8_t id = *array[0];
    uint64_t name = (uint64_t)*array[1];
    uint8_t startSector = *array[9];
    uint32_t size = (uint32_t)*array[10];
    uint8_t isConfig = *array[14];
    uint8_t numberOfSectors = *array[15];
}*/

int getIndexBySector(filesystemConfiguration_t *filesystemConfig, uint8_t sector) {
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        if (filesystemConfig->fileSystem[i].entry.startSector == sector) {
            return i;
        }
    }
    PRINT_DEBUG("No entry found with this address...\n");
    return -1;
}

void writeFileToSector(flashConfiguration_t *flashConfig,
                       filesystemConfiguration_t *filesystemConfig, uint8_t currentSector,
                       uint8_t newSector) {
    fileSystemEntry_t fse = *filesystemGetEntryBySector(filesystemConfig, currentSector);

    uint32_t currentAddress = (currentSector) * (flashConfig->flashBytesPerSector);
    uint32_t newAddress = (newSector) * (flashConfig->flashBytesPerSector);

    // clear new sectors before writing
    for (int i = 0; i < fse.entry.numberOfSectors; i++) {
        flashEraseSector(flashConfig, newAddress + (i * FLASH_BYTES_PER_SECTOR));
    }

    // Write file to new sectors
    size_t numberOfPages = (size_t)ceilf((float)fse.entry.size) / flashConfig->flashBytesPerPage;

    size_t page = 0;
    uint8_t data[flashConfig->flashBytesPerPage];
    data_t pageBuffer = {.data = data, .length = flashConfig->flashBytesPerPage};

    while (page < numberOfPages) {
        flashReadData(flashConfig, currentAddress + (page * flashConfig->flashBytesPerPage),
                      &pageBuffer);

        flashWritePage(flashConfig, newAddress + (page * flashConfig->flashBytesPerPage),
                       pageBuffer.data, flashConfig->flashBytesPerPage);
        page++;
    }

    // clear old sectors
    for (int i = 0; i < fse.entry.numberOfSectors; i++) {
        flashEraseSector(flashConfig, currentAddress + (i * FLASH_BYTES_PER_SECTOR));
    }
}

void fileSystemEntryToByteArray(filesystemConfiguration_t *filesystemConfig,
                                fileSystemEntry_t *data, uint8_t *byteArray, uint8_t index) {
    size_t offset = index * filesystemConfig->entrySize;
    byteArray[offset] = (uint8_t)data->entry.id;
    offset++;

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset++] = (uint8_t)(data->entry.startSector & 0xFF);
    }

    // shift to write 8 bits after each other to the array
    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        byteArray[offset++] = (uint8_t)((data->entry.size >> (i * 8)) & 0xFF);
    }

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset++] = (uint8_t)((data->entry.isConfig) & 0xFF);
    }

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset++] = (uint8_t)((data->entry.numberOfSectors >> (i * 8)) & 0xFF);
    }
}

void printFileSystemEntry(fileSystemEntry_t entry) {
    PRINT_DEBUG("ID:                %d\n", entry.entry.id);
    PRINT_DEBUG("Start Sector:      %d\n", entry.entry.startSector);
    PRINT_DEBUG("Size:              %d\n", entry.entry.size);
    PRINT_DEBUG("isConfig:          %d\n", entry.entry.isConfig);
    PRINT_DEBUG("Number of Sectors: %d\n", entry.entry.numberOfSectors);
    PRINT_DEBUG("---\n");
}

bool checkIfFileSystemExists(flashConfiguration_t *flashConfig,
                             filesystemConfiguration_t *filesystemConfig) {
    uint8_t data[filesystemConfig->entrySize];
    data_t entryBuffer = {.data = data, .length = filesystemConfig->entrySize};
    // FileSystem can only be in Sectors 44 ... 51

    for (int i = 6; i <= 9; i++) {
        uint32_t sectorStartAddress = i * FLASH_BYTES_PER_SECTOR;
        flashReadData(flashConfig, sectorStartAddress, &entryBuffer);

        if (entryBuffer.data[0] != 255) {
            PRINT_DEBUG("\n");
            PRINT_DEBUG("FileSystem found in sector %d.\n", i);
            filesystemConfig->nextFileSystemSector = i;

            inc(filesystemConfig);
            return true;
        }

        free(entryBuffer.data);
        sectorStartAddress += i * FLASH_BYTES_PER_SECTOR;
    }
    PRINT_DEBUG("\n");
    PRINT_DEBUG("No FileSystem found...\n");
    PRINT_DEBUG("\n");
    return false;
}

void printBinary(uint8_t byte) {
    int bits = sizeof(byte) * 8;
    for (int i = bits - 1; i >= 0; i--) {
        uint32_t mask = 1 << i;
        putchar((byte & mask) ? '1' : '0');
    }
    putchar('\n');
}

void checkNumberOfEntries(flashConfiguration_t *flashConfig,
                          filesystemConfiguration_t *filesystemConfig) {
    uint32_t sector;
    if (filesystemConfig->nextFileSystemSector == 6) {
        sector = 9;
    } else {
        sector = filesystemConfig->nextFileSystemSector - 1;
    }

    uint32_t fileSystemStartAddress = sector * FLASH_BYTES_PER_SECTOR;
    uint8_t currentEntry = 0;

    uint8_t data[filesystemConfig->entrySize];

    data_t entryBuffer = {.data = data, .length = filesystemConfig->entrySize};

    while (true) {
        flashReadData(flashConfig,
                      fileSystemStartAddress + (currentEntry * filesystemConfig->entrySize),
                      &entryBuffer);

        // iterate over bytes until uint32_t is filled
        uint32_t entry_size = 0;
        uint8_t offset = 0;
        for (int i = 2; i < sizeof(uint32_t) + 2; i++) {
            entry_size |= (uint32_t)(entryBuffer.data[i] << offset * 8);
            ++offset;
        }

        uint8_t entry_id = entryBuffer.data[4];
        uint8_t entry_startSector = entryBuffer.data[5];
        uint8_t entry_isConfig = entryBuffer.data[6];
        uint8_t entry_numberOfSectors = entryBuffer.data[7];

        if (entry_id == 255) {
            break;
        }

        reconstructEntry(filesystemConfig, entry_id, "", entry_startSector, entry_size,
                         entry_isConfig, entry_numberOfSectors);
        filesystemConfig->currentID = entry_id + 1;
        currentEntry++;
    }
    PRINT_DEBUG("Number of entries: %d\n", currentEntry);
    PRINT_DEBUG("\n");
    filesystemConfig->numberOfEntries = currentEntry;
}

void deleteFileSystemEntry(flashConfiguration_t *flashConfig,
                           filesystemConfiguration_t *filesystemConfig, uint8_t index) {
    for (int i = index + 1; i < filesystemConfig->numberOfEntries; i++) {
        filesystemConfig->fileSystem[i - 1] = filesystemConfig->fileSystem[i];
    }
    memset(&filesystemConfig->fileSystem[filesystemConfig->numberOfEntries - 1], 0xFF,
           sizeof(fileSystemEntry_t));
    --filesystemConfig->numberOfEntries;
}

void writeFileSystemToFlash(flashConfiguration_t *flashConfig,
                            filesystemConfiguration_t *filesystemConfig) {
    // Clear old data from pages to be used by new file system
    PRINT_DEBUG("Writing new File System to Sector %i\n", filesystemConfig->nextFileSystemSector);

    // Erase new sector before writing
    uint32_t newAddress = filesystemConfig->nextFileSystemSector * FLASH_BYTES_PER_SECTOR;
    flashEraseSector(flashConfig, newAddress);

    // Convert fileEntry Structs to 8-bit array to use flashWritePage()
    uint8_t dataByteArray[filesystemConfig->numberOfEntries * filesystemConfig->entrySize];
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        fileSystemEntry_t *ptr = &filesystemConfig->fileSystem[i];
        // printFileSystemEntry(fileSystem[i]);
        fileSystemEntryToByteArray(filesystemConfig, ptr, dataByteArray, i);
    }

    // wieder implementieren, wenn FileSystem größer als eine Page ist
    // Write Byte Array to Flash
    flashWritePage(flashConfig, newAddress, dataByteArray,
                   filesystemConfig->numberOfEntries * filesystemConfig->entrySize);

    // update Free Sector Array and erase old sector
    if (filesystemConfig->nextFileSystemSector == 6) {
        filesystemConfig->sectorFree[9] = 1;
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;

        uint32_t address = 9 * flashConfig->flashBytesPerSector;
        flashEraseSector(flashConfig, address);
    } else {
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector - 1] = 1;
        filesystemConfig->sectorFree[filesystemConfig->nextFileSystemSector] = 0;

        uint32_t currentFileSystemSector = filesystemConfig->nextFileSystemSector - 1;
        uint32_t address = currentFileSystemSector * flashConfig->flashBytesPerSector;
        flashEraseSector(flashConfig, address);
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
