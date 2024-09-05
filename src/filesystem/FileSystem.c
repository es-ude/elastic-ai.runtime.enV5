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

static uint8_t numberOfEntries = 0;
static uint8_t currentID = 100;

static uint32_t nextFileSystemSector;
static fileSystemEntry fileSystem[60];
static size_t fileSystemLength = sizeof(fileSystem) / sizeof(fileSystemEntry);
static uint8_t entrySize = 8;

uint32_t nextFileSector = 0;
uint8_t sectorFree[10];

void filesystemInit(flashConfiguration_t flashConfig) {
    if (checkIfFileSystemExists(&flashConfig)) {
        checkNumberOfEntries(&flashConfig);

        // initialize array to all 1
        for (int i = 0; i < sizeof(sectorFree); i++) {
            sectorFree[i] = 1;
        }

        // set sector of filesystem to 0
        sectorFree[nextFileSystemSector - 1] = 0;

        // for each entry, set all used sectors to 0
        for (int i = 0; i < numberOfEntries; i++) {
            fileSystemEntry entry = fileSystem[i];
            for (int j = 0; j < entry.numberOfSectors; j++) {
                sectorFree[entry.startSector + j] = 0;
            }
        }

        for (int i = 0; i < sizeof(sectorFree); i++) {
            if (sectorFree[i] == 1) {
                nextFileSector = i;
                break;
            }
        }

        return;
    }

    nextFileSystemSector = 6;
    // initially all sectors are free except for the sector containing the file system
    for (int i = 0; i < sizeof(sectorFree); i++) {
        sectorFree[i] = 1;
    }
    sectorFree[nextFileSystemSector] = 0;
}

bool filesystemFindFittingStartSector(uint8_t numberOfRequiredSectors) {
    uint32_t tempNewFileSector = nextFileSector;
    uint8_t counter = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (sectorFree[i] == 1 && counter == 0) {
            tempNewFileSector = i;
            ++counter;
        } else if (sectorFree[i] == 1 && counter < numberOfRequiredSectors) {
            ++counter;
        } else if (counter == numberOfRequiredSectors) {
            nextFileSector = tempNewFileSector;
            return true;
        } else {
            counter = 0;
        }
    }
    return false;
}

void filesystemAddNewFileSystemEntry(flashConfiguration_t *flashConfig, uint32_t size, uint8_t isConfig) {
    uint8_t sector = nextFileSector;
    uint8_t id = currentID;
    currentID++;
    fileSystem[numberOfEntries].id = id;
    /*for (int i = 0; i <= sizeof(*name); i++) {
        //fileSystem[numberOfEntries].name[i] = name[i];
    }*/
    fileSystem[numberOfEntries].startSector = sector;
    fileSystem[numberOfEntries].size = size;
    fileSystem[numberOfEntries].isConfig = isConfig;

    // Set number of used sectors for this file
    fileSystem[numberOfEntries].numberOfSectors =
        (size_t)ceilf((float)fileSystem[numberOfEntries].size / FLASH_BYTES_PER_SECTOR);

    // set all used sectors to used
    for (int i = 0; i < fileSystem[numberOfEntries].numberOfSectors; i++) {
        sectorFree[sector + i] = 0;
    }

    // update number of entries
    ++numberOfEntries;
    PRINT("SLOW");
    while (1) {}
    writeFileSystemToFlash(flashConfig);
}

void filesystemMoveFileToSector(flashConfiguration_t *flashConfig, uint8_t ID, uint8_t newSector) {
    fileSystemEntry *entry = filesystemGetEntryByID(ID);
    uint8_t currentSector = entry->startSector;
    int index = getIndexBySector(currentSector);
    if (index == -1) {
        return;
    }
    fileSystemEntry fse = fileSystem[index];

    uint8_t requiredSectors = fse.numberOfSectors;
    char input;
    PRINT("Number of required sectors: %d\n", requiredSectors);

    // Check what to do, if sector isn't empty
    for (int i = 0; i < requiredSectors; i++) {
        if (sectorFree[newSector + i] == 0) {
            PRINT("Sector %d already contains data. Do you want to overwrite? y/n\n",
                   newSector + i);
            scanf("%c", &input);

            if (input == 'n') {
                PRINT("Aborting...\n");
                return;
            }

            // check if entry with this startsector exists and remove it (as it is overwritten)
            index = getIndexBySector(newSector);
            if (index != -1) {
                deleteFileSystemEntry(flashConfig, index);
            }
        }
    }

    PRINT("Writing File with ID %d to sector %d... \n", ID, newSector);
    writeFileToSector(flashConfig, currentSector, newSector);
    updateEntrySector(currentSector, newSector);
    for (int i = 0; i < fse.numberOfSectors; i++) {
        sectorFree[currentSector + i] = 1;
        sectorFree[newSector + i] = 0;
    }
    writeFileSystemToFlash(flashConfig);
}

