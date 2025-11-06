#include "Tensor.h"
#include "Linear.h"
#include "Add.h"
#include "Matmul.h"
#include "Mul.h"


#include <DTypes.h>
#include <stdio.h>
#include <string.h>

void printTensor(tensor_t *t) {
    quantization_t *q = t->quantization;
    printf("TENSOR BEGINN \n");
    size_t numValues = calcNumberOfElementsByTensor(t);
    int32_t data[numValues];
    switch (q->type) {
    case ASYM:
        asymQConfig_t *lq = q->qConfig;
        printf("linearQ \n");
        printf("scale=%f\n", lq->scale);
        printf("offset=%i\n", lq->zeroPoint);
        printf("Data \n");
        for (size_t i=0; i<numValues;i++) {
            printf("%i\n", t->data[i]);
        }
        break;
    case FLOAT32:
        printf("float32Q \n");
        break;
    case INT32:
        printf("INT32Q \n");


        readBytesAsInt32Array(numValues, t->data, data);
        for (size_t i=0; i<numValues; i++) {
            printf("%i\n", data[i]);
        }
        break;
    default:
        printf("WTF");
    }

    printf("TENSOR END \n");
    printf("\n");
}
void linearForwardFloat(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    matmulFloatTensors(w, input, output);
    addFloatTensorsInplace(output, b);
}

void linearForwardAsym(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    quantization_t intermediateOutputIntQ;
    initInt32Quantization(&intermediateOutputIntQ);
    size_t numberOfOutputs = calcNumberOfElementsByTensor(output);
    uint8_t intermediateOutputIntData[
        numberOfOutputs * calcBytesPerElement(&intermediateOutputIntQ)];
    tensor_t intermediateOutputInt;
    setTensorValuesForConversion(intermediateOutputIntData, &intermediateOutputIntQ,
                                 output, &intermediateOutputInt);

    quantization_t weightsIntQ;
    initInt32Quantization(&weightsIntQ);
    size_t numberOfWeights = calcNumberOfElementsByTensor(w);
    uint8_t weightsIntData[numberOfWeights * calcBytesPerElement(&weightsIntQ)];
    tensor_t weightsInt;
    setTensorValuesForConversion(weightsIntData, &weightsIntQ, w, &weightsInt);
    convertTensor(w, &weightsInt);

    quantization_t inputIntQ;
    initInt32Quantization(&inputIntQ);
    size_t numberOfInputs = calcNumberOfElementsByTensor(input);
    uint8_t inputIntData[numberOfInputs * calcBytesPerElement(&inputIntQ)];
    tensor_t inputInt;
    setTensorValuesForConversion(inputIntData, &inputIntQ, input, &inputInt);
    convertTensor(input, &inputInt);

    matmulInt32Tensors(&weightsInt, &inputInt, &intermediateOutputInt);
    addInt32TensorsInplace(&intermediateOutputInt, b);

    asymQConfig_t *asymQWeightConfig = w->quantization->qConfig;
    asymQConfig_t *asymQInputConfig = input->quantization->qConfig;

    if (output->quantization->type == FLOAT32) {
        convertTensor(&intermediateOutputInt, output);
        float totalScale = asymQWeightConfig->scale * asymQInputConfig->scale;
        float zeroPoint = asymQWeightConfig->zeroPoint;
        addFloatElementWithTensorInplace(output, zeroPoint);

        mulFloatElementWithTensorInplace(output, totalScale);
    } else if (output->quantization->type == ASYM) {
        convertTensor(&intermediateOutputInt, output);
        asymQConfig_t *asymQOutConfig = output->quantization->qConfig;
        asymQOutConfig->scale *= asymQWeightConfig->scale * asymQInputConfig->scale;
    }
}


void linearForward(void *config, tensor_t *input, tensor_t *output) {
    linearConfig_t *lConfig = config;

    tensor_t weights;
    size_t orderOfDimensionsWeights[lConfig->weight->numberOfDimensions];
    getTensorFromParameter(lConfig->weight, &weights, orderOfDimensionsWeights);

    tensor_t bias;
    size_t orderOfDimensionsBias[lConfig->bias->numberOfDimensions];
    getTensorFromParameter(lConfig->bias, &bias, orderOfDimensionsBias);

    if (lConfig->type == FLOATLAYER) {
        linearForwardFloat(&weights, &bias, input, output);

    } else if (lConfig->type == ASYMLAYER) {
        linearForwardAsym(&weights, &bias, input, output);

    }
}

void calcWeightGrads(tensor_t *loss, tensor_t *forwardInput, tensor_t *weightGrads) {
    transposeTensor(forwardInput, 0, 1);
    matMulTensors(loss, forwardInput, weightGrads);
    transposeTensor(forwardInput, 0, 1);
}

void calcBiasGrads(tensor_t *biasGrads, tensor_t *loss) {
    addTensorsInplace(biasGrads, loss);
}

