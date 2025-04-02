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

#define paramTest(fn, param)                                                                       \
    void fn##param(void) {                                                                         \
        fn(param);                                                                                 \
    }

static size_t entrySize = sizeof(fileSystemEntry_t);

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

void init() {
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();

    stdio_init_all();
    while (!stdio_usb_connected()) {}

    flashInit(&flashConfig);
    filesystemInit(&flashConfig, &filesystemConfig);
    filesystemEraseAllEntries(&filesystemConfig);
}

void testFileSystemExists() {
    bool filesystemExists = false;
    uint8_t data[entrySize];
    data_t entryBuffer = {.data = data, .length = entrySize};

    for (int i = filesystemConfig.filesystemStartSector; i <= filesystemConfig.filesystemEndSector;
         i++) {
        uint32_t sectorStartAddress = i * flashGetBytesPerSector(filesystemConfig.flash);
        flashReadData(filesystemConfig.flash, sectorStartAddress, &entryBuffer);

        if (entryBuffer.data[0] != 255) {
            filesystemExists = true;
            break;
        }
    }
    TEST_ASSERT_EQUAL(true, filesystemExists);
}

void testFileSystemDoesntExist() {
    bool filesystemExists = false;
    uint8_t data[entrySize];
    data_t entryBuffer = {.data = data, .length = entrySize};

    for (int i = filesystemConfig.filesystemStartSector; i <= filesystemConfig.filesystemEndSector;
         i++) {
        uint32_t sectorStartAddress = i * flashGetBytesPerSector(filesystemConfig.flash);
        flashReadData(filesystemConfig.flash, sectorStartAddress, &entryBuffer);

        if (entryBuffer.data[0] != 255) {
            filesystemExists = true;
            break;
        }
    }
    TEST_ASSERT_EQUAL(false, filesystemExists);
}

void testFindFittingStartSector() {
    // test behaviour if file is too large (sectors between start and end of filesystem are blocked)
    uint32_t numberOfRequiredBytes =
        filesystemConfig.filesystemStartSector * flashGetBytesPerSector(filesystemConfig.flash);
    uint32_t startSector =
        filesystemFindFittingStartSector(&filesystemConfig, numberOfRequiredBytes);
    TEST_ASSERT_EQUAL(-1, startSector);

    // check, that all other sizes fit into an empty flash
    for (int i = 1; i < filesystemConfig.filesystemStartSector; i++) {
        numberOfRequiredBytes = i * flashGetBytesPerSector(filesystemConfig.flash);
        startSector = filesystemFindFittingStartSector(&filesystemConfig, numberOfRequiredBytes);
        TEST_ASSERT_NOT_EQUAL(-1, startSector);
    }

    // if sector 1 is occupied, startSector should not be zero (2 subsequent sectors needed)
    numberOfRequiredBytes = 2 * flashGetBytesPerSector(filesystemConfig.flash);
    filesystemConfig.sectorFree[1] = 0;
    startSector = filesystemFindFittingStartSector(&filesystemConfig, numberOfRequiredBytes);
    filesystemConfig.sectorFree[1] = 1;
    TEST_ASSERT_NOT_EQUAL(0, startSector);
}

