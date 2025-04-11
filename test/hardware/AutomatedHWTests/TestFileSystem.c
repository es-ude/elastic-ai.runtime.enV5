#define SOURCE_FILE "FILESYSTEM_HWTEST"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "FileSystem.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "unity.h"

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

#include <stdlib.h>

#define START_FILE_ID 100

spiConfiguration_t spiToFlashConfig = {.sckPin = FLASH_SPI_CLOCK,
                                       .misoPin = FLASH_SPI_MISO,
                                       .mosiPin = FLASH_SPI_MOSI,
                                       .baudrate = FLASH_SPI_BAUDRATE,
                                       .spiInstance = FLASH_SPI_MODULE,
                                       .csPin = FLASH_SPI_CS};

flashConfiguration_t flashConfig = {
    .spiConfiguration = &spiToFlashConfig,
};

filesystemConfiguration_t filesystemConfig;

// Helper Functions
void resetFilesystem() {
    filesystemEraseAllEntries(&filesystemConfig);
    filesystemInit(&flashConfig, &filesystemConfig);
}

bool checkIfFilesystemEntryMatchesGivenEntry(const fileSystemEntry_t *testEntry,
                                             const fileSystemEntry_t *entryToCompare) {
    return testEntry->entry.id == entryToCompare->entry.id &&
           testEntry->entry.size == entryToCompare->entry.size &&
           testEntry->entry.startSector == entryToCompare->entry.startSector &&
           testEntry->entry.isConfig == entryToCompare->entry.isConfig &&
           testEntry->entry.numberOfSectors == entryToCompare->entry.numberOfSectors;
}

bool checkSectorFree(uint16_t numberOfSectors, uint16_t startSector, bool expectedIfFree) {

    for (int i = 0; i < numberOfSectors; i++) {
        if ((bool)filesystemConfig.sectorFree[startSector + i] != expectedIfFree) {
            PRINT("WRONG SECTOR FREE VALUE!");
            PRINT("Expected: %d, got: %d", expectedIfFree,
                  filesystemConfig.sectorFree[startSector + i]);
            return false;
        }
    }
    return true;
}

bool checkIfFilesystemValuesMatchGivenValues(uint8_t numberOfEntries, uint8_t fileID,
                                             uint32_t nextFileSystemSector,
                                             uint16_t numberOfFreeSectors) {

    if (numberOfEntries != filesystemConfig.numberOfEntries) {
        PRINT("WRONG NUMBER OF ENTRIES!");
        PRINT("Expected: %d, got: %d", numberOfEntries, filesystemConfig.numberOfEntries);
        return false;
    }
    if (nextFileSystemSector != filesystemConfig.nextFileSystemSector) {
        PRINT("WRONG NEXT FILE SYSTEM SECTOR!");
        PRINT("Expected: %d, got: %d", nextFileSystemSector, filesystemConfig.nextFileSystemSector);
        return false;
    }
    if (fileID != filesystemConfig.fileID) {
        PRINT("WRONG FILE ID!");
        PRINT("Expected: %d, got: %d", fileID, filesystemConfig.fileID);
        return false;
    }
    if (numberOfFreeSectors != filesystemConfig.numberOfFreeSectors) {
        PRINT("WRONG NUMBER OF FREE SECTORS!");
        PRINT("Expected %d, got %d", numberOfFreeSectors, filesystemConfig.numberOfFreeSectors);
        return false;
    }

    return true;
}

// Unit Tests
void testFindFittingStartSectorReturnIfFileDoesntFit() {
    uint32_t numberOfRequiredBytes =
        filesystemConfig.filesystemStartSector * flashGetBytesPerSector(filesystemConfig.flash);
    uint32_t startSector =
        filesystemFindFittingStartSector(&filesystemConfig, numberOfRequiredBytes);
    TEST_ASSERT_EQUAL(-1, startSector);
}
void testFindFittingStartSectorReturnIfFileFits() {
    uint32_t numberOfRequiredBytes = 0;
    uint32_t startSector = 0;
    for (int i = 1; i < filesystemConfig.filesystemStartSector; i++) {
        numberOfRequiredBytes = i * flashGetBytesPerSector(filesystemConfig.flash);
        startSector = filesystemFindFittingStartSector(&filesystemConfig, numberOfRequiredBytes);
        TEST_ASSERT_NOT_EQUAL(-1, startSector);
    }
}
void testFindFittingStartSectorWithNonSubsequentSectors() {
    // if sector 1 is occupied, startSector should not be zero (2 subsequent sectors needed)
    uint32_t numberOfRequiredBytes = 2 * flashGetBytesPerSector(filesystemConfig.flash);
    filesystemConfig.sectorFree[1] = 0;
    uint32_t startSector =
        filesystemFindFittingStartSector(&filesystemConfig, numberOfRequiredBytes);
    TEST_ASSERT_NOT_EQUAL(0, startSector);
    filesystemConfig.sectorFree[1] = 1;
}

