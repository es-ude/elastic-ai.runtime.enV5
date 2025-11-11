#include "Add.h"
#include "Tensor.h"
#include "DTypes.h"
#include "Arithmetic.h"
#include "TensorConversion.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef TRACK_INSTRUCTIONS
#define ADD_FUNC_INT addIntsWithInstructionCounter
#define ADD_FUNC_FLOAT addFloatsWithInstructionCounter
#else
#define ADD_FUNC_INT addInts
#define ADD_FUNC_FLOAT addFloats
#endif

size_t addInstructionCounter = 0;

int32_t addInts(int32_t a, int32_t b) {
    return a + b;
}

int32_t addIntsWithInstructionCounter(int32_t a, int32_t b) {
    ++addInstructionCounter;
    return a + b;
}

int32_t addInt32s(int32_t a, int32_t b) {
    return ADD_FUNC_INT(a, b);
}

float addFloats(float a, float b) {
    return a + b;
}

float addFloatsWithInstructionCounter(float a, float b) {
    ++addInstructionCounter;
    return a + b;
}

float addFloat32s(float a, float b) {
    return ADD_FUNC_FLOAT(a, b);
}

void addInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    int32PointWiseArithmetic(a, b, ADD_FUNC_INT, outputTensor);
}

void addInt32TensorsInplace(tensor_t *a, tensor_t *b) {
    int32PointWiseArithmeticInplace(a, b, ADD_FUNC_INT);
}

void addInt32ElementWithTensor(tensor_t *a, int32_t b, tensor_t *outputTensor) {
    int32ElementWithTensorArithmetic(a, b, ADD_FUNC_INT, outputTensor);
}

void addInt32ElementWithTensorInplace(tensor_t *a, int32_t b) {
    int32ElementWithTensorArithmeticInplace(a, b, ADD_FUNC_INT);
}

void addFloat32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    floatPointWiseArithmetic(a, b, ADD_FUNC_FLOAT, outputTensor);
}

void addFloat32TensorsInplace(tensor_t *a, tensor_t *b) {
    floatPointWiseArithmeticInplace(a, b, ADD_FUNC_FLOAT);
}

void addFloat32ElementWithTensor(tensor_t *a, float b, tensor_t *outputTensor) {
    floatElementWithTensorArithmetic(a, b, ADD_FUNC_FLOAT, outputTensor);
}

void addFloat32ElementWithTensorInplace(tensor_t *a, float b) {
    floatElementWithTensorArithmeticInplace(a, b, ADD_FUNC_FLOAT);
}

// Important: Scale is ignored!
void addInt32TensorToSymInt32TensorInplace(tensor_t *symInt32Tensor, tensor_t *int32Tensor) {
    size_t symInt32NumberOfValues = calcNumberOfElementsByTensor(symInt32Tensor);
    size_t int32NumberOfValues = calcNumberOfElementsByTensor(int32Tensor);

    if (symInt32NumberOfValues != int32NumberOfValues) {
        printf("Error in add int to symInt: mismatched number of elements\n");
    }

    int32_t intValues[symInt32NumberOfValues];
    readBytesAsInt32Array(symInt32NumberOfValues, int32Tensor->data, intValues);

    int32_t symValues[symInt32NumberOfValues];
    readBytesAsInt32Array(symInt32NumberOfValues, symInt32Tensor->data, symValues);

    int32_t outputValues[symInt32NumberOfValues];

    for (size_t i = 0; i < int32NumberOfValues; i++) {
        outputValues[i] = ADD_FUNC_INT(intValues[i], symValues[i]);
    }

    writeInt32ArrayToByteArray(symInt32NumberOfValues, outputValues, symInt32Tensor->data);
}