void testCorrectEntryAfterNewFile() {
    // writes mock data to sector
    uint32_t sizeOfEntry = 1 * flashGetBytesPerSector(filesystemConfig.flash);
    uint8_t testData[10] = {1};
    uint32_t startSector = filesystemFindFittingStartSector(&filesystemConfig, sizeOfEntry);
    uint16_t numberOfRequiredPages = sizeOfEntry / flashGetBytesPerPage(&flashConfig);
    uint16_t numberOfRequiredSectors =
        ceil((double)sizeOfEntry / flashGetBytesPerSector(&flashConfig));

    // mock data should always fit
    TEST_ASSERT_NOT_EQUAL(-1, startSector);

    for (int i = 0; i < numberOfRequiredPages; i++) {
        flashWritePage(&flashConfig, startSector + i * flashGetBytesPerPage(&flashConfig), testData,
                       sizeof(testData));
    }

    filesystemAddNewFileSystemEntry(&filesystemConfig, startSector, sizeOfEntry, 1);
    fileSystemEntry_t *entry = filesystemGetEntryByIndex(&filesystemConfig, startSector);

    // checks if new entry with correct values is created
    TEST_ASSERT_EQUAL(100, entry->entry.id);
    TEST_ASSERT_EQUAL(sizeOfEntry, entry->entry.size);
    TEST_ASSERT_EQUAL(1, entry->entry.isConfig);
    TEST_ASSERT_EQUAL(startSector, entry->entry.startSector);
    TEST_ASSERT_EQUAL(numberOfRequiredSectors, entry->entry.numberOfSectors);

    // checks if filesystemGet... functions dont return null with correct values
    TEST_ASSERT_NOT_EQUAL(NULL, filesystemGetEntryByIndex(&filesystemConfig, 0));
    TEST_ASSERT_NOT_EQUAL(NULL, filesystemGetEntryBySector(&filesystemConfig, startSector));
    TEST_ASSERT_NOT_EQUAL(NULL,
                          filesystemGetEntryByID(&filesystemConfig, filesystemConfig.fileID - 1));

    // checks if wrong values lead to null return
    TEST_ASSERT_EQUAL(NULL, filesystemGetEntryByIndex(&filesystemConfig, 0 + 1));
    TEST_ASSERT_EQUAL(NULL, filesystemGetEntryBySector(&filesystemConfig, startSector + 1));
    TEST_ASSERT_EQUAL(NULL, filesystemGetEntryByID(&filesystemConfig, filesystemConfig.fileID));

    // checks if filesystem values are updated accordingly
    TEST_ASSERT_EQUAL(1, filesystemConfig.numberOfEntries);
    TEST_ASSERT_EQUAL(101, filesystemConfig.fileID);
    TEST_ASSERT_EQUAL(1020, filesystemConfig.nextFileSystemSector);
}

void testMovingFile(uint16_t newSector) {
    // move most recent file to given sector
    fileSystemEntry_t *oldEntry =
        filesystemGetEntryByID(&filesystemConfig, filesystemConfig.fileID - 1);
    uint16_t oldStartSector = oldEntry->entry.startSector;
    bool moveSuccessful =
        filesystemMoveFileToSector(&filesystemConfig, filesystemConfig.fileID - 1, newSector);
    fileSystemEntry_t *newEntry =
        filesystemGetEntryByID(&filesystemConfig, filesystemConfig.fileID - 1);

    // moving to empty sector should succeed
    TEST_ASSERT_EQUAL(true, moveSuccessful);

    // these values should not change by moving a file
    TEST_ASSERT_EQUAL(oldEntry->entry.id, newEntry->entry.id);
    TEST_ASSERT_EQUAL(oldEntry->entry.size, newEntry->entry.size);
    TEST_ASSERT_EQUAL(oldEntry->entry.isConfig, newEntry->entry.isConfig);
    TEST_ASSERT_EQUAL(oldEntry->entry.numberOfSectors, newEntry->entry.numberOfSectors);

    // this should change
    TEST_ASSERT_NOT_EQUAL(oldStartSector, newEntry->entry.startSector);

    // to given sector
    TEST_ASSERT_EQUAL(newSector, newEntry->entry.startSector);

    // moving a file to protected sector (i.e. filesystem sector) should always fail
    moveSuccessful = filesystemMoveFileToSector(&filesystemConfig, filesystemConfig.fileID - 1,
                                                filesystemConfig.filesystemStartSector);
    TEST_ASSERT_EQUAL(false, moveSuccessful);
}