// Integration Tests
void testFileSystemExistsAfterAddingNewEntry() {
    resetFilesystem();

    uint8_t numberOfRequiredSectors = 1;
    uint32_t fileSize = numberOfRequiredSectors * flashGetBytesPerSector(filesystemConfig.flash);
    filesystemAddNewFileSystemEntry(&filesystemConfig, 0, fileSize, true);

    TEST_ASSERT_EQUAL(true, filesystemCheckIfFilesystemExists(&filesystemConfig));
}
void testFileSystemDoesntExistAfterErasingAllEntries() {
    filesystemEraseAllEntries(&filesystemConfig);
    TEST_ASSERT_EQUAL(false, filesystemCheckIfFilesystemExists(&filesystemConfig));
}

void testFilesystemValuesCorrectAfterNewEntry() {
    resetFilesystem();

    uint16_t startSector = 0;
    uint8_t numberOfRequiredSectors = 2;
    uint32_t fileSize = numberOfRequiredSectors * flashGetBytesPerSector(filesystemConfig.flash);
    filesystemAddNewFileSystemEntry(&filesystemConfig, 0, fileSize, true);

    // 1024 Total Sectors - 5 File System Sectors - 2 File Sectors = 1017 Free Sectors
    TEST_ASSERT_EQUAL(1, checkIfFilesystemValuesMatchGivenValues(1, 101, 1020, 1017));
    TEST_ASSERT_EQUAL(1, checkSectorFree(numberOfRequiredSectors, startSector, false));
}
void testFilesystemValuesCorrectAfterDeletingEntry() {
    resetFilesystem();

    uint16_t startSector = 0;
    uint8_t numberOfRequiredSectors = 2;
    uint32_t fileSize = numberOfRequiredSectors * flashGetBytesPerSector(filesystemConfig.flash);
    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, fileSize, true);
    filesystemEraseFileByID(&filesystemConfig, START_FILE_ID);

    // 1024 Total Sectors - 5 File System Sectors = 1019 Free Sectors
    TEST_ASSERT_EQUAL(true, checkIfFilesystemValuesMatchGivenValues(0, 101, 1021, 1019));
    TEST_ASSERT_EQUAL(1, checkSectorFree(numberOfRequiredSectors, startSector, true));
}

