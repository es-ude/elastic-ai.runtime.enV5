#include "math.h"

#include "FileSystem.h"
#include "FileSystemInternal.h"
#include "Flash.h"
#include "middleware.h"
#include "Common.h"
#include "EnV5HwConfiguration.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

uint8_t numberOfEntries = 0;
uint8_t sectorFree[64];
uint8_t currentID = 5;

uint32_t nextFileSystemSector = 0;
fileSystemEntry fileSystem[51];
size_t fileSystemLength = sizeof(fileSystem) / sizeof(fileSystemEntry);
/*uint8_t entrySize = sizeof(fileSystemEntry);*/
uint8_t entrySize = 8;

/*
// PRIO
//  random löschen vom FileSystem fixen             !! vorübergehend durch rev1 gefixt -> mit David sprechen
//  neue download-Funktion die Pfad und Größe der Datei entgegennimmt und checkt


//  erase von Sektor löschen auf ganzes File löschen anpassen (quick fix)
// und selber entscheidet in welchen Sektor geschrieben wird
// größe von FileSystem dynamisch anpassen (immer Größe von altem Array +1)
//  name wieder implementieren (vielleicht auch Suche nach namen)
*/



void inc() {
    if (nextFileSystemSector == 51) {
        nextFileSystemSector = 44;
    }

    else {
        nextFileSystemSector = nextFileSystemSector + 1;
    }
}

void initFileSystem(flashConfiguration_t flashConfig) {
    if(checkIfFileSystemExists(&flashConfig)) {
        checkNumberOfEntries(&flashConfig);

        // initialize array to all 1
        for (int i = 0; i < sizeof(sectorFree); i++) {
            sectorFree[i] = 1;
        }
        // set sector of filesystem to 0
        sectorFree[nextFileSystemSector-1] = 0;

        // for each entry, set all used sectors to 0
        for (int i = 0; i < numberOfEntries; i++) {
            fileSystemEntry entry = fileSystem[i];
            for (int j = 0; j < entry.numberOfSectors; j++) {
                sectorFree[entry.startSector + j] = 0;
            }
        }
        return;
    }


    nextFileSystemSector = 44;
    // initially all sectors are free except for the sector containing the file system
    for (int i = 0; i < sizeof(sectorFree); i++) {
        sectorFree[i] = 1;
    }
    sectorFree[nextFileSystemSector] = 0;
}

void initSectorFree(flashConfiguration_t flashConfig) {

}


void reconstructEntry(uint8_t id, char name[], uint8_t sector, uint32_t size, uint8_t isConfig, uint8_t numberOfSectors) {
    fileSystem[numberOfEntries].id = id;
    fileSystem[numberOfEntries].startSector = sector;
    fileSystem[numberOfEntries].size = size;
    fileSystem[numberOfEntries].isConfig = isConfig;
    fileSystem[numberOfEntries].numberOfSectors = numberOfSectors;
    ++numberOfEntries;
}

void addNewFileSystemEntry(char name[] , uint8_t sector,  uint32_t size, uint8_t isConfig){
    uint8_t id = currentID;
    currentID++;
    fileSystem[numberOfEntries].id = id;
    for (int i = 0; i <= sizeof(*name); i++) {
        //fileSystem[numberOfEntries].name[i] = name[i];
    }
    fileSystem[numberOfEntries].startSector = sector;
    fileSystem[numberOfEntries].size = size;
    fileSystem[numberOfEntries].isConfig = isConfig;

    // Set number of used sectors for this file
    fileSystem[numberOfEntries].numberOfSectors = (size_t)ceilf((float)fileSystem[numberOfEntries].size / FLASH_BYTES_PER_SECTOR);

    // set all used sectors to used
    for (int i = 0; i < fileSystem[numberOfEntries].numberOfSectors; i++) {
        sectorFree[sector + i] = 0;
    }

    // update number of entries
    ++numberOfEntries;
}