void calcPropLoss(tensor_t *weights, tensor_t *loss, tensor_t *propLoss) {
    transposeTensor(weights, 0, 1);
    matMulTensors(weights, loss, propLoss);
    transposeTensor(weights, 0, 01);
}

void linearBackwardFloat(void *config, tensor_t *loss, tensor_t *output, tensor_t *propLossTensor) {
    linearConfig_t *linearConfig = config;

    size_t outputSize = linearConfig->weight->dimensions[0];
    size_t numberOfWeights = calcNumberOfElementsByDims(linearConfig->weight->numberOfDimensions,
                                                        linearConfig->weight->dimensions);
    size_t numberOfBiases = calcNumberOfElementsByDims(linearConfig->bias->numberOfDimensions,
                                                       linearConfig->bias->dimensions);

    // weightGrads
    float weightGradData[numberOfWeights];
    size_t weightGradOrderOfDims[linearConfig->weight->numberOfDimensions];
    for (size_t i = 0; i < linearConfig->weight->numberOfDimensions; i++) {
        weightGradOrderOfDims[i] = i;
    }

    tensor_t weightGrad = {
        .data = weightGradData,
        .dimensions = linearConfig->weight->dimensions,
        .quantization = linearConfig->weight->gradQuantization,
        .sparsityBitmask = linearConfig->weight->sparsityBitmask,
        .numberOfDimensions = linearConfig->weight->numberOfDimensions,
        .orderOfDimensions = weightGradOrderOfDims
    };

    calcWeightGrads(loss, output, &weightGrad);
    memcpy(linearConfig->weight->grad, weightGrad.data, numberOfWeights * sizeof(float));

    // biasGrads
    size_t biasGradDims[] = {outputSize, 1};
    size_t biasGradOrderOfDims[] = {0, 1};
    quantization_t biasGradQ = {.type = FLOAT32};

    tensor_t biasGrad = {
        .data = linearConfig->bias->grad,
        .dimensions = biasGradDims,
        .quantization = &biasGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 2,
        .orderOfDimensions = biasGradOrderOfDims
    };

    calcBiasGrads(&biasGrad, loss);
    memcpy(linearConfig->bias->grad, biasGrad.data, numberOfBiases * sizeof(float));

    // propLoss
    tensor_t weightData;
    size_t weightDataOrderOfDims[linearConfig->weight->numberOfDimensions];
    getTensorFromParameter(linearConfig->weight, &weightData, weightDataOrderOfDims);

    calcPropLoss(&weightData, loss, propLossTensor);
}

