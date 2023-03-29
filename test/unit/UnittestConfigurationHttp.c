#include "FpgaConfigurationHttp.h"
#include "FlashTypedefs.h"
#include "flash/Flash.h"
#include <unity.h>
#include <stdlib.h>
#include <string.h>


#define NUMSECTORS 2
const uint8_t numberOfPages = 5;
const uint16_t configSize = FLASH_PAGE_SIZE * numberOfPages;
const uint8_t numberOfSectors = NUMSECTORS;

void setUp(void) {
    numWriteBlocks = 0;
    addressSectorErase = 0;
    numSectorErase = 0;
    for (uint16_t i = 0; i < numberOfSectors; i++) {
        for (uint32_t j = 0; j < FLASH_SECTOR_SIZE; j++) {
            dataComplete[(j + i * FLASH_PAGE_SIZE)] = 0;
        }
    }
}

void tearDown(void) {}

void writeExpectedAddresses(uint32_t *expectedAddresses, uint32_t startingAddress, uint8_t
                                                                                       numberOfPages) {
    for (uint16_t i = 0; i < numberOfPages; i++) {
        expectedAddresses[i] = startingAddress + (FLASH_PAGE_SIZE * i);
    }
}
void testWriteData(){
    uint32_t startAddress=0;
    uint32_t sizeOfConfiguration=numberOfPages*FLASH_PAGE_SIZE;
    uint8_t expected[sizeOfConfiguration];
    for(uint16_t j=0;j<numberOfPages; j++) {
        for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++) {
            expected[i+j*FLASH_PAGE_SIZE] = i;
        }
    }

    printf("Hallo?\n");
    setCommunication(dataReceive);
    configErrorCode_t status=configure(startAddress,sizeOfConfiguration);
    printf("\n%u\n",dataComplete[1]);
    
    
    uint32_t expectedAddresses[numberOfPages];
    writeExpectedAddresses(expectedAddresses, startAddress, numberOfPages);
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expectedAddresses, addressWrite, numberOfPages,
                                           "Addresses of flash pages");
    
    
   // TEST_ASSERT_EQUAL_UINT8(CONFIG_NO_ERROR, status);
   // TEST_ASSERT_EQUAL_UINT32(1, numSectorErase);
   // TEST_ASSERT_EQUAL_UINT32(numberOfPages, numWriteBlocks);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, (&dataComplete[0] + startAddress),
                                                     sizeOfConfiguration,
                                          "Content of written data");
}

HttpResponse_t* dataReceive(uint32_t address){
    uint8_t data[256];
    HttpResponse_t * block = malloc(sizeof(HttpResponse_t));
    block->length = 256;
    block->response = malloc(sizeof(uint8_t) * 256);
 
    for(uint16_t i=0; i<256; i++){
        data[i]=i;
    }
    memcpy(block->response, data, 256);
    return block;
}
int main(void) {
    UNITY_BEGIN();
    printf("haaaalllo=???\n‚");
    RUN_TEST(testWriteData);
  
    return UNITY_END();
}