void deleteFileSystemEntry(uint8_t index) {
    --numberOfEntries;
    fileSystemEntry newFileSystem[numberOfEntries];

    for (int i = 0; i < index; i++) {
        newFileSystem[i] = fileSystem[i];
    }
    for (int i = index + 1; i < numberOfEntries; i++) {
        newFileSystem[i] = fileSystem[i];
    }

    memcpy(newFileSystem, fileSystem, numberOfEntries);
}

void updateEntrySector(uint8_t oldSector, uint8_t newSector) {
    fileSystemEntry* fse = getEntryBySector(oldSector);
    fse->startSector = newSector;
    PRINT("Sector of entry updated to %d\n", fse->startSector);
}

void moveFileToSector(flashConfiguration_t* flashConfig, uint8_t currentSector, uint8_t newSector) {

    int index = getIndexBySector(currentSector);
    if (index == -1) {
        return;
    }
    fileSystemEntry fse = fileSystem[index];

    uint8_t requiredSectors = fse.numberOfSectors;
    char input;
    printf("Number of required sectors: %d\n", requiredSectors);

    // Check what to do, if sector isn't empty
    for (int i = 0; i < requiredSectors; i++) {
        if (sectorFree[newSector + i] == 0) {
            printf("Sector %d already contains data. Do you want to overwrite? y/n\n", newSector+i);
            scanf("%c", &input);

            if (input == 'n') {
                printf("Aborting...\n");
                return;
            }

            // check if entry with this startsector exists and remove it (as it is overwritten)
            index = getIndexBySector(newSector);
            if (index != -1) {
                deleteFileSystemEntry(index);
            }
        }
    }

    printf("Writing sector %d to sector %d... \n", currentSector, newSector);
    writeFileToSector(flashConfig, currentSector, newSector);
    updateEntrySector(currentSector, newSector);
    for (int i = 0; i < fse.numberOfSectors; i++) {
        sectorFree[currentSector + i] = 1;
        sectorFree[newSector + i] = 0;
    }
    writeFileSystemToFlash(flashConfig);
}

uint8_t getNumberOfFreeSectors() {
    uint8_t count = 0;
    for (int i = 0; i < sizeof(sectorFree); i++) {
        if (sectorFree[i] == 1) {
            count++;
        }
    }
    return count;
}

void writeFileSystemToFlash(flashConfiguration_t* flashConfig){
    // Clear old data from pages to be used by new file system
    printf("Writing File System to Sector %i\n", nextFileSystemSector);

    // Erase new sector before writing
    uint32_t newAddress = nextFileSystemSector * FLASH_BYTES_PER_SECTOR;
    flashEraseSector(flashConfig, newAddress);

    // Convert fileEntry Structs to 8-bit array to use flashWritePage()
    uint8_t dataByteArray[numberOfEntries * entrySize];
    for(int i = 0; i < numberOfEntries; i++){
        fileSystemEntry *ptr = &fileSystem[i];
        printFileSystemEntry(fileSystem[i]);
        fileSystemEntryToByteArray(ptr, dataByteArray, i);
    }

    // wieder implementieren, wenn FileSystem größer als eine Page ist
    // Write Byte Array to Flash
    flashWritePage(flashConfig, newAddress, dataByteArray, numberOfEntries * entrySize);

    // update Free Sector Array and erase old sector
    if (nextFileSystemSector == 44) {
        sectorFree[51] = 1;
        sectorFree[nextFileSystemSector] = 0;

        printf("Deleting sector 51, because current sector is %i\n", nextFileSystemSector);
        uint32_t address = 51* flashConfig->flashBytesPerSector;
        flashEraseSector(flashConfig, address);
    }
    else {
        sectorFree[nextFileSystemSector-1] = 1;
        sectorFree[nextFileSystemSector] = 0;

        uint32_t currentFileSystemSector = nextFileSystemSector -1;
        uint32_t address = currentFileSystemSector * flashConfig->flashBytesPerSector;
        flashEraseSector(flashConfig, address);
        printf("Deleting sector %i\n", currentFileSystemSector);
    }
    inc();

    printf("DONE...\n");
}

void printFSS() {
    printf("Next File System Sector: %i\n", nextFileSystemSector);
}

