#include "MSE.h"
#include "Tensor.h"
#include "TensorConversion.h"
#include "Sub.h"

#include <Mul.h>
#include <stdio.h>

void MSELossBackwardFloat(tensor_t *modelOutput, tensor_t *label, tensor_t *result) {
    size_t numberOfElements = calcNumberOfElementsByTensor(modelOutput);

    float mean = 2.f / (float)numberOfElements;

    float *modelOutputArray = (float *)modelOutput->data;
    float *labelArray = (float *)label->data;
    float *resultArray = (float *)result->data;

    for (size_t i = 0; i < numberOfElements; i++) {
        resultArray[i] = mulFloat32s(mean, subFloat32s(modelOutputArray[i], labelArray[i]));
    }
}

void MSELossBackwardAsym(tensor_t *modelOutput, tensor_t *label, tensor_t *result) {
    size_t numberOfElements = calcNumberOfElementsByTensor(modelOutput);

    tensor_t modelOutputSymInt32;
    asymQConfig_t *modelOutputAsymQC = modelOutput->quantization->qConfig;
    symInt32QConfig_t modelOutputSymInt32QC;
    initSymInt32QConfig(modelOutputAsymQC->roundingMode, &modelOutputSymInt32QC);
    quantization_t modelOutputSymInt32Q;
    initSymInt32Quantization(&modelOutputSymInt32QC, &modelOutputSymInt32Q);
    int32_t modelOutputSymInt32Data[numberOfElements];
    setTensorValuesForConversion(modelOutputSymInt32Data, &modelOutputSymInt32Q, modelOutput, &modelOutputSymInt32);
    convertTensor(modelOutput, &modelOutputSymInt32);

    tensor_t labelSymInt32;
    asymQConfig_t *labelAsymQC = label->quantization->qConfig;
    symInt32QConfig_t labelSymInt32QC;
    initSymInt32QConfig(labelAsymQC->roundingMode, &labelSymInt32QC);
    quantization_t labelSymInt32Q;
    initSymInt32Quantization(&labelSymInt32QC, &labelSymInt32Q);
    int32_t labelSymInt32Data[numberOfElements];
    setTensorValuesForConversion(labelSymInt32Data, &labelSymInt32Q, label, &labelSymInt32);
    convertTensor(label, &labelSymInt32);

    tensor_t resultSymInt32;
    asymQConfig_t *resultAsymQC = result->quantization->qConfig;
    symInt32QConfig_t resultSymInt32QC;
    initSymInt32QConfig(resultAsymQC->roundingMode, &resultSymInt32QC);
    quantization_t resultSymInt32Q;
    initSymInt32Quantization(&resultSymInt32QC, &resultSymInt32Q);
    int32_t resultSymInt32Data[numberOfElements];
    setTensorValuesForConversion(resultSymInt32Data, &resultSymInt32Q, result, &resultSymInt32);
    convertTensor(result, &resultSymInt32);


    int32_t *modelOutputArray = (int32_t *)modelOutputSymInt32.data;
    int32_t *labelArray = (int32_t *)labelSymInt32.data;
    int32_t *resultArray = (int32_t *)resultSymInt32.data;


    for (size_t i = 0; i < numberOfElements; i++) {
        resultArray[i] = subInt32s(modelOutputArray[i], labelArray[i]);
    }

    float mean = 2.f / (float)numberOfElements;
    // TODO Leo fragen, ob scale so sinnvoll ist
    resultSymInt32QC.scale = mean * labelAsymQC->scale;

    convertTensor(&resultSymInt32, result);
}


void MSELossBackward(tensor_t *modelOutput, tensor_t *label, tensor_t *result) {
    qtype_t modelOutputQType = modelOutput->quantization->type;

    switch (modelOutputQType) {
    case FLOAT32:
        MSELossBackwardFloat(modelOutput, label, result);
        break;
    case ASYM:
        MSELossBackwardAsym(modelOutput, label, result);
        break;
    default:
        printf("Error in MSE Backward: qtype not supported\n");
        break;
    }
}
