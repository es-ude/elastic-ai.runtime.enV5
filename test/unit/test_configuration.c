//
// Created by Natalie Maman on 01.06.22.
//
#define TEST_BUILD


#include "unity.h"
#include "dummies/flash/config_data_handler.h"
#include "dummies/flash/flash.h"
#include "fpga_config/configuration.h"
uint8_t expectedData[256*4];
void setUp(){
    for(uint16_t i=0; i<4; i++) {
        for (uint16_t j = 0; j < 256; j++) {
            expectedData[(j +i*256)]=j;
        }
    }


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
    return UNITY_END( );
}