void testCorrectEntryAfterDelete(uint16_t id) {
    fileSystemEntry_t *entry = filesystemGetEntryByID(&filesystemConfig, id - 1);
    filesystemEraseFileByID(&filesystemConfig, filesystemConfig.fileID - 1);

    // test that number of entries is updated
    TEST_ASSERT_EQUAL(0, filesystemConfig.numberOfEntries);

    // check that all Get... functions behave accordingly when passed non-existent id
    TEST_ASSERT_EQUAL(NULL, filesystemGetEntryByIndex(&filesystemConfig, 0));
    TEST_ASSERT_EQUAL(NULL,
                      filesystemGetEntryBySector(&filesystemConfig, entry->entry.startSector));
    TEST_ASSERT_EQUAL(NULL, filesystemGetEntryByID(&filesystemConfig, entry->entry.id));

    // make sure that all sectors of deleted file are cleared
    for (int i = 0; i < entry->entry.numberOfSectors; i++) {
        TEST_ASSERT_EQUAL(1, filesystemConfig.sectorFree[entry->entry.startSector + i]);
    }
}

void testCorrectNumberOfFreeSectors(int expectedNumberOfFreeSectors) {
    TEST_ASSERT_EQUAL(expectedNumberOfFreeSectors, filesystemConfig.numberOfFreeSectors);
}

void testCorrectNumberOfBlockedSectors(uint16_t startSector) {
    // block 2 sectors
    uint32_t numberOfRequiredBytes = flashGetBytesPerSector(filesystemConfig.flash);
    bool blockSuccessful =
        filesystemBlockBytesForFPGA(&filesystemConfig, startSector, numberOfRequiredBytes);

    TEST_ASSERT_EQUAL(true, blockSuccessful);
}

void testBlockSectorsFailsIfSectorIsOccupied(uint16_t startSector) {
    // block 2 sectors
    uint32_t numberOfRequiredBytes = flashGetBytesPerSector(filesystemConfig.flash);
    bool blockSuccessful =
        filesystemBlockBytesForFPGA(&filesystemConfig, startSector, numberOfRequiredBytes);

    TEST_ASSERT_EQUAL(false, blockSuccessful);
}

void testCorrectNumberOfBlockedSectorsAfterFree() {
    filesystemFreeBlockedFPGASectors(&filesystemConfig);
    TEST_ASSERT_EQUAL(0, filesystemConfig.numberOfBlockedSectors);
}

paramTest(testMovingFile, 100)

    paramTest(testCorrectNumberOfFreeSectors, 1018) paramTest(testCorrectNumberOfFreeSectors, 1019)

        paramTest(testCorrectNumberOfBlockedSectors, 0)
            paramTest(testBlockSectorsFailsIfSectorIsOccupied, 0)

                void setUp() {}
void tearDown(){};

void deInit() {
    filesystemEraseAllEntries(&filesystemConfig);
    rom_reset_usb_boot(0, 0);
}

int main() {
    init();
    UNITY_BEGIN();

    // make sure no filesystem exists after erasing filesystem (inside init)
    RUN_TEST(testFileSystemDoesntExist);

    // test behaviour of function that finds a fitting startSector, if possible
    RUN_TEST(testFindFittingStartSector);

    // test behaviour if file is added
    RUN_TEST(testCorrectEntryAfterNewFile);
    RUN_TEST(testCorrectNumberOfFreeSectors1018);
    RUN_TEST(testFileSystemExists);
    RUN_TEST(testBlockSectorsFailsIfSectorIsOccupied0);
    RUN_TEST(testMovingFile100);

    // test behaviour after file is deleted
    RUN_TEST(testCorrectEntryAfterDelete);
    RUN_TEST(testFileSystemDoesntExist);
    RUN_TEST(testCorrectNumberOfFreeSectors1019);

    // test behaviour of block sector for fpga
    RUN_TEST(testCorrectNumberOfBlockedSectors0);
    RUN_TEST(testFileSystemExists);
    RUN_TEST(testCorrectNumberOfFreeSectors1018);

    // test behaviour of free blocked sectors
    RUN_TEST(testCorrectNumberOfBlockedSectorsAfterFree);

    // check that no filesystem exists after all entries are deleted and all blocked sectors are
    // free
    RUN_TEST(testFileSystemDoesntExist);

    UNITY_END();
    deInit();
}