void printFileSystem(){
    printf("\n");
    printf("---File System---\n");
    for(int i = 0; i < numberOfEntries; i++){
        printFileSystemEntry(fileSystem[i]);
    }
}

void reconstructFileSystemFromFlash(flashConfiguration_t* flashConfig) {


    /*uint8_t counter = 0;

    // find number of entries
    while data != 251{
        ++counter;
    }

    for (int i = 0; i < counter; i++) {

    }*/
}

void byteArrayToFileSystemEntry(uint8_t *array[entrySize]) {
    uint8_t id = *array[0];
    uint64_t name = (uint64_t)*array[1];
    uint8_t startSector = *array[9];
    uint32_t size = (uint32_t)*array[10];
    uint8_t isConfig = *array[14];
    uint8_t numberOfSectors = *array[15];
}

fileSystemEntry *readByID(uint8_t id) {
    for (int i = 0; i <= numberOfEntries; i++) {
        printf("%d, %d\n", fileSystem[i].id, id);
        if (fileSystem[i].id == id) {
            return &fileSystem[i];
        }
    }
    printf("No entry found with this id.");
    return NULL;
}

fileSystemEntry *readByIndex(uint8_t index) {
    for (int i = 0; i <= numberOfEntries; i++) {
        printf("%d, %d\n", i, index);
        if (i == index) {
            return &fileSystem[i];
        }
    }
    printf("No entry found at this index.");
    return NULL;
}



// ----------------------------------------------------------------------------------------------------

// Internal Functions

fileSystemEntry* getEntryBySector(uint8_t sector) {
    for (int i = 0; i <= numberOfEntries; i++) {
        if (fileSystem[i].startSector == sector) {
            return &fileSystem[i];
        }
    }
    printf("No entry found with this address...\n");
    return NULL;
}

int getIndexBySector(uint8_t sector) {
    for (int i = 0; i < numberOfEntries; i++) {
        if (fileSystem[i].startSector == sector) {
            return i;
        }
    }
    printf("No entry found with this address...\n");
    return -1;
}

void writeFileToSector(flashConfiguration_t* flashConfig, uint8_t currentSector, uint8_t newSector){
    fileSystemEntry fse = *getEntryBySector(currentSector);

    uint32_t currentAddress = (currentSector) * (flashConfig->flashBytesPerSector);
    uint32_t newAddress = (newSector) * (flashConfig->flashBytesPerSector);

    // clear new sectors before writing
    for (int i = 0; i < fse.numberOfSectors; i++) {
        flashEraseSector(flashConfig, newAddress + (i * FLASH_BYTES_PER_SECTOR));
    }
    printf("New Sectors erased.");

    // Write file to new sectors
    printf("Writing file to new sectors...\n");
    size_t numberOfPages = (size_t)ceilf((float)fse.size) / flashConfig->flashBytesPerPage;

    printf("Number of sectors: %d\n", fse.numberOfSectors);
    printf("Number of pages: %d\n", numberOfPages);

    size_t page = 0;
    data_t pageBuffer = {.data = NULL, .length = flashConfig->flashBytesPerPage};

    while (page < numberOfPages) {
        // create buffer
        pageBuffer.data = calloc(flashConfig->flashBytesPerPage, sizeof(uint8_t));
        // read 1 page
        flashReadData(flashConfig, currentAddress + (page * flashConfig->flashBytesPerPage), &pageBuffer);

        flashWritePage(flashConfig, newAddress + (page * flashConfig->flashBytesPerPage), pageBuffer.data, flashConfig->flashBytesPerPage);
        free(pageBuffer.data);
        page++;
    }

    // clear old sectors
    for (int i = 0; i < fse.numberOfSectors; i++) {
        flashEraseSector(flashConfig, currentAddress + (i * FLASH_BYTES_PER_SECTOR));
        printf("Erased data in sector %i.\n", currentSector+i);
    }


    printf("DONE!\n");
}