// Important: Scale is ignored!
void addFloat32TensorToSymInt32TensorInplace(tensor_t *symInt32Tensor, tensor_t *float32Tensor) {
    size_t symInt32NumberOfValues = calcNumberOfElementsByTensor(symInt32Tensor);
    size_t int32NumberOfValues = calcNumberOfElementsByTensor(float32Tensor);

    if (symInt32NumberOfValues != int32NumberOfValues) {
        printf("Error in add int to symInt: mismatched number of elements\n");
    }

    float floatValues[symInt32NumberOfValues];
    readBytesAsFloatArray(symInt32NumberOfValues, float32Tensor->data, floatValues);

    int32_t symValues[symInt32NumberOfValues];
    readBytesAsInt32Array(symInt32NumberOfValues, symInt32Tensor->data, symValues);

    int32_t outputValues[symInt32NumberOfValues];

    for (size_t i = 0; i < int32NumberOfValues; i++) {
        outputValues[i] = ADD_FUNC_INT((int32_t)floatValues[i], symValues[i]);
    }

    writeInt32ArrayToByteArray(symInt32NumberOfValues, outputValues, symInt32Tensor->data);
}

void addSymInt32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(aTensor);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(bTensor);

    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in addSymInt32Tensors: mismatched number of values\n");
    }

    symInt32QConfig_t *aSymInt32QConfig = aTensor->quantization->qConfig;
    symInt32QConfig_t *bSymInt32QConfig = bTensor->quantization->qConfig;
    symInt32QConfig_t *outputSymInt32QConfig = outputTensor->quantization->qConfig;

    float aScale = aSymInt32QConfig->scale;
    float bScale = bSymInt32QConfig->scale;

    if (aScale == bScale) {
        outputSymInt32QConfig->scale = aScale;

        int32_t aValues[aNumberOfValues];
        readBytesAsInt32Array(aNumberOfValues, aTensor->data, aValues);
        int32_t bValues[aNumberOfValues];
        readBytesAsInt32Array(aNumberOfValues, bTensor->data, bValues);

        int32_t outputValues[aNumberOfValues];;

        for (size_t i = 0; i < aNumberOfValues; i++) {
            outputValues[i] = ADD_FUNC_INT(aValues[i], bValues[i]);
        }
        writeInt32ArrayToByteArray(aNumberOfValues, outputValues, outputTensor->data);
    } else {
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

        addFloat32TensorsInplace(&aFloat, &bFloat);

        convertTensor(&aFloat, outputTensor);
    }
}

void addSymInt32TensorsInplace(tensor_t *aTensor, tensor_t *bTensor) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(aTensor);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(bTensor);

    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in addSymInt32Tensors: mismatched number of values\n");
    }

    symInt32QConfig_t *aSymInt32QConfig = aTensor->quantization->qConfig;
    symInt32QConfig_t *bSymInt32QConfig = bTensor->quantization->qConfig;

    float aScale = aSymInt32QConfig->scale;
    float bScale = bSymInt32QConfig->scale;

    if (aScale == bScale) {
        int32_t aValues[aNumberOfValues];
        readBytesAsInt32Array(aNumberOfValues, aTensor->data, aValues);
        int32_t bValues[aNumberOfValues];
        readBytesAsInt32Array(aNumberOfValues, bTensor->data, bValues);

        int32_t outputValues[aNumberOfValues];;

        for (size_t i = 0; i < aNumberOfValues; i++) {
            outputValues[i] = addInt32s(aValues[i], bValues[i]);
        }
        writeInt32ArrayToByteArray(aNumberOfValues, outputValues, aTensor->data);
    } else {
        quantization_t floatQ;
        initFloat32Quantization(&floatQ);
        float aFloatData[aNumberOfValues];
        tensor_t aFloat;

        setTensorValuesForConversion(aFloatData, &floatQ, aTensor, &aFloat);
        convertTensor(aTensor, &aFloat);

        float bFloatData[aNumberOfValues];
        tensor_t bFloat;
        setTensorValuesForConversion(bFloatData, &floatQ, bTensor, &bFloat);
        convertTensor(bTensor, &bFloat);

        addFloat32TensorsInplace(&aFloat, &bFloat);

        convertTensor(&aFloat, aTensor);
    }
}

size_t getAddInstructionCounter() {
    return addInstructionCounter;
}
