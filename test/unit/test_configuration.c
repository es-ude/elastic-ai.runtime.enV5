//
// Created by Natalie Maman on 01.06.22.
//
#define TEST_BUILD


#include "unity.h"
#include "dummies/flash/config_data_handler.h"
#include "dummies/flash/flash.h"
#include "stdlib.h"
#include "fpga_config/configuration.h"

#include<stdbool.h>
uint8_t expectedData[256*4];
void setUp() {
    for (uint16_t i = 0; i < 4; i++) {
        for (uint16_t j = 0; j < 256; j++) {
            expectedData[(j + i * 256)] = j;
        }
    }
}



uint16_t readData2(uint8_t *block, uint16_t buffer_length) {
    uint16_t buffer_index = 0;
    while (true) {
        if ( buffer_index < buffer_length) {
            block[buffer_index++] = 0xFF;
        } else {
            break;
        }
    }
    return buffer_index;
}
void readValue2(uint32_t *destination) {
    readData2((uint8_t *) destination, sizeof(uint32_t));

}

void test_readValue(){
    uint32_t result;
    readValue2(&result);
    TEST_ASSERT_EQUAL((uint32_t)0xFFFFFFFF, result);
}
void tearDown(){}
void test_write_data_to_flash_at_address_0(){
    uint32_t expectedAddresses[4];
    for(uint16_t i=0; i<4; i++) {
        expectedAddresses[i]=(256*i);
    }
    setAddress(0);
    setConfigSize(256*4);
   configurationFlash();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0,addressBlockErase,"Number of Blocks erased");
   TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedData, dataComplete, (256*4), "Content of written data");
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(addressWrite, expectedAddresses ,4, "Addresses of flash pages" );
}



int main ( void )
{
    UNITY_BEGIN( );
    RUN_TEST( test_write_data_to_flash_at_address_0);
    RUN_TEST(test_readValue);
    return UNITY_END( );
}