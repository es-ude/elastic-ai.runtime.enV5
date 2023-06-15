#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unity.h>
#include "Flash.h"

typedef struct write{
    uint8_t * block;
    uint16_t blockLength;
    uint32_t startAddress;
}Write_t;

void setUp(void){

}
void tearDown(void){

}
void flashWrite(Write_t *write){
//   flashWritePage()
}

void testWriteBlockOnTwoPages(void){
    uint16_t writeLength = 300;
    uint8_t* data = malloc(sizeof(uint8_t) * writeLength);
    memset(data, 'A', writeLength);
    
    Write_t block = {
        .blockLength = writeLength,
        .startAddress = 0,
        .block = data
    };
    flashWrite(&block);
    TEST_FAIL();
}



int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testWriteBlockOnTwoPages);
    
    return UNITY_END();
}