bool filesystemEraseFileByID(flashConfiguration_t *flashConfig, uint8_t id) {
    fileSystemEntry *entry = filesystemGetEntryByID(id);
    if (entry == NULL) {
        PRINT("No entry with this ID. Aborting...\n");
        return false;
    }

    for (int i = 0; i < entry->numberOfSectors; i++) {
        uint32_t address = entry->startSector + i * FLASH_BYTES_PER_SECTOR;
        flashEraseSector(flashConfig, address);
        sectorFree[entry->startSector + i] = 1;
    }
    deleteFileSystemEntry(flashConfig, getIndexBySector(entry->startSector));
    writeFileSystemToFlash(flashConfig);
    return true;
}

void filesystemPrintFileSystem() {
    PRINT("\n");
    PRINT("---File System---\n");
    PRINT("Number of entries: %i\n", numberOfEntries);
    PRINT("\n");
    for (int i = 0; i < numberOfEntries; i++) {
        printFileSystemEntry(fileSystem[i]);
    }
    PRINT("\n");
}

uint8_t filesystemGetNumberOfFreeSectors() {
    uint8_t count = 0;
    for (int i = 0; i < sizeof(sectorFree); i++) {
        if (sectorFree[i] == 1) {
            count++;
        }
    }
    return count;
}

fileSystemEntry *filesystemGetEntryByID(uint8_t id) {
    for (int i = 0; i < numberOfEntries; i++) {
        if (fileSystem[i].id == id) {
            return &fileSystem[i];
        }
    }
    return NULL;
}

fileSystemEntry *filesystemGetEntryByIndex(uint8_t index) {
    for (int i = 0; i <= numberOfEntries; i++) {
        PRINT("%d, %d\n", i, index);
        if (i == index) {
            return &fileSystem[i];
        }
    }
    PRINT("No entry found at this index.");
    return NULL;
}

fileSystemEntry *filesystemGetEntryBySector(uint8_t sector) {
    for (int i = 0; i <= numberOfEntries; i++) {
        if (fileSystem[i].startSector == sector) {
            return &fileSystem[i];
        }
    }
    PRINT("No entry found with this address...\n");
    return NULL;
}

void filesystemSortFileSystemByStartSector() {
    qsort(fileSystem, numberOfEntries, sizeof(fileSystemEntry), compareStartSectors);
}

void filesystemSortFileSystemByID() {
    qsort(fileSystem, numberOfEntries, sizeof(fileSystemEntry), compareIDs);
}

// ----------------------------------------------------------------------------------------------------

// Internal Functions

void inc() {
    if (nextFileSystemSector == 9) {
        nextFileSystemSector = 6;
    }

    else {
        nextFileSystemSector = nextFileSystemSector + 1;
    }
}

void updateEntrySector(uint8_t oldSector, uint8_t newSector) {
    fileSystemEntry *fse = filesystemGetEntryBySector(oldSector);
    fse->startSector = newSector;
}

void reconstructEntry(uint8_t id, char name[], uint8_t sector, uint32_t size, uint8_t isConfig,
                      uint8_t numberOfSectors) {
    fileSystem[numberOfEntries].id = id;
    fileSystem[numberOfEntries].startSector = sector;
    fileSystem[numberOfEntries].size = size;
    fileSystem[numberOfEntries].isConfig = isConfig;
    fileSystem[numberOfEntries].numberOfSectors = numberOfSectors;
    ++numberOfEntries;
}

void byteArrayToFileSystemEntry(uint8_t *array[entrySize]) {
    uint8_t id = *array[0];
    uint64_t name = (uint64_t)*array[1];
    uint8_t startSector = *array[9];
    uint32_t size = (uint32_t)*array[10];
    uint8_t isConfig = *array[14];
    uint8_t numberOfSectors = *array[15];
}

