#include "Sub.h"
#include "Arithmetic.h"
#include "DTypes.h"
#include "TensorConversion.h"

#include <stdio.h>

#ifdef TRACK_INSTRUCTIONS
#define SUB_FUNC_INT subIntsWithInstructionCounter
#define SUB_FUNC_FLOAT subFloatsWithInstructionCounter
#else
#define SUB_FUNC_INT subInts
#define SUB_FUNC_FLOAT subFloats
#endif

size_t subInstructionCounter = 0;

int32_t subInts(int32_t a, int32_t b) {
    return a - b;
}

int32_t subIntsWithInstructionCounter(int32_t a, int32_t b) {
    ++subInstructionCounter;
    return a - b;
}

int32_t subInt32s(int32_t a, int32_t b) {
    return SUB_FUNC_INT(a, b);
}

float subFloats(float a, float b) {
    return a - b;
}

float subFloatsWithInstructionCounter(float a, float b) {
    ++subInstructionCounter;
    return a - b;
}

float subFloat32s(float a, float b) {
    return SUB_FUNC_FLOAT(a, b);
}

void subInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t sub = subInt32s;
    int32PointWiseArithmetic(a, b, sub, outputTensor);
}

void subInt32TensorsInplace(tensor_t *a, tensor_t *b) {
    int32ElementArithmeticFunc_t sub = subInt32s;
    int32PointWiseArithmeticInplace(a, b, sub);
}

void subInt32ElementWithTensor(tensor_t *a, int32_t b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t sub = subInt32s;
    int32ElementWithTensorArithmetic(a, b, sub, outputTensor);
}

void subInt32ElementWithTensorInplace(tensor_t *a, int32_t b) {
    int32ElementArithmeticFunc_t sub = subInt32s;
    int32ElementWithTensorArithmeticInplace(a, b, sub);
}

void subFloat32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t sub = subFloats;
    floatPointWiseArithmetic(a, b, sub, outputTensor);
}

void subFloat32TensorsInplace(tensor_t *a, tensor_t *b) {
    floatElementArithmeticFunc_t sub = subFloats;
    floatPointWiseArithmeticInplace(a, b, sub);
}

void subFloat32ElementWithTensor(tensor_t *a, float b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t sub = subFloats;
    floatElementWithTensorArithmetic(a, b, sub, outputTensor);
}

void subFloat32ElementWithTensorInplace(tensor_t *a, float b) {
    floatElementArithmeticFunc_t sub = subFloats;
    floatElementWithTensorArithmeticInplace(a, b, sub);
}

void subSymInt32Tensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(aTensor);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(bTensor);

    if(aNumberOfValues != bNumberOfValues) {
        printf("Error in subSymInt32Tensors: mismatched number of Values\n");
    }

    symInt32QConfig_t *aSymInt32QConfig = aTensor->quantization->qConfig;
    symInt32QConfig_t *bSymInt32QConfig = bTensor->quantization->qConfig;
    symQConfig_t *outputSymInt32QConfig = outputTensor->quantization->qConfig;

    float aScale = aSymInt32QConfig->scale;
    float bScale = bSymInt32QConfig->scale;

    if(aScale == bScale) {
        outputSymInt32QConfig->scale = aScale;

        int32_t aValues[aNumberOfValues];
        readBytesAsInt32Array(aNumberOfValues, aTensor->data, aValues);
        int32_t bValues[aNumberOfValues];
        readBytesAsInt32Array(aNumberOfValues, bTensor->data, bValues);
        int32_t outputValues[aNumberOfValues];

        for(size_t i = 0; i < aNumberOfValues; i++) {
            outputValues[i] = SUB_FUNC_INT(aValues[i], bValues[i]);
        }
        writeInt32ArrayToByteArray(aNumberOfValues, outputValues, outputTensor->data);
    }

    else {
        quantization_t floatQ;
        initFloat32Quantization(&floatQ);
        float aFloatData[aNumberOfValues];
        tensor_t aFloat;
        setTensorValuesForConversion(aFloatData, &floatQ, &aFloat, aTensor);
        convertTensor(aTensor, &aFloat);

        float bFloatData[aNumberOfValues];
        tensor_t bFloat;
        setTensorValuesForConversion(bFloatData, &floatQ, bTensor, &bFloat);
        convertTensor(bTensor, &bFloat);

        subFloatTensorsInplace(&aFloat, &bFloat);

        convertTensor(&aFloat, outputTensor);
    }
}

void subSymInt32TensorsInplace(tensor_t* aTensor, tensor_t* bTensor) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(aTensor);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(bTensor);

    if(aNumberOfValues != bNumberOfValues) {
        printf("Error in subSymInt32Tensors: mismatched number of Values\n");
    }

    symInt32QConfig_t *aSymInt32QConfig = aTensor->quantization->qConfig;
    symInt32QConfig_t *bSymInt32QConfig = bTensor->quantization->qConfig;

    float aScale = aSymInt32QConfig->scale;
    float bScale = bSymInt32QConfig->scale;

    if(aScale == bScale) {
        int32_t aValues[aNumberOfValues];
        readBytesAsInt32Array(aNumberOfValues, aTensor->data, aValues);
        int32_t bValues[aNumberOfValues];
        readBytesAsInt32Array(aNumberOfValues, bTensor->data, bValues);
        int32_t outputValues[aNumberOfValues];

        for(size_t i = 0; i < aNumberOfValues; i++) {
            outputValues[i] = SUB_FUNC_INT(aValues[i], bValues[i]);
        }
        writeInt32ArrayToByteArray(aNumberOfValues, outputValues, aTensor->data);
    }

    else {
        quantization_t floatQ;
        initFloat32Quantization(&floatQ);
        float aFloatData[aNumberOfValues];
        tensor_t aFloat;
        setTensorValuesForConversion(aFloatData, &floatQ, &aFloat, aTensor);
        convertTensor(aTensor, &aFloat);

        float bFloatData[aNumberOfValues];
        tensor_t bFloat;
        setTensorValuesForConversion(bFloatData, &floatQ, bTensor, &bFloat);
        convertTensor(bTensor, &bFloat);

        subFloatTensorsInplace(&aFloat, &bFloat);

        convertTensor(&aFloat, aTensor);
    }
}

size_t getSubInstructionCounter() {
    return subInstructionCounter;
}
