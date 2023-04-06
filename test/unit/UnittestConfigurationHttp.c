#include "FpgaConfigurationHttp.h"
#include "FlashTypedefs.h"
#include "flash/Flash.h"
#include <unity.h>
#include <stdlib.h>
#include <string.h>


#define NUMSECTORS 2
uint8_t numberOfPages;
const uint8_t numberOfSectors = NUMSECTORS;
const uint16_t http_receive_size=1024;
const uint32_t config_size=3000;

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
    uint8_t expected[config_size];
    for(uint32_t j=0;j<config_size; j++) {
        expected[j]=j%FLASH_PAGE_SIZE;
    }
    
    setCommunication(dataReceive);
    configErrorCode_t status=configure(startAddress,config_size);
    
    numberOfPages=config_size/FLASH_PAGE_SIZE;
    uint32_t expectedAddresses[numberOfPages];
    writeExpectedAddresses(expectedAddresses, startAddress, numberOfPages);
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expectedAddresses, addressWrite, numberOfPages,
                                           "Addresses of flash pages");
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, (&dataComplete[0] + startAddress),
                                                     config_size,
                                          "Content of written data");
}

HttpResponse_t* dataReceive(uint32_t blockNum){
    printf("num Block: %i\n", blockNum);
    uint32_t blockSize;
    if((config_size) / http_receive_size==blockNum){
        blockSize=config_size%http_receive_size;
        printf("size last block: %i\n", blockSize);
    
    }else{
        blockSize=http_receive_size;
    }
    uint8_t data[blockSize];
    HttpResponse_t * block = malloc(sizeof(HttpResponse_t));
    block->length =blockSize;
    block->response = malloc(sizeof(uint8_t) * blockSize);
 
    for(uint32_t i=0; i<blockSize; i++){
        data[i]=i%FLASH_PAGE_SIZE;
    }
    memcpy(block->response, data,blockSize);
    return block;
}
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testWriteData);
  
    return UNITY_END();
}