int getIndexBySector(uint8_t sector) {
    for (int i = 0; i < numberOfEntries; i++) {
        if (fileSystem[i].startSector == sector) {
            return i;
        }
    }
    PRINT("No entry found with this address...\n");
    return -1;
}

void writeFileToSector(flashConfiguration_t *flashConfig, uint8_t currentSector,
                       uint8_t newSector) {
    fileSystemEntry fse = *filesystemGetEntryBySector(currentSector);

    uint32_t currentAddress = (currentSector) * (flashConfig->flashBytesPerSector);
    uint32_t newAddress = (newSector) * (flashConfig->flashBytesPerSector);

    // clear new sectors before writing
    for (int i = 0; i < fse.numberOfSectors; i++) {
        flashEraseSector(flashConfig, newAddress + (i * FLASH_BYTES_PER_SECTOR));
    }

    // Write file to new sectors
    size_t numberOfPages = (size_t)ceilf((float)fse.size) / flashConfig->flashBytesPerPage;

    size_t page = 0;
    data_t pageBuffer = {.data = NULL, .length = flashConfig->flashBytesPerPage};

    while (page < numberOfPages) {
        // create buffer
        pageBuffer.data = calloc(flashConfig->flashBytesPerPage, sizeof(uint8_t));
        // read 1 page
        flashReadData(flashConfig, currentAddress + (page * flashConfig->flashBytesPerPage),
                      &pageBuffer);

        flashWritePage(flashConfig, newAddress + (page * flashConfig->flashBytesPerPage),
                       pageBuffer.data, flashConfig->flashBytesPerPage);
        free(pageBuffer.data);
        page++;
    }

    // clear old sectors
    for (int i = 0; i < fse.numberOfSectors; i++) {
        flashEraseSector(flashConfig, currentAddress + (i * FLASH_BYTES_PER_SECTOR));
    }
}

void fileSystemEntryToByteArray(fileSystemEntry *data, uint8_t *byteArray, uint8_t index) {
    size_t offset = index * entrySize;
    byteArray[offset] = (uint8_t)data->id;
    offset++;

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset++] = (uint8_t)(data->startSector & 0xFF);
    }

    // shift to write 8 bits after each other to the array
    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        byteArray[offset++] = (uint8_t)((data->size >> (i * 8)) & 0xFF);
    }

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset++] = (uint8_t)((data->isConfig) & 0xFF);
    }

    for (size_t i = 0; i < sizeof(uint8_t); i++) {
        byteArray[offset++] = (uint8_t)((data->numberOfSectors >> (i * 8)) & 0xFF);
    }
}

void printFileSystemEntry(fileSystemEntry entry) {
    PRINT("ID:                %d\n", entry.id);
    PRINT("Start Sector:      %d\n", entry.startSector);
    PRINT("Size:              %d\n", entry.size);
    PRINT("isConfig:          %d\n", entry.isConfig);
    PRINT("Number of Sectors: %d\n", entry.numberOfSectors);
    PRINT("---\n");
}

