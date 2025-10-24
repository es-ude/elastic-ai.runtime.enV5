#include "Tensor.h"
#include "Linear.h"
#include "Arithmetic.h"
#include "Add.h"
#include "Matmul.h"
#include "Mul.h"

#include <DTypes.h>
#include <stdio.h>
#include <string.h>


void linearForwardFloat(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    matmulFloatTensors(w, input, output);
    addFloatTensorsInplace(output, b);
}

void linearForwardLinearQ(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    /* PSEUDOCode
     * intermediate_int = int(w) * int(input)
     * intermediate_int += bias;
     * if output->qtype == FLOAT
     * {
     * output = float(intermediate_int);
     * output *= w->qconfig->scale * input->qConfig->scale;
     * }
     * else if output->qType == LINEAR
     * {
     * output = linearQ(intermediate_int)
     * output->qconfig->scale *= w->qconfig->scale * input->qConfig->scale;
     * }
     */
    quantization_t intermediate_int_quantization;
    initInt32Quantization(&intermediate_int_quantization);
    uint8_t intermediate_int_data[calcNumberOfElementsByTensor(output) * calcBytesPerElement(
                                      &intermediate_int_quantization)];
    tensor_t intermediate_int = {.data = intermediate_int_data,
                                 .quantization = &intermediate_int_quantization,};
    {
        quantization_t w_int_quantization;
        initInt32Quantization(&w_int_quantization);
        uint8_t w_int_data[calcNumberOfElementsByTensor(w) * calcBytesPerElement(
                               &w_int_quantization)];
        tensor_t w_int = {.data = w_int_data, .quantization = &w_int_quantization,};
        convertTensor(w, &w_int);

        quantization_t input_int_quantization;
        initInt32Quantization(&input_int_quantization);
        uint8_t input_int_data[calcNumberOfElementsByTensor(input) * calcBytesPerElement(
                                   &input_int_quantization)];
        tensor_t input_int = {.data = input_int_data, .quantization = &input_int_quantization,};
        convertTensor(input, &input_int);

        matmulInt32Tensors(&w_int, &input_int, &intermediate_int);
    }
    int32ElementArithmeticFunc_t add = addInt32s;
    int32PointWiseArithmeticInplace(&intermediate_int, b, add);

    linearQConfig_t *linearQWConfig = (linearQConfig_t *)w->quantization->qConfig;
    linearQConfig_t *linearQInputConfig = (linearQConfig_t *)input->quantization->qConfig;
    if (output->quantization->type == FLOAT32) {
        quantization_t intermediate_float_quantization;
        initInt32Quantization(&intermediate_float_quantization);
        uint8_t intermediate_float_data[calcNumberOfElementsByTensor(input) * calcBytesPerElement(
                                            &intermediate_float_quantization)];
        tensor_t intermediate_float = {.data = intermediate_float_data,
                                       .quantization = &intermediate_float_quantization,};
        convertTensor(&intermediate_int, output);
        floatElementArithmeticFunc_t mul = mulFloats;
        float totalScale = linearQWConfig->scale * linearQInputConfig->scale;
        floatElementWithTensorArithmeticInplace(output, totalScale, mul);
    } else if (output->quantization->type == LINEAR) {
        convertTensor(&intermediate_int, output);
        linearQConfig_t *linearQOutConfig = (linearQConfig_t *)output->quantization->qConfig;
        linearQOutConfig->scale *= linearQWConfig->scale * linearQInputConfig->scale;
    }
}


void linearForward(void *config, tensor_t *input, tensor_t *output) {
    /*PSEUDO-Code:
     *Convert Params W und B to Tensors
     *if lConfig is a type FLOATLayer go there
     *else go to linearlayer
     */
    linearConfig_t *lConfig = (linearConfig_t *)config;

    tensor_t weights;
    size_t orderOfDimensionsWeights[lConfig->weight->numberOfDimensions];
    getTensorFromParameter(lConfig->weight, &weights, orderOfDimensionsWeights);

    tensor_t bias;
    size_t orderOfDimensionsBias[lConfig->bias->numberOfDimensions];
    getTensorFromParameter(lConfig->bias, &bias, orderOfDimensionsBias);

    if (lConfig->type == FLOATLAYER) {
        linearForwardFloat(&weights, &bias, input, output);

    } else if (lConfig->type == LINEARLAYER) {
        linearForwardLinearQ(&weights, &bias, input, output);

    }
}

void linearBackwardFloat(void *config, tensor_t* loss, tensor_t* output, tensor_t* propLossTensor) {
    linearConfig_t *linearConfig = config;
    size_t bytesPerElement = sizeof(float);
    size_t numberOfOutputValues = calcNumberOfElementsByTensor(output);

    size_t outputSize = linearConfig->weight->dimensions[0];
    size_t inputSize = linearConfig->weight->dimensions[1];

    float propLoss[numberOfOutputValues];

    for(size_t lossIndex = 0; lossIndex < outputSize; lossIndex++) {
        size_t lossByteIndex = lossIndex * bytesPerElement;
        size_t biasByteIndex = lossIndex * bytesPerElement;

        uint8_t *lossAddress = &loss->data[lossByteIndex];
        float lossValue = readBytesAsFloat(lossAddress);



        for(size_t inputIndex = 0; inputIndex < inputSize; inputIndex++) {
            size_t weightIndex = lossIndex * inputSize + inputIndex;
            size_t weightByteIndex = weightIndex * bytesPerElement;

            size_t dataByteIndex = inputIndex * bytesPerElement;
            size_t outputByteIndex = inputIndex * bytesPerElement;

            uint8_t *weightAddress = &linearConfig->weight->data[weightByteIndex];
            float weightValue = readBytesAsFloat(weightAddress);

            uint8_t *weightGradAddress = &linearConfig->weight->grad[weightByteIndex];
            float weightGrad = readBytesAsFloat(weightGradAddress);

            float propLossValue = propLoss[inputIndex];

            uint8_t *outputAddress = &output->data[outputByteIndex];
            float outputValue = readBytesAsFloat(outputAddress);

            weightGrad += lossValue * outputValue;
            writeFloatToByteArray(weightGrad, weightGradAddress);

            propLossValue += weightValue * lossValue;
        }

        uint8_t *biasGradAddress = &linearConfig->bias->grad[biasByteIndex];
        float biasGrad = readBytesAsFloat(biasGradAddress);
        biasGrad += lossValue;
        writeFloatToByteArray(biasGrad, biasGradAddress);
    }
    memcpy(propLossTensor->data, propLoss, numberOfOutputValues);
}
