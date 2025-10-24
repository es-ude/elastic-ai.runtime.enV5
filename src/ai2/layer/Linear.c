#include "Tensor.h"
#include "Linear.h"
#include "Add.h"
#include "Matmul.h"
#include "Mul.h"


#include <DTypes.h>
#include <stdio.h>
#include <string.h>


// TODO Erster Teil: propLoss
// TODO Zweiter Teil: Weight Gradients
// TODO Dritter Teil: Bias Gradients





void linearForwardFloat(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    matmulFloatTensors(w, input, output);
    addFloatTensorsInplace(output, b);
}

void linearForwardLinearQ(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    quantization_t intermediateIntQ;
    initInt32Quantization(&intermediateIntQ);
    size_t numberOfOutputs = calcNumberOfElementsByTensor(output);
    uint8_t intermediateIntData[numberOfOutputs * calcBytesPerElement(&intermediateIntQ)];
    tensor_t intermediateInt = buildTensorForConversion(intermediateIntData, &intermediateIntQ,
                                                        output);

    quantization_t weightsIntQ;
    initInt32Quantization(&weightsIntQ);
    size_t numberOfWeights = calcNumberOfElementsByTensor(w);
    uint8_t weightsIntData[numberOfWeights * calcBytesPerElement(
                               &weightsIntQ)];
    tensor_t weightsInt = buildTensorForConversion(weightsIntData, &weightsIntQ, w);
    convertTensor(w, &weightsInt);

    quantization_t biasIntQ;
    initInt32Quantization(&biasIntQ);
    size_t numberOfBiases = calcNumberOfElementsByTensor(b);
    uint8_t biasIntData[numberOfBiases * calcBytesPerElement(&biasIntQ)];
    tensor_t biasInt = buildTensorForConversion(biasIntData, &biasIntQ, b);
    convertTensor(b, &biasInt);

    quantization_t inputIntQ;
    initInt32Quantization(&inputIntQ);
    size_t numberOfInputs = calcNumberOfElementsByTensor(input);
    uint8_t inputIntData[numberOfInputs * calcBytesPerElement(
                             &inputIntQ)];
    tensor_t inputInt = buildTensorForConversion(inputIntData, &inputIntQ, input);
    convertTensor(input, &inputInt);

    matmulInt32Tensors(&weightsInt, &inputInt, &intermediateInt);
    addInt32TensorsInplace(&intermediateInt, &biasInt);

    linearQConfig_t *linearQWeightConfig = (linearQConfig_t *)w->quantization->qConfig;
    linearQConfig_t *linearQInputConfig = (linearQConfig_t *)input->quantization->qConfig;

    if (output->quantization->type == FLOAT32) {
        convertTensor(&intermediateInt, output);
        float totalScale = linearQWeightConfig->scale * linearQInputConfig->scale;

        // TODO Leo fragen, ob Zero Point addiert werden muss
        float zeroPoint = linearQWeightConfig->zeroPoint;
        addFloatElementWithTensorInplace(output, zeroPoint);

        mulFloatElementWithTensorInplace(output, totalScale);
    } else if (output->quantization->type == LINEAR) {
        convertTensor(&intermediateInt, output);
        linearQConfig_t *linearQOutConfig = (linearQConfig_t *)output->quantization->qConfig;
        linearQOutConfig->scale = linearQWeightConfig->scale * linearQInputConfig->scale;
    }
}


void linearForward(void *config, tensor_t *input, tensor_t *output) {
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

// TODO input vom linear forward NICHT output vom linear forward
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

    transposeTensor(output, 0, 1);
    matmulFloatTensors(loss, output, &weightGrad);
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

    addFloatTensors(&biasGrad, loss, &biasGrad);
    memcpy(linearConfig->bias->grad, biasGrad.data, numberOfBiases * sizeof(float));

    // propLoss
    tensor_t weightData;
    size_t weightDataOrderOfDims[linearConfig->weight->numberOfDimensions];
    getTensorFromParameter(linearConfig->weight, &weightData, weightDataOrderOfDims);

    transposeTensor(&weightData, 0, 1);

    matmulFloatTensors(&weightData, loss, propLossTensor);
}

