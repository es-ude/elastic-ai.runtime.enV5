//
// Created by jan on 01.07.24.
//

#ifndef FILESYSTEMINTERNAL_H
#define FILESYSTEMINTERNAL_H

fileSystemEntry* getEntryBySector(uint8_t sector);
void writeFileToSector(flashConfiguration_t* flashConfig, uint8_t currentSector, uint8_t newSector);
void fileSystemEntryToByteArray(fileSystemEntry* data, uint8_t* byteArray, uint8_t index);
void byteArrayToFileSystemEntry(uint8_t *array[]);
void printFileSystemEntry(fileSystemEntry entry);
int getIndexBySector(uint8_t sector);
#endif //FILESYSTEMINTERNAL_H