void testFilesystemGetEntryByIndexReturnsCorrectEntryWithValidIndex() {
    resetFilesystem();

    uint32_t sizeOfEntry = 1 * flashGetBytesPerSector(filesystemConfig.flash);
    uint32_t startSector = filesystemFindFittingStartSector(&filesystemConfig, sizeOfEntry);
    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, sizeOfEntry, 1);

    uint16_t numberOfRequiredSectors =
        ceil((double)sizeOfEntry / flashGetBytesPerSector(&flashConfig));
    fileSystemEntry_t *entry = filesystemGetEntryByIndex(&filesystemConfig, 0);

    fileSystemEntry_t testEntry = {0};
    testEntry.entry.id = START_FILE_ID;
    testEntry.entry.size = sizeOfEntry;
    testEntry.entry.startSector = startSector;
    testEntry.entry.isConfig = 1;
    testEntry.entry.numberOfSectors = numberOfRequiredSectors;

    PRINT_DEBUG("%d, %d", entry->entry.id, testEntry.entry.id);
    PRINT_DEBUG("%d, %d", entry->entry.size, testEntry.entry.size);
    PRINT_DEBUG("%d, %d", entry->entry.startSector, testEntry.entry.startSector);
    PRINT_DEBUG("%d, %d", entry->entry.isConfig, testEntry.entry.isConfig);
    PRINT_DEBUG("%d, %d", entry->entry.numberOfSectors, testEntry.entry.numberOfSectors);

    TEST_ASSERT_EQUAL(1, checkIfFilesystemEntryMatchesGivenEntry(&testEntry, entry));
}
void testFilesystemGetEntryBySectorReturnsCorrectEntryWithValidSector() {
    resetFilesystem();

    uint32_t sizeOfEntry = 1 * flashGetBytesPerSector(filesystemConfig.flash);
    uint32_t startSector = filesystemFindFittingStartSector(&filesystemConfig, sizeOfEntry);
    uint16_t numberOfRequiredSectors =
        ceil((double)sizeOfEntry / flashGetBytesPerSector(&flashConfig));

    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, sizeOfEntry, 1);
    fileSystemEntry_t *entry = filesystemGetEntryBySector(&filesystemConfig, startSector);

    fileSystemEntry_t testEntry = {0};
    testEntry.entry.id = START_FILE_ID;
    testEntry.entry.size = sizeOfEntry;
    testEntry.entry.startSector = startSector;
    testEntry.entry.isConfig = 1;
    testEntry.entry.numberOfSectors = numberOfRequiredSectors;

    PRINT_DEBUG("%d, %d", entry->entry.id, testEntry.entry.id);
    PRINT_DEBUG("%d, %d", entry->entry.size, testEntry.entry.size);
    PRINT_DEBUG("%d, %d", entry->entry.startSector, testEntry.entry.startSector);
    PRINT_DEBUG("%d, %d", entry->entry.isConfig, testEntry.entry.isConfig);
    PRINT_DEBUG("%d, %d", entry->entry.numberOfSectors, testEntry.entry.numberOfSectors);

    TEST_ASSERT_EQUAL(1, checkIfFilesystemEntryMatchesGivenEntry(&testEntry, entry));
}
void testFilesystemGetEntryByIdReturnsCorrectEntryWithValidID() {
    resetFilesystem();

    uint32_t sizeOfEntry = 1 * flashGetBytesPerSector(filesystemConfig.flash);
    uint32_t startSector = filesystemFindFittingStartSector(&filesystemConfig, sizeOfEntry);
    uint16_t numberOfRequiredSectors =
        ceil((double)sizeOfEntry / flashGetBytesPerSector(&flashConfig));

    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, sizeOfEntry, 1);
    fileSystemEntry_t *entry = filesystemGetEntryByID(&filesystemConfig, START_FILE_ID);

    fileSystemEntry_t testEntry = {0};
    testEntry.entry.id = START_FILE_ID;
    testEntry.entry.size = sizeOfEntry;
    testEntry.entry.startSector = startSector;
    testEntry.entry.isConfig = 1;
    testEntry.entry.numberOfSectors = numberOfRequiredSectors;

    PRINT_DEBUG("%d, %d", entry->entry.id, testEntry.entry.id);
    PRINT_DEBUG("%d, %d", entry->entry.size, testEntry.entry.size);
    PRINT_DEBUG("%d, %d", entry->entry.startSector, testEntry.entry.startSector);
    PRINT_DEBUG("%d, %d", entry->entry.isConfig, testEntry.entry.isConfig);
    PRINT_DEBUG("%d, %d", entry->entry.numberOfSectors, testEntry.entry.numberOfSectors);

    TEST_ASSERT_EQUAL(1, checkIfFilesystemEntryMatchesGivenEntry(&testEntry, entry));
}

void testFilesystemGetEntryByIndexReturnsNullWithInvalidIndex() {
    resetFilesystem();

    uint32_t sizeOfEntry = 1 * flashGetBytesPerSector(filesystemConfig.flash);
    uint32_t startSector = filesystemFindFittingStartSector(&filesystemConfig, sizeOfEntry);

    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, sizeOfEntry, 1);

    TEST_ASSERT_EQUAL(NULL, filesystemGetEntryByIndex(&filesystemConfig, 0 + 1));
}
void testFilesystemGetEntryBySectorReturnsNullWithInvalidIndex() {
    resetFilesystem();

    uint32_t sizeOfEntry = 1 * flashGetBytesPerSector(filesystemConfig.flash);
    uint32_t startSector = filesystemFindFittingStartSector(&filesystemConfig, sizeOfEntry);

    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, sizeOfEntry, 1);

    TEST_ASSERT_EQUAL(NULL, filesystemGetEntryBySector(&filesystemConfig, startSector + 1));
}
void testFilesystemGetEntryByIdReturnsNullWithInvalidIndex() {
    resetFilesystem();

    uint32_t sizeOfEntry = 1 * flashGetBytesPerSector(filesystemConfig.flash);
    uint32_t startSector = filesystemFindFittingStartSector(&filesystemConfig, sizeOfEntry);

    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, sizeOfEntry, 1);

    TEST_ASSERT_EQUAL(NULL, filesystemGetEntryByID(&filesystemConfig, START_FILE_ID + 1));
}

