#include "Relu.h"
#include "Tensor.h"

#include "Comparison.h"
#include "DTypes.h"
#include "TensorConversion.h"
#include <stdio.h>

void reluForwardFloat32(tensor_t *input, tensor_t *output) {
    gteFloatValue(input, 0, 0, output);
}

void reluForwardAsym(tensor_t *input, tensor_t *output) {
    size_t numberOfElements = calcNumberOfElementsByTensor(input);

    asymQConfig_t *inputAsymQC = input->quantization->qConfig;
    symInt32QConfig_t inputSymInt32QC;
    initSymInt32QConfig(inputAsymQC->roundingMode, &inputSymInt32QC);
    quantization_t inputSymInt32Q;
    initSymInt32Quantization(&inputSymInt32QC, &inputSymInt32Q);
    int32_t inputSymInt32Data[numberOfElements];
    tensor_t inputSymInt32;
    setTensorValuesForConversion(inputSymInt32Data, &inputSymInt32Q, input, &inputSymInt32);
    convertTensor(input, &inputSymInt32);

    asymQConfig_t *outputAsymQC = output->quantization->qConfig;
    symInt32QConfig_t outputSymInt32QC;
    initSymInt32QConfig(outputAsymQC->roundingMode, &outputSymInt32QC);
    quantization_t outputSymInt32Q;
    initSymInt32Quantization(&outputSymInt32QC, &outputSymInt32Q);
    int32_t outputSymInt32Data[numberOfElements];
    tensor_t outputSymInt32;
    setTensorValuesForConversion(outputSymInt32Data, &outputSymInt32Q, output, &outputSymInt32);
    convertTensor(output, &outputSymInt32);

    gteSymInt32Zero(&inputSymInt32, 0, &outputSymInt32);

    convertTensor(&outputSymInt32, output);
}

void reluForward(void *config, tensor_t *input, tensor_t *output) {
    qtype_t inputQType = input->quantization->type;

    switch (inputQType) {
    case FLOAT32:
        reluForwardFloat32(input, output);
        break;
    case ASYM:
        reluForwardAsym(input, output);
        break;
    default:
        break;
    }
}


void reluBackwardFloat32(tensor_t *input, tensor_t *gradOutputFromPreviousLayer,
                         tensor_t *gradInputForNextLayer) {
    size_t numberOfElements = calcNumberOfElementsByTensor(input);

    float *inputArray = (float *)input->data;
    float *gradOutArray = (float *)gradOutputFromPreviousLayer->data;
    float *gradInArray = (float *)gradInputForNextLayer->data;

    for (size_t i = 0; i < numberOfElements; i++) {
        if (inputArray[i] <= 0) {
            gradInArray[i] = 0;
        } else {
            gradInArray[i] = gradOutArray[i];
        }
    }
}


void reluBackwardAsym(tensor_t *input, tensor_t *gradOutputFromPrevLayer,
                      tensor_t *gradInputForNextLayer) {

    size_t numberOfElements = calcNumberOfElementsByTensor(input);

    tensor_t inputSymInt32;
    int32_t inputSymInt32Data[numberOfElements];
    asymQConfig_t *inputAsymQC = input->quantization->qConfig;
    symInt32QConfig_t inputSymInt32QC;
    initSymInt32QConfig(inputAsymQC->roundingMode, &inputSymInt32QC);
    quantization_t inputSymInt32Q;
    initSymInt32Quantization(&inputSymInt32QC, &inputSymInt32Q);
    setTensorValuesForConversion(inputSymInt32Data, &inputSymInt32Q, input, &inputSymInt32);
    convertTensor(input, &inputSymInt32);

    tensor_t gradOutputSymInt32;
    int32_t gradOutputSymInt32Data[numberOfElements];
    asymQConfig_t *gradOutputAsymQC = gradOutputFromPrevLayer->quantization->qConfig;
    symInt32QConfig_t gradOutputSymInt32QC;
    initSymInt32QConfig(gradOutputAsymQC->roundingMode, &gradOutputSymInt32QC);
    quantization_t gradOutputSymInt32Q;
    initSymInt32Quantization(&gradOutputSymInt32QC, &gradOutputSymInt32Q);
    setTensorValuesForConversion(gradOutputSymInt32Data, &gradOutputSymInt32Q,
                                 gradOutputFromPrevLayer, &gradOutputSymInt32);
    convertTensor(gradOutputFromPrevLayer, &gradOutputSymInt32);

    tensor_t gradInputSymInt32;
    int32_t gradInputSymInt32Data[numberOfElements];
    asymQConfig_t *gradInputAsymQC = gradInputForNextLayer->quantization->qConfig;
    symInt32QConfig_t gradInputSymInt32QC;
    initSymInt32QConfig(gradInputAsymQC->roundingMode, &gradInputSymInt32QC);
    quantization_t gradInputSymInt32Q;
    initSymInt32Quantization(&gradInputSymInt32QC, &gradInputSymInt32Q);
    setTensorValuesForConversion(gradInputSymInt32Data, &gradInputSymInt32Q, gradInputForNextLayer,
                                 &gradInputSymInt32);

    int32_t *inputArray = (int32_t *)inputSymInt32.data;
    int32_t *gradOutputArray = (int32_t *)gradOutputSymInt32.data;
    int32_t *gradInputArray = (int32_t *)gradInputSymInt32.data;

    for (size_t i = 0; i < numberOfElements; i++) {
        if (inputArray[i] <= 0) {
            gradInputArray[i] = 0;
        } else {
            gradInputArray[i] = gradOutputArray[i];
        }
    }

    gradInputSymInt32QC.scale = gradOutputSymInt32QC.scale;
    convertTensor(&gradInputSymInt32, gradInputForNextLayer);
}

void reluBackward(void *config, tensor_t *input, tensor_t *gradOutputFromPreviousLayer,
                  tensor_t *gradInput) {
    qtype_t inputQType = input->quantization->type;

    switch (inputQType) {
    case FLOAT32:
        reluBackwardFloat32(input, gradOutputFromPreviousLayer, gradInput);
        break;
    case ASYM:
        reluBackwardAsym(input, gradOutputFromPreviousLayer, gradInput);
        break;
    default:
        break;
    }
}
