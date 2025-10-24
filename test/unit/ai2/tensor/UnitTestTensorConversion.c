//
// Created by Leo Buron on 27.10.25.
//

#include "DTypes.h"
#include "Quantization.h"
#include "Tensor.h"
#include "TensorConversion.h"
#include "unity.h"

#include <stddef.h>
#include <stdlib.h>


void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();
    //RUN_TEST(testConversionFloatLinear);
    RUN_TEST(testByteFlattening);
    RUN_TEST(testByteFlattening2);
    RUN_TEST(testByteFlattening3);
    RUN_TEST(testGetBitmask);
    RUN_TEST(testGetBitmask2);
    RUN_TEST(testWriteByte);
    RUN_TEST(testWriteByte2);
    RUN_TEST(testReadByte);
    //RUN_TEST(testConversionLinearFloat);
    UNITY_END();
}