void testCorrectNumberOfBlockedSectors() {
    resetFilesystem();

    uint16_t startSector = 2;
    uint8_t numberOfSectors = 5;
    uint8_t countBlockedSectors = 0;
    uint32_t numberOfRequiredBytes =
        numberOfSectors * flashGetBytesPerSector(filesystemConfig.flash);
    filesystemBlockBytesForFPGA(&filesystemConfig, startSector, numberOfRequiredBytes);

    for (int i = 0; i < numberOfSectors; i++) {
        if (filesystemConfig.sectorFree[startSector + i] == BLOCKED_FOR_FPGA) {
            ++countBlockedSectors;
        }
    }

    TEST_ASSERT_EQUAL(numberOfSectors, countBlockedSectors);
}
void testBlockSectorsFailsIfSectorIsOccupied() {
    resetFilesystem();

    int16_t startSector = 2;

    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, 1, true);

    uint8_t numberOfSectorsToBlock = 1;
    uint32_t numberOfRequiredBytes =
        numberOfSectorsToBlock * flashGetBytesPerSector(filesystemConfig.flash);

    TEST_ASSERT_EQUAL(
        false, filesystemBlockBytesForFPGA(&filesystemConfig, startSector, numberOfRequiredBytes));
}
void testCorrectNumberOfBlockedSectorsAfterFree() {
    resetFilesystem();

    uint16_t startSector = 0;
    uint16_t numberOfSectors = 2;
    uint32_t numberOfRequiredBytes =
        numberOfSectors * flashGetBytesPerSector(filesystemConfig.flash);
    filesystemBlockBytesForFPGA(&filesystemConfig, startSector, numberOfRequiredBytes);
    filesystemFreeBlockedFPGASectors(&filesystemConfig);

    TEST_ASSERT_EQUAL(0, filesystemConfig.numberOfBlockedSectors);
}

void testStartSectorUpdatedCorrectlyAfterMovingFile() {
    resetFilesystem();

    uint8_t numberOfRequiredSectors = 1;
    uint32_t fileSize = numberOfRequiredSectors * flashGetBytesPerSector(filesystemConfig.flash);
    fileSystemEntry_t *entry =
        filesystemAddNewFileSystemEntry(&filesystemConfig, 0, fileSize, true);

    uint16_t oldStartSector = entry->entry.startSector;
    uint16_t newStartSector = oldStartSector + 1;

    fileSystemEntry_t expectedEntry = *entry;
    expectedEntry.entry.startSector = newStartSector;

    filesystemMoveFileToSector(&filesystemConfig, entry->entry.id, newStartSector);

    TEST_ASSERT_EQUAL(true, checkIfFilesystemEntryMatchesGivenEntry(&expectedEntry, entry));
}
void testMovingFileFailsIfSectorIsBlockedForFPGA() {
    resetFilesystem();

    uint16_t blockedSector = 1;
    uint8_t numberOfRequiredSectors = 1;
    uint32_t fileSize = numberOfRequiredSectors * flashGetBytesPerSector(filesystemConfig.flash);
    fileSystemEntry_t *entry =
        filesystemAddNewFileSystemEntry(&filesystemConfig, 0, fileSize, true);

    filesystemBlockBytesForFPGA(&filesystemConfig, blockedSector, numberOfRequiredSectors);

    TEST_ASSERT_EQUAL(
        false, filesystemMoveFileToSector(&filesystemConfig, entry->entry.id, blockedSector));
};

// Setup
void init() {
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();

    stdio_init_all();
    while (!stdio_usb_connected()) {}

    flashInit(&flashConfig);
    filesystemInit(&flashConfig, &filesystemConfig);
}
void setUp() {}
void tearDown(){};
void deInit() {
    filesystemEraseAllEntries(&filesystemConfig);
    rom_reset_usb_boot(0, 0);
}

int main() {
    init();
    UNITY_BEGIN();

    RUN_TEST(testFileSystemExistsAfterAddingNewEntry);
    RUN_TEST(testFileSystemDoesntExistAfterErasingAllEntries);

    RUN_TEST(testFilesystemValuesCorrectAfterNewEntry);
    RUN_TEST(testFilesystemValuesCorrectAfterDeletingEntry);

    RUN_TEST(testFilesystemGetEntryByIndexReturnsCorrectEntryWithValidIndex);
    RUN_TEST(testFilesystemGetEntryBySectorReturnsCorrectEntryWithValidSector);
    RUN_TEST(testFilesystemGetEntryByIdReturnsCorrectEntryWithValidID);

    RUN_TEST(testFilesystemGetEntryByIndexReturnsNullWithInvalidIndex);
    RUN_TEST(testFilesystemGetEntryByIndexReturnsNullWithInvalidIndex);
    RUN_TEST(testFilesystemGetEntryByIdReturnsNullWithInvalidIndex);

    RUN_TEST(testCorrectNumberOfBlockedSectors);
    RUN_TEST(testCorrectNumberOfBlockedSectorsAfterFree);
    RUN_TEST(testBlockSectorsFailsIfSectorIsOccupied);

    RUN_TEST(testStartSectorUpdatedCorrectlyAfterMovingFile);
    RUN_TEST(testMovingFileFailsIfSectorIsBlockedForFPGA);

    UNITY_END();
    deInit();
}