void linearBackwardAsym(void *config, tensor_t *loss, tensor_t *forwardInput,
                           tensor_t *propLossTensor) {
    linearConfig_t *linearConfig = config;

    size_t outputSize = linearConfig->weight->dimensions[0];
    size_t inputSize = linearConfig->weight->dimensions[1];
    size_t numberOfWeights = calcNumberOfElementsByDims(linearConfig->weight->numberOfDimensions,
                                                        linearConfig->weight->dimensions);
    size_t numberOfBiases = calcNumberOfElementsByDims(linearConfig->bias->numberOfDimensions,
                                                       linearConfig->bias->dimensions);

    // Get data and grad tensors from parameters
    tensor_t weightsAsym;
    size_t weightsAsymOrderOfDims[linearConfig->weight->numberOfDimensions];
    for (size_t i = 0; i < linearConfig->weight->numberOfDimensions; i++) {
        weightsAsymOrderOfDims[i] = i;
    }
    getTensorFromParameter(linearConfig->weight, &weightsAsym, weightsAsymOrderOfDims);

    tensor_t weightGradsAsym;
    size_t weightGradsAsymOrderOfDims[linearConfig->weight->numberOfDimensions];
    for (size_t i = 0; i < linearConfig->weight->numberOfDimensions; i++) {
        weightGradsAsymOrderOfDims[i] = i;
    }
    getGradTensorFromParameter(linearConfig->weight, &weightGradsAsym,
                               weightGradsAsymOrderOfDims);

    tensor_t biasAsym;
    size_t biasAsymOrderOfDims[linearConfig->bias->numberOfDimensions];
    for (size_t i = 0; i < linearConfig->bias->numberOfDimensions; i++) {
        biasAsymOrderOfDims[i] = i;
    }
    getTensorFromParameter(linearConfig->bias, &biasAsym, biasAsymOrderOfDims);

    tensor_t biasGradsAsym;
    size_t biasGradsAsymOrderOfDims[linearConfig->bias->numberOfDimensions];
    for (size_t i = 0; i < linearConfig->bias->numberOfDimensions; i++) {
        biasGradsAsymOrderOfDims[i] = i;
    }
    getTensorFromParameter(linearConfig->bias, &biasGradsAsym, biasGradsAsymOrderOfDims);
    // ___________________________________________________________________________________

    // Get everything as int32 tensors
    quantization_t weightsIntQ;
    initInt32Quantization(&weightsIntQ);
    uint8_t weightsIntData[numberOfWeights * sizeof(int32_t)];
    tensor_t weightsInt;
    setTensorValuesForConversion(weightsIntData, &weightsIntQ, &weightsAsym, &weightsInt);
    convertTensor(&weightsAsym, &weightsInt);

    quantization_t forwardInputIntQ;
    initInt32Quantization(&forwardInputIntQ);
    uint8_t forwardInputIntData[outputSize * sizeof(int32_t)];
    tensor_t forwardInputInt;
    setTensorValuesForConversion(forwardInputIntData, &forwardInputIntQ, forwardInput,
                                 &forwardInputInt);
    convertTensor(forwardInput, &forwardInputInt);

    quantization_t lossIntQ;
    initInt32Quantization(&lossIntQ);
    uint8_t lossIntData[calcNumberOfElementsByTensor(loss) * sizeof(int32_t)];
    tensor_t lossInt;
    setTensorValuesForConversion(lossIntData, &lossIntQ, loss, &lossInt);
    convertTensor(loss, &lossInt);

    quantization_t intermediateWeightGradsQ;
    initInt32Quantization(&intermediateWeightGradsQ);
    uint8_t intermediateWeightGradsData[numberOfWeights * sizeof(int32_t)];
    tensor_t intermediateWeightGradsInt;
    setTensorValuesForConversion(
        intermediateWeightGradsData, &intermediateWeightGradsQ, &weightGradsAsym,
        &intermediateWeightGradsInt);
    convertTensor(&weightGradsAsym, &intermediateWeightGradsInt);

    quantization_t intermediateBiasGradsQ;
    initInt32Quantization(&intermediateBiasGradsQ);
    uint8_t intermediateBiasGradsData[numberOfBiases * sizeof(int32_t)];
    tensor_t intermediateBiasGradsInt;
    setTensorValuesForConversion(intermediateBiasGradsData, &intermediateBiasGradsQ,
                                 &biasGradsAsym, &intermediateBiasGradsInt);
    convertTensor(&biasGradsAsym, &intermediateBiasGradsInt);

    quantization_t intermediatePropLossIntQ;
    initInt32Quantization(&intermediatePropLossIntQ);
    uint8_t intermediatePropLossIntData[inputSize * sizeof(int32_t)];
    tensor_t intermediatePropLossInt;
    setTensorValuesForConversion(intermediatePropLossIntData, &intermediatePropLossIntQ,
                                 propLossTensor, &intermediatePropLossInt);
    convertTensor(propLossTensor, &intermediatePropLossInt);
    // ______________________________________________________________-

    // Weight gradients
    calcWeightGrads(&lossInt, &forwardInputInt, &intermediateWeightGradsInt);
    convertTensor(&intermediateWeightGradsInt, &weightGradsAsym);

    asymQConfig_t *linearWeightGradQConfig = linearConfig->weight->gradQuantization->qConfig;
    asymQConfig_t *linearWeightQConfig = linearConfig->weight->dataQuantization->qConfig;
    asymQConfig_t *linearForwardInputQConfig = forwardInput->quantization->qConfig;

    printf("weight scale: %f, input scale: %f\n", linearWeightQConfig->scale, linearForwardInputQConfig->scale);
    linearWeightGradQConfig->scale = linearWeightQConfig->scale * linearForwardInputQConfig->scale;


    //linearWeightGradQConfig->zeroPoint = -255;
    memcpy(linearConfig->weight->grad, weightGradsAsym.data,
           numberOfWeights * calcBytesPerElement(linearConfig->weight->gradQuantization));
    // Bias gradients

    addInt32TensorsInplace(&intermediateBiasGradsInt, &lossInt);
    convertTensor(&intermediateBiasGradsInt, &biasGradsAsym);
    memcpy(linearConfig->bias->grad, biasGradsAsym.data,
           numberOfBiases * calcBytesPerElement(linearConfig->bias->gradQuantization));

    asymQConfig_t *linearBiasGradQConfig = linearConfig->bias->gradQuantization->qConfig;
    asymQConfig_t *lossQConfig = loss->quantization->qConfig;

    linearBiasGradQConfig->scale = linearWeightQConfig->scale;

    // Propagated loss
    calcPropLoss(&weightsInt, &lossInt, &intermediatePropLossInt);
    convertTensor(&intermediatePropLossInt, propLossTensor);
    asymQConfig_t *linearPropLossQConfig = propLossTensor->quantization->qConfig;
    linearPropLossQConfig->scale = linearWeightQConfig->scale * linearForwardInputQConfig->scale;
}


void linearBackward(void *config, tensor_t *loss, tensor_t *output, tensor_t *propLoss) {
    linearConfig_t *lConfig = config;

    if (lConfig->type == FLOATLAYER) {
        linearBackwardFloat(config, loss, output, propLoss);

    } else if (lConfig->type == ASYMLAYER) {
        linearBackwardAsym(config, loss, output, propLoss);
    }
}
