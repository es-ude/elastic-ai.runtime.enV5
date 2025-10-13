#include "Float16.h"
#include "Quantization.h"

#include <AiHelpers.h>
#include <stdlib.h>

uint8_t *castDataToFloat16(qTensor_t *inputQTensor) {
    size_t totalNumberOfElements = calcTotalNumberOfElementsByTensor(inputQTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputQTensor->quantization);

    quantization_t *float16Quantization = initQuantizationByType(FLOAT16_Q);
    floatQ_t *float16Config = float16Quantization->qConfig;
    size_t mantissaBits = float16Config->mantissaBits;
    size_t exponentBits = float16Config->exponentBits;
    size_t outputBytesPerElement = calcBytesPerElement(float16Quantization);
    uint8_t *outputData = calloc(totalNumberOfElements, outputBytesPerElement);

    for (size_t i = 0; i < totalNumberOfElements; i++) {
        size_t elementStartIndex = i * outputBytesPerElement;
        for (size_t j = 0; j < inputBytesPerElement; j++) {
            outputData[elementStartIndex + j] = (uint8_t)inputQTensor->data << 8 * j;
        }

    }
    return outputData;
}