void fileSystemEntryToByteArray(fileSystemEntry* data, uint8_t* byteArray, uint8_t index){
    size_t offset = index * entrySize;
    byteArray[offset] = (uint8_t)data->id;
    offset++;

    for(size_t i = 0; i < sizeof(uint8_t); i++){
        byteArray[offset++] = (uint8_t)(data->startSector & 0xFF);
    }

    // shift to write 8 bits after each other to the array
    for(size_t i = 0; i < sizeof(uint32_t); i++){
        byteArray[offset++] = (uint8_t)((data->size >> (i*8)) & 0xFF);
    }

    for(size_t i = 0; i < sizeof(uint8_t); i++){
        byteArray[offset++] = (uint8_t)((data->isConfig) & 0xFF);
    }

    for(size_t i = 0; i < sizeof(uint8_t); i++){
        byteArray[offset++] = (uint8_t)((data->numberOfSectors >> (i*8)) & 0xFF);
    }
}

void printFileSystemEntry(fileSystemEntry entry){
    printf("ID: %d\n", entry.id);
    printf("Start Sector: %d\n", entry.startSector);
    printf("Size: %d\n", entry.size);
    printf("isConfig: %d\n", entry.isConfig);
    printf("Number of Sectors: %d\n", entry.numberOfSectors);
    printf("---\n");
}

bool checkIfFileSystemExists(flashConfiguration_t* flashConfig) {
    data_t entryBuffer = {.data = NULL, .length = entrySize};
    // FileSystem can only be in Sectors 44 ... 51

    for (int i = 44; i <= 51; i++) {
        uint32_t sectorStartAddress = i * FLASH_BYTES_PER_SECTOR;
        entryBuffer.data = calloc(entrySize, sizeof(uint8_t));
        flashReadData(flashConfig, sectorStartAddress, &entryBuffer);

        if (entryBuffer.data[0] != 255) {
            printf("FileSystem found in sector %d.\n", i);
            nextFileSystemSector = i;

            inc();
            free(entryBuffer.data);
            return true;
        }

        free(entryBuffer.data);
        sectorStartAddress += i * FLASH_BYTES_PER_SECTOR;
    }
    printf("No FileSystem found...\n");
    return false;
}

void printBinary(uint8_t byte) {
    int bits = sizeof(byte) * 8; // Number of bits in the type
    for (int i = bits - 1; i >= 0; i--) {
        uint32_t mask = 1 << i; // Create a mask with a 1 at the ith position
        putchar((byte & mask) ? '1' : '0');
    }
    putchar('\n');
}

void checkNumberOfEntries(flashConfiguration_t* flashConfig) {
    uint32_t sector;
    if (nextFileSystemSector == 44) {
        sector = 51;
    }
    else {
        sector = nextFileSystemSector -1;
    }

    uint32_t fileSystemStartAddress = sector * FLASH_BYTES_PER_SECTOR;
    data_t entryBuffer = {.data = NULL, .length = entrySize};

    uint8_t currentEntry = 0;

    while (true){
        entryBuffer.data = calloc(entrySize, sizeof(uint8_t));
        flashReadData(flashConfig, fileSystemStartAddress + (currentEntry * entrySize), &entryBuffer);
        fileSystemEntry fse;

        // print for testing purposes
        /*for (int i = 0; i < entrySize; i++) {
            if (i == 0) {
                printf("id: ");
            }
            else if (i == 1) {
                printf("startSector: ");
            }
            else if (i == 2) {
                printf("size: ");
            }
            else if (i == 6) {
                printf("isConfig: ");
            }
            else if (i == 7) {
                printf("numberOfSectors: ");
            }

            printBinary(entryBuffer.data[i]);
        }*/

        uint8_t entry_id = entryBuffer.data[0];
        //uint64_t entry_name = (uint64_t)(entryBuffer.data[1]);
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

        if (entry_id == 255){
            break;
        }

        reconstructEntry(entry_id, "", entry_startSector, entry_size, entry_isConfig, entry_numberOfSectors);
        currentID = entry_id + 1;
        currentEntry++;
        free(entryBuffer.data);
    }
    printf("Number of entries: %d\n", currentEntry);
}