bool checkIfFileSystemExists(flashConfiguration_t *flashConfig) {
    data_t entryBuffer = {.data = NULL, .length = entrySize};
    // FileSystem can only be in Sectors 44 ... 51

    for (int i = 6; i <= 9; i++) {
        uint32_t sectorStartAddress = i * FLASH_BYTES_PER_SECTOR;
        entryBuffer.data = calloc(entrySize, sizeof(uint8_t));
        flashReadData(flashConfig, sectorStartAddress, &entryBuffer);

        if (entryBuffer.data[0] != 255) {
            PRINT("\n");
            PRINT("FileSystem found in sector %d.\n", i);
            nextFileSystemSector = i;

            inc();
            free(entryBuffer.data);
            return true;
        }

        free(entryBuffer.data);
        sectorStartAddress += i * FLASH_BYTES_PER_SECTOR;
    }
    PRINT("\n");
    PRINT("No FileSystem found...\n");
    PRINT("\n");
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

void checkNumberOfEntries(flashConfiguration_t *flashConfig) {
    uint32_t sector;
    if (nextFileSystemSector == 6) {
        sector = 9;
    } else {
        sector = nextFileSystemSector - 1;
    }

    uint32_t fileSystemStartAddress = sector * FLASH_BYTES_PER_SECTOR;
    data_t entryBuffer = {.data = NULL, .length = entrySize};

    uint8_t currentEntry = 0;

    while (true) {
        entryBuffer.data = calloc(entrySize, sizeof(uint8_t));
        flashReadData(flashConfig, fileSystemStartAddress + (currentEntry * entrySize),
                      &entryBuffer);

        uint8_t entry_id = entryBuffer.data[0];
        // uint64_t entry_name = (uint64_t)(entryBuffer.data[1]);
        uint8_t entry_startSector = entryBuffer.data[1];

        // iterate over bytes until uint32_t is filled
        uint32_t entry_size = 0;
        uint8_t offset = 0;
        for (int i = 2; i < sizeof(uint32_t) + 2; i++) {
            entry_size |= (uint32_t)(entryBuffer.data[i] << offset * 8);
            ++offset;
        }

        uint8_t entry_isConfig = entryBuffer.data[6];
        uint8_t entry_numberOfSectors = entryBuffer.data[7];

        if (entry_id == 255) {
            break;
        }

        reconstructEntry(entry_id, "", entry_startSector, entry_size, entry_isConfig,
                         entry_numberOfSectors);
        currentID = entry_id + 1;
        currentEntry++;
        free(entryBuffer.data);
    }
    PRINT("Number of entries: %d\n", currentEntry);
    PRINT("\n");
    numberOfEntries = currentEntry;
    // nextFileSector = currentEntry-1;
}

void deleteFileSystemEntry(flashConfiguration_t *flashConfig, uint8_t index) {
    for (int i = index + 1; i < numberOfEntries; i++) {
        fileSystem[i - 1] = fileSystem[i];
    }
    memset(&fileSystem[numberOfEntries - 1], 0xFF, sizeof(fileSystemEntry));
    --numberOfEntries;
}

void writeFileSystemToFlash(flashConfiguration_t *flashConfig) {
    // Clear old data from pages to be used by new file system
    PRINT("Writing new File System to Sector %i\n", nextFileSystemSector);

    // Erase new sector before writing
    uint32_t newAddress = nextFileSystemSector * FLASH_BYTES_PER_SECTOR;
    flashEraseSector(flashConfig, newAddress);

    // Convert fileEntry Structs to 8-bit array to use flashWritePage()
    uint8_t dataByteArray[numberOfEntries * entrySize];
    for (int i = 0; i < numberOfEntries; i++) {
        fileSystemEntry *ptr = &fileSystem[i];
        // printFileSystemEntry(fileSystem[i]);
        fileSystemEntryToByteArray(ptr, dataByteArray, i);
    }

    // wieder implementieren, wenn FileSystem größer als eine Page ist
    // Write Byte Array to Flash
    flashWritePage(flashConfig, newAddress, dataByteArray, numberOfEntries * entrySize);

    // update Free Sector Array and erase old sector
    if (nextFileSystemSector == 6) {
        sectorFree[9] = 1;
        sectorFree[nextFileSystemSector] = 0;

        uint32_t address = 9 * flashConfig->flashBytesPerSector;
        flashEraseSector(flashConfig, address);
    } else {
        sectorFree[nextFileSystemSector - 1] = 1;
        sectorFree[nextFileSystemSector] = 0;

        uint32_t currentFileSystemSector = nextFileSystemSector - 1;
        uint32_t address = currentFileSystemSector * flashConfig->flashBytesPerSector;
        flashEraseSector(flashConfig, address);
    }
    inc();

    PRINT("DONE...\n");
}

int compareStartSectors(const void *a, const void *b) {
    fileSystemEntry *entryA = (fileSystemEntry *)a;
    fileSystemEntry *entryB = (fileSystemEntry *)b;

    if (entryA->startSector < entryB->startSector) {
        return -1; // a kommt vor b
    } else if (entryA->startSector > entryB->startSector) {
        return 1; // b kommt vor a
    } else {
        return 0; // a und b sind gleich
    }
}

int compareIDs(const void *a, const void *b) {
    fileSystemEntry *entryA = (fileSystemEntry *)a;
    fileSystemEntry *entryB = (fileSystemEntry *)b;

    if (entryA->id < entryB->id) {
        return -1; // a kommt vor b
    } else if (entryA->id > entryB->id) {
        return 1; // b kommt vor a
    } else {
        return 0; // a und b sind gleich
    }
}
