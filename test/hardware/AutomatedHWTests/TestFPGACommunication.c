/*!
 *  IMPORTANT: This test assumes the echo skeleton is already stored in flash sector 0!
 *             If you run this test via the test script, this will be done by the script.
 */

#define SOURCE_FILE "TEST_FPGA_COMMUNICATION"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "stub.h"

#include "unity.h"

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

uint32_t echoSkeletonSector = 0;

// ID of the echo skeleton: 240823ECHOSERVER
uint8_t echoSkeletonId[] = {0x32, 0x34, 0x30, 0x38, 0x32, 0x33, 0x45, 0x43,
                            0x48, 0x4F, 0x53, 0x45, 0x52, 0x56, 0x45, 0x52};

uint8_t testData[] = {0xFF, 0xFE, 0xFD, 0xFC, 0x00, 0x01, 0x02, 0x03,
                      0xDC, 0xDD, 0x1F, 0xFC, 0xFF, 0xFE, 0xFD, 0xFC};

spiConfiguration_t spiToFlash = {.spiInstance = FLASH_SPI_MODULE,
                                 .baudrate = FLASH_SPI_BAUDRATE,
                                 .sckPin = FLASH_SPI_CLOCK,
                                 .misoPin = FLASH_SPI_MISO,
                                 .mosiPin = FLASH_SPI_MOSI,
                                 .csPin = FLASH_SPI_CS};

flashConfiguration_t flashConfig = {.spiConfiguration = &spiToFlash};

static void initialize(void) {
    stdio_init_all();
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();
    flashInit(&flashConfig);

    while (!stdio_usb_connected()) {}
}

void setUp() {}
void tearDown() {}
void deInit() {
    env5HwControllerFpgaPowersOff();
    rom_reset_usb_boot(0, 0);
}

void testDeployReturnsTrueWithValidAddressAndID() {
    env5HwControllerFpgaPowersOn();
    bool returnValue =
        modelDeploy(echoSkeletonSector * flashGetBytesPerSector(&flashConfig), echoSkeletonId);
    TEST_ASSERT_EQUAL(true, returnValue);
    env5HwControllerFpgaPowersOff();
}

void testDeployReturnsFalseWithInvalidAcceleratorId() {
    env5HwControllerFpgaPowersOn();
    bool returnValue =
        modelDeploy(echoSkeletonSector * flashGetBytesPerSector(&flashConfig), echoSkeletonId + 1);
    TEST_ASSERT_EQUAL(false, returnValue);
    env5HwControllerFpgaPowersOff();
}

void testDeployReturnsFalseWithInvalidAcceleratorAddress() {
    env5HwControllerFpgaPowersOn();
    bool returnValue = modelDeploy((echoSkeletonSector + 1) * flashGetBytesPerSector(&flashConfig),
                                   echoSkeletonId + 1);
    TEST_ASSERT_EQUAL(false, returnValue);
    env5HwControllerFpgaPowersOff();
}

void testGetIDFromSkeletonReturnsCorrectIdAfterDeploy() {
    env5HwControllerFpgaPowersOn();
    modelDeploy(echoSkeletonSector * flashGetBytesPerSector(&flashConfig), echoSkeletonId);
    uint8_t skeletonId[16] = {0};
    modelGetId(skeletonId);
    bool flagEqual = true;
    for (int i = 0; i < 16; i++) {
        if (echoSkeletonId[i] != skeletonId[i]) {
            PRINT("%0x", skeletonId[i]);
            flagEqual = false;
        }
    }
    TEST_ASSERT_EQUAL(true, flagEqual);
    env5HwControllerFpgaPowersOff();
}

void testEchoMatchesGivenTestData() {
    env5HwControllerFpgaPowersOn();
    modelDeploy(echoSkeletonSector * flashGetBytesPerSector(&flashConfig), echoSkeletonId);
    uint8_t result[16] = {0};
    modelPredict(testData, sizeof(testData), result, sizeof(result));

    bool flagEqual = true;
    for (int i = 0; i < sizeof(result); i++) {
        if (result[i] != testData[i]) {
            flagEqual = false;
            PRINT("MISMATCH");
        }
    }
    TEST_ASSERT_EQUAL(true, flagEqual);
    env5HwControllerFpgaPowersOff();
}

void testEchoDoesntMatchManipulatedTestData() {
    env5HwControllerFpgaPowersOn();
    modelDeploy(echoSkeletonSector * flashGetBytesPerSector(&flashConfig), echoSkeletonId);
    uint8_t result[16] = {0};
    modelPredict(testData, sizeof(testData), result, sizeof(result));

    bool flagEqual = true;
    for (int i = 0; i < sizeof(result); i++) {
        if (result[i] != testData[i] + 1) {
            flagEqual = false;
        }
    }
    TEST_ASSERT_EQUAL(false, flagEqual);
    env5HwControllerFpgaPowersOff();
}

int main(void) {
    initialize();
    UNITY_BEGIN();
    RUN_TEST(testDeployReturnsTrueWithValidAddressAndID);
    RUN_TEST(testDeployReturnsFalseWithInvalidAcceleratorId);
    RUN_TEST(testDeployReturnsFalseWithInvalidAcceleratorAddress);

    RUN_TEST(testGetIDFromSkeletonReturnsCorrectIdAfterDeploy);

    RUN_TEST(testEchoMatchesGivenTestData);
    RUN_TEST(testEchoDoesntMatchManipulatedTestData);

    UNITY_END();
    deInit();
}
