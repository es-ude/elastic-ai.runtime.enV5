#include "FpgaConfigurationHttp.h"
#include "FlashTypedefs.h"
#include "Flash.h"
#include <unity.h>

void setUp(void) {
}

void tearDown(void) {}


void testWriteData(){
    uint8_t expected[256];
    for (uint16_t i=0; i<256; i++){
        expected[i]=i;
    }
    setCommunication(dataReceive);
    uint8_t* block=configure();
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, block, 256, "data");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testWriteData);
  
    return UNITY_END();
}