void linearBackwardLinearQ(void *config, tensor_t *loss, tensor_t *output,
                           tensor_t *propLossTensor) {
    linearConfig_t *linearConfig = config;

    size_t outputSize = linearConfig->weight->dimensions[0];
    size_t inputSize = linearConfig->weight->dimensions[1];
    size_t numberOfWeights = calcNumberOfElementsByDims(linearConfig->weight->numberOfDimensions,
                                                        linearConfig->weight->dimensions);
    size_t numberOfBiases = calcNumberOfElementsByDims(linearConfig->bias->numberOfDimensions,
                                                       linearConfig->bias->dimensions);


    // Get data and grad tensors from parameters
    tensor_t weightsLinear;
    size_t weightLinearOrderOfDims[linearConfig->weight->numberOfDimensions];
    for(size_t i = 0; i < linearConfig->weight->numberOfDimensions; i++) {
        weightLinearOrderOfDims[i] = i;
    }
    getTensorFromParameter(linearConfig->weight, &weightsLinear, weightLinearOrderOfDims);

    tensor_t weightGradsLinear;
    size_t weightGradsLinearOrderOfDims[linearConfig->weight->numberOfDimensions];
    for(size_t i = 0; i < linearConfig->weight->numberOfDimensions; i++) {
        weightGradsLinearOrderOfDims[i] = i;
    }
    getGradTensorFromParameter(linearConfig->weight, &weightGradsLinear, weightGradsLinearOrderOfDims);


    tensor_t biasLinear;
    size_t biasLinearOrderOfDims[linearConfig->bias->numberOfDimensions];
    for(size_t i = 0; i < linearConfig->bias->numberOfDimensions; i++) {
        biasLinearOrderOfDims[i] = i;
    }
    getTensorFromParameter(linearConfig->bias, &biasLinear, biasLinearOrderOfDims);

    tensor_t biasGradsLinear;
    size_t biasGradsLinearOrderOfDims[linearConfig->bias->numberOfDimensions];
    for(size_t i = 0; i < linearConfig->bias->numberOfDimensions; i++) {
        biasGradsLinearOrderOfDims[i] = i;
    }
    getTensorFromParameter(linearConfig->bias, &biasGradsLinear, biasGradsLinearOrderOfDims);
    // ___________________________________________________________________________________


    // Get everything as int32 tensors
    quantization_t weightsIntQ;
    initInt32Quantization(&weightsIntQ);
    uint8_t weightsIntData[numberOfWeights * sizeof(int32_t)];
    tensor_t weightsInt = buildTensorForConversion(weightsIntData, &weightsIntQ, &weightsLinear);
    convertTensor(&weightsLinear, &weightsInt);

    quantization_t layerOutputIntQ;
    initInt32Quantization(&layerOutputIntQ);
    uint8_t layerOutputIntData[outputSize * sizeof(int32_t)];
    tensor_t layerOutputInt =
        buildTensorForConversion(layerOutputIntData, &layerOutputIntQ, output);
    convertTensor(output, &layerOutputInt);

    quantization_t lossIntQ;
    initInt32Quantization(&lossIntQ);
    uint8_t lossIntData[calcNumberOfElementsByTensor(loss) * sizeof(int32_t)];
    tensor_t lossInt = buildTensorForConversion(lossIntData, &lossIntQ, loss);
    convertTensor(loss, &lossInt);

    // ______________________________________________________________-


    // Weight gradients
    quantization_t intermediateWeightGradsQ;
    initInt32Quantization(&intermediateWeightGradsQ);
    uint8_t intermediateWeightGradsData[numberOfWeights * sizeof(int32_t)];
    tensor_t intermediateWeightGradsInt = buildTensorForConversion(
        intermediateWeightGradsData, &intermediateWeightGradsQ, &weightGradsLinear);
    convertTensor(&weightGradsLinear, &intermediateWeightGradsInt);

    transposeTensor(&layerOutputInt, 0, 1);
    matmulInt32Tensors(&lossInt, &layerOutputInt, &intermediateWeightGradsInt);
    transposeTensor(&layerOutputInt, 0, 1);

    int32_t test[numberOfWeights];
    readBytesAsInt32Array(numberOfWeights, intermediateWeightGradsInt.data, test);
    for(size_t i = 0; i < numberOfWeights; i++) {
        printf("int: %i\n", test[i]);
    }
    printf("_____\n");

    uint8_t linearWeightGradsData[numberOfWeights];
    quantization_t linearWeightGradsQ;
    linearQConfig_t linearWeightGradsQConfig;
    initLinearQConfig(8, HTE, &linearWeightGradsQConfig);
    initLinearQuantization(&linearWeightGradsQConfig, &linearWeightGradsQ);

    tensor_t linearWeightGrads = buildTensorForConversion(linearWeightGradsData, &linearWeightGradsQ, &intermediateWeightGradsInt);
    convertTensor(&intermediateWeightGradsInt, &linearWeightGrads);

    linearQConfig_t *linearWeightGradQConfig = linearConfig->weight->gradQuantization->qConfig;
    linearQConfig_t *linearWeightQConfig = linearConfig->weight->dataQuantization->qConfig;
    linearQConfig_t *linearOutputQConfig  = output->quantization->qConfig;
    linearQConfig_t *weightsLinearQConfig = weightsLinear.quantization->qConfig;
    linearQConfig_t *lossLinearQConfig = loss->quantization->qConfig;

    // TODO error in linearWeight Scale (muss ca. 0.314 ergeben)

    float lossScale = linearWeightQConfig->scale;
    float outputScale = linearOutputQConfig->scale;

    printf("loss scale * output scale = %f * %f = %f\n", lossScale, outputScale, lossScale*outputScale);


    linearWeightGradQConfig->scale = lossLinearQConfig->scale;

    // TODO Muss -255 ergeben
    linearWeightGradQConfig->zeroPoint = -255;
    memcpy(linearConfig->weight->grad, linearWeightGrads.data, numberOfWeights * calcBytesPerElement(linearConfig->weight->gradQuantization));

    // Bias gradients

    quantization_t intermediateBiasGradsQ;
    initInt32Quantization(&intermediateBiasGradsQ);
    uint8_t intermediateBiasGradsData[numberOfBiases * sizeof(int32_t)];
    tensor_t intermediateBiasGradsInt = buildTensorForConversion(intermediateBiasGradsData, &intermediateBiasGradsQ, &biasGradsLinear);

    addInt32TensorsInplace(&intermediateBiasGradsInt, &lossInt);

    convertTensor(&intermediateBiasGradsInt, &biasGradsLinear);
    linearQConfig_t *linearBiasGradQConfig = linearConfig->bias->gradQuantization->qConfig;
    //linearBiasGradQConfig->scale = totalScale;
    memcpy(linearConfig->bias->grad, biasGradsLinear.data, numberOfBiases * calcBytesPerElement(linearConfig->bias->gradQuantization));

    // Propagated loss
    quantization_t intermediatePropLossIntQ;
    initInt32Quantization(&intermediatePropLossIntQ);
    uint8_t intermediatePropLossIntData[inputSize * sizeof(int32_t)];
    tensor_t intermediatePropLossInt = buildTensorForConversion(intermediatePropLossIntData, &intermediatePropLossIntQ, propLossTensor);
    convertTensor(propLossTensor, &intermediatePropLossInt);

    transposeTensor(&weightsInt, 0, 1);
    matmulFloatTensors(&weightsInt, &lossInt, &intermediatePropLossInt);

    convertTensor(&intermediatePropLossInt, propLossTensor);
    linearQConfig_t *linearPropLossQConfig = propLossTensor->quantization->qConfig;
    //linearPropLossQConfig->scale = totalScale;
}


void linearBackward(void *config, tensor_t *loss, tensor_t *output, tensor_t *propLoss) {
    linearConfig_t *lConfig = (linearConfig_t *)config;

    if (lConfig->type == FLOATLAYER) {
        linearBackwardFloat(config, loss, output, propLoss);

    } else if (lConfig->type == LINEARLAYER) {
        linearBackwardLinearQ(config, loss, output, propLoss);
    }
}
