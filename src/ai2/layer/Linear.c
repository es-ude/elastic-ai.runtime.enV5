#include "Tensor.h"
#include "Linear.h"
#include "Add.h"
#include "Matmul.h"
#include "TensorConversion.h"
#include "Rounding.h";
#include "DTypes.h"
#include "Layer.h"

#include <stdio.h>
#include <string.h>

void initLinearConfig(linearConfig_t* linearConfig, linearQType_t qType, parameter_t* weights, parameter_t* bias) {
    linearConfig->qType = qType;
    linearConfig->weights = weights;
    linearConfig->bias = bias;
}


void linearForwardFloat32(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    matmulFloat32Tensors(w, input, output);
    addFloat32TensorsInplace(output, b);
}

void linearForwardAsym(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {

    size_t numberOfWeights = calcNumberOfElementsByTensor(w);
    size_t numberOfOutputs = calcNumberOfElementsByTensor(output);
    size_t numberOfInputs = calcNumberOfElementsByTensor(input);

    symInt32QConfig_t weightsSymInt32QConfig;
    initSymInt32QConfig(HTE, &weightsSymInt32QConfig);
    quantization_t weightsSymInt32Q;
    initSymInt32Quantization(&weightsSymInt32QConfig, &weightsSymInt32Q);
    int32_t weightsSymInt32Data[numberOfWeights];
    tensor_t weightsSymInt32;
    setTensorValuesForConversion(weightsSymInt32Data, &weightsSymInt32Q, w, &weightsSymInt32);
    convertTensor(w, &weightsSymInt32);

    symInt32QConfig_t inputSymInt32QConfig;
    initSymInt32QConfig(HTE, &inputSymInt32QConfig);
    quantization_t inputsSymInt32Q;
    initSymInt32Quantization(&inputSymInt32QConfig, &inputsSymInt32Q);
    int32_t inputSymInt32Data[numberOfInputs];
    tensor_t inputSymInt32;
    setTensorValuesForConversion(inputSymInt32Data, &inputsSymInt32Q, input, &inputSymInt32);
    convertTensor(input, &inputSymInt32);

    symInt32QConfig_t outputSymInt32QConfig;
    initSymInt32QConfig(HTE, &outputSymInt32QConfig);
    quantization_t outputSymInt32Q;
    initSymInt32Quantization(&outputSymInt32QConfig, &outputSymInt32Q);
    int32_t outputSymInt32Data[numberOfOutputs];
    tensor_t outputSymInt32;
    setTensorValuesForConversion(outputSymInt32Data, &outputSymInt32Q,
                                 output, &outputSymInt32);

    matmulSymInt32Tensors(&weightsSymInt32, &inputSymInt32, &outputSymInt32);

    addInt32TensorToSymInt32TensorInplace(&outputSymInt32, b);

    convertTensor(&outputSymInt32, output);
}


void linearForward(void *layerConfig, tensor_t *input, tensor_t *output) {
    linearConfig_t *linearConfig = layerConfig;

    tensor_t weights;
    size_t orderOfDimensionsWeights[linearConfig->weights->tensor.shape.numberOfDimensions];
    initOrderOfDimensions(orderOfDimensionsWeights, linearConfig->weights->tensor.shape.numberOfDimensions);
    getTensorFromParameter(linearConfig->weights, &weights, orderOfDimensionsWeights);

    tensor_t bias;
    size_t orderOfDimensionsBias[linearConfig->bias->tensor.shape.numberOfDimensions];
    initOrderOfDimensions(orderOfDimensionsBias, linearConfig->bias->tensor.shape.numberOfDimensions);
    getTensorFromParameter(linearConfig->bias, &bias, orderOfDimensionsBias);

    if (linearConfig->qType == FLOATLAYER) {
        linearForwardFloat32(&weights, &bias, input, output);

    } else if (linearConfig->qType == ASYMLAYER) {
        linearForwardAsym(&weights, &bias, input, output);

    }
}

void calcWeightGradsFloat32(tensor_t *loss, tensor_t *forwardInput, tensor_t *weightGrads) {
    transposeTensor(forwardInput, 0, 1);
    matmulFloat32Tensors(loss, forwardInput, weightGrads);
    transposeTensor(forwardInput, 0, 1);
}

void calcBiasGradsFloat32(tensor_t *biasGrads, tensor_t *loss) {
    addFloat32TensorsInplace(biasGrads, loss);
}

void calcPropLossFloat32(tensor_t *weights, tensor_t *loss, tensor_t *propLoss) {
    transposeTensor(weights, 0, 1);
    matmulFloat32Tensors(weights, loss, propLoss);
    transposeTensor(weights, 0, 01);
}

void linearBackwardFloat(void *config, tensor_t *loss, tensor_t *output, tensor_t *propLossTensor) {
    linearConfig_t *linearConfig = config;

    size_t outputSize = linearConfig->weights->tensor.shape.dimensions[0];
    size_t numberOfWeights = calcNumberOfElementsByDims(linearConfig->weights->tensor.shape.numberOfDimensions,
                                                        linearConfig->weights->tensor.shape.dimensions);
    size_t numberOfBiases = calcNumberOfElementsByDims(linearConfig->bias->tensor.shape.numberOfDimensions,
                                                       linearConfig->bias->tensor.shape.dimensions);

    tensor_t weightGrad;
    size_t weightGradOrderOfDims[linearConfig->weights->tensor.shape.numberOfDimensions];
    for (size_t i = 0; i < linearConfig->weights->tensor.shape.numberOfDimensions; i++) {
        weightGradOrderOfDims[i] = i;
    }
    getGradTensorFromParameter(linearConfig->weights, &weightGrad, weightGradOrderOfDims);
    calcWeightGradsFloat32(loss, output, &weightGrad);
    memcpy(linearConfig->weights->grad, weightGrad.data, numberOfWeights * sizeof(float));

    tensor_t biasGrad;
    size_t biasGradOrderOfDims[] = {0, 1};
    getGradTensorFromParameter(linearConfig->bias, &biasGrad, biasGradOrderOfDims);
    calcBiasGradsFloat32(&biasGrad, loss);
    memcpy(linearConfig->bias->grad, biasGrad.data, numberOfBiases * sizeof(float));

    tensor_t weightData;
    size_t weightDataOrderOfDims[linearConfig->weights->tensor.shape.numberOfDimensions];
    getTensorFromParameter(linearConfig->weights, &weightData, weightDataOrderOfDims);
    calcPropLossFloat32(&weightData, loss, propLossTensor);
}

void calcWeightGradsAsym(tensor_t *loss, tensor_t *forwardInput, tensor_t *weightGrads) {
    transposeTensor(forwardInput, 0, 1);
    matmulSymInt32Tensors(loss, forwardInput, weightGrads);
    transposeTensor(forwardInput, 0, 1);
}

void calcBiasGradsAsym(tensor_t *biasGrads, tensor_t *loss) {
    addSymInt32TensorsInplace(biasGrads, loss);
}

void calcPropLossAsym(tensor_t *weights, tensor_t *loss, tensor_t *propLoss) {
    transposeTensor(weights, 0, 1);
    matmulSymInt32Tensors(weights, loss, propLoss);
    transposeTensor(weights, 0, 1);
}

void linearBackwardAsym(void *config, tensor_t *loss, tensor_t *forwardInput,
                        tensor_t *propLossTensor) {
    linearConfig_t *linearConfig = config;

    size_t outputSize = linearConfig->weights->tensor.shape.dimensions[0];
    size_t inputSize = linearConfig->weights->tensor.shape.dimensions[1];
    size_t numberOfWeights = calcNumberOfElementsByDims(linearConfig->weights->tensor.shape.numberOfDimensions,
                                                        linearConfig->weights->tensor.shape.dimensions);
    size_t numberOfBiases = calcNumberOfElementsByDims(linearConfig->bias->tensor.shape.numberOfDimensions,
                                                       linearConfig->bias->tensor.shape.dimensions);
    size_t numberOfLosses = calcNumberOfElementsByTensor(loss);

    // Get data and grad tensors from parameters
    tensor_t weightsAsym;
    size_t weightsAsymOrderOfDims[linearConfig->weights->tensor.shape.numberOfDimensions];
    for (size_t i = 0; i < linearConfig->weights->tensor.shape.numberOfDimensions; i++) {
        weightsAsymOrderOfDims[i] = i;
    }
    getTensorFromParameter(linearConfig->weights, &weightsAsym, weightsAsymOrderOfDims);

    tensor_t weightGradsAsym;
    size_t weightGradsAsymOrderOfDims[linearConfig->weights->tensor.shape.numberOfDimensions];
    for (size_t i = 0; i < linearConfig->weights->tensor.shape.numberOfDimensions; i++) {
        weightGradsAsymOrderOfDims[i] = i;
    }
    getGradTensorFromParameter(linearConfig->weights, &weightGradsAsym,
                               weightGradsAsymOrderOfDims);

    tensor_t biasAsym;
    size_t biasAsymOrderOfDims[linearConfig->bias->tensor.shape.numberOfDimensions];
    for (size_t i = 0; i < linearConfig->bias->tensor.shape.numberOfDimensions; i++) {
        biasAsymOrderOfDims[i] = i;
    }
    getTensorFromParameter(linearConfig->bias, &biasAsym, biasAsymOrderOfDims);

    tensor_t biasGradsAsym;
    size_t biasGradsAsymOrderOfDims[linearConfig->bias->tensor.shape.numberOfDimensions];
    for (size_t i = 0; i < linearConfig->bias->tensor.shape.numberOfDimensions; i++) {
        biasGradsAsymOrderOfDims[i] = i;
    }
    getGradTensorFromParameter(linearConfig->bias, &biasGradsAsym, biasGradsAsymOrderOfDims);

    // ___________________________________________________________________________________

    // Get everything as SymInt32 tensors
    asymQConfig_t *weightsAsymQC = linearConfig->weights->tensor.quantization->qConfig;
    symInt32QConfig_t weightsSymInt32QC;
    initSymInt32QConfig(weightsAsymQC->roundingMode, &weightsSymInt32QC);

    quantization_t weightsSymInt32Q;
    initSymInt32Quantization(&weightsSymInt32QC, &weightsSymInt32Q);
    int32_t weightsIntData[numberOfWeights];
    tensor_t weightsSymInt32;
    setTensorValuesForConversion(weightsIntData, &weightsSymInt32Q, &weightsAsym, &weightsSymInt32);
    convertTensor(&weightsAsym, &weightsSymInt32);

    asymQConfig_t *forwardInputAsymQC = forwardInput->quantization->qConfig;
    symInt32QConfig_t forwardInputSymInt32QC;
    initSymInt32QConfig(forwardInputAsymQC->roundingMode, &forwardInputSymInt32QC);
    quantization_t forwardInputSymInt32Q;
    initSymInt32Quantization(&forwardInputSymInt32QC, &forwardInputSymInt32Q);

    int32_t forwardInputIntData[outputSize];
    tensor_t forwardInputSymInt32;
    setTensorValuesForConversion(forwardInputIntData, &forwardInputSymInt32Q, forwardInput,
                                 &forwardInputSymInt32);
    convertTensor(forwardInput, &forwardInputSymInt32);


    asymQConfig_t *lossAsymQC = loss->quantization->qConfig;
    symInt32QConfig_t lossSymInt32QC;
    initSymInt32QConfig(lossAsymQC->roundingMode, &lossSymInt32QC);
    quantization_t lossSymInt32Q;
    initSymInt32Quantization(&lossSymInt32QC, &lossSymInt32Q);

    int32_t lossSymInt32Data[numberOfLosses];
    tensor_t lossSymInt32;
    setTensorValuesForConversion(lossSymInt32Data, &lossSymInt32Q, loss, &lossSymInt32);
    convertTensor(loss, &lossSymInt32);


    asymQConfig_t *weightGradsAsymQC = linearConfig->weights->gradQuantization->qConfig;
    symInt32QConfig_t weightGradsSymInt32QC;
    initSymInt32QConfig(weightGradsAsymQC->roundingMode, &weightGradsSymInt32QC);
    quantization_t weightGradsSymInt32Q;
    initSymInt32Quantization(&weightGradsSymInt32QC, &weightGradsSymInt32Q);

    int32_t weightGradsSymInt32Data[numberOfWeights];
    tensor_t weightGradsSymInt32;
    setTensorValuesForConversion(
        weightGradsSymInt32Data, &weightGradsSymInt32Q, &weightGradsAsym,
        &weightGradsSymInt32);
    convertTensor(&weightGradsAsym, &weightGradsSymInt32);


    asymQConfig_t *biasGradsAsymQC = linearConfig->bias->gradQuantization->qConfig;
    symInt32QConfig_t biasGradsSymInt32QC;
    initSymInt32QConfig(biasGradsAsymQC->roundingMode, &biasGradsSymInt32QC);
    quantization_t biasGradsSymInt32Q;
    initSymInt32Quantization(&biasGradsSymInt32QC, &biasGradsSymInt32Q);

    int32_t biasGradsSymInt32Data[numberOfBiases];
    tensor_t biasGradsSymInt32;
    setTensorValuesForConversion(biasGradsSymInt32Data, &biasGradsSymInt32Q,
                                 &biasGradsAsym, &biasGradsSymInt32);
    convertTensor(&biasGradsAsym, &biasGradsSymInt32);

    asymQConfig_t *propLossAsymQC = propLossTensor->quantization->qConfig;
    symInt32QConfig_t propLossSymInt32QC;
    initSymInt32QConfig(propLossAsymQC->roundingMode, &propLossSymInt32QC);
    quantization_t propLossSymInt32Q;
    initSymInt32Quantization(&propLossSymInt32QC, &propLossSymInt32Q);

    int32_t propLossSymInt32Data[inputSize];
    tensor_t propLossSymInt32;
    setTensorValuesForConversion(propLossSymInt32Data, &propLossSymInt32Q,
                                 propLossTensor, &propLossSymInt32);
    convertTensor(propLossTensor, &propLossSymInt32);
    // ______________________________________________________________-

    // Weight gradients
    calcWeightGradsAsym(&lossSymInt32, &forwardInputSymInt32, &weightGradsSymInt32);
    convertTensor(&weightGradsSymInt32, &weightGradsAsym);

    weightGradsAsymQC->scale = weightsAsymQC->scale;


    memcpy(linearConfig->weights->grad, weightGradsAsym.data,
           numberOfWeights * calcBytesPerElement(linearConfig->weights->gradQuantization));

    // Bias gradients
    calcBiasGradsAsym(&biasGradsSymInt32, &lossSymInt32);

    // set scale to weight scale, because bias is int32 and the only remaining scale is weight scale
    biasGradsSymInt32QC.scale = weightsAsymQC->scale;

    convertTensor(&biasGradsSymInt32, &biasGradsAsym);

    memcpy(linearConfig->bias->grad, biasGradsAsym.data,
           numberOfBiases * calcBytesPerElement(linearConfig->bias->gradQuantization));

    // Propagated loss
    calcPropLossAsym(&weightsSymInt32, &lossSymInt32, &propLossSymInt32);
    convertTensor(&propLossSymInt32, propLossTensor);
}


void linearBackward(void *config, tensor_t *loss, tensor_t *output, tensor_t *propLoss) {
    linearConfig_t *lConfig = config;

    if (lConfig->qType == FLOATLAYER) {
        linearBackwardFloat(config, loss, output, propLoss);

    } else if (lConfig->qType == ASYMLAYER) {
        linearBackwardAsym(config, loss, output, propLoss);
    }
}

void calcOutputShapeLinear(layer_t *linearLayer, shape_t inputShape, shape_t *outputShape) {
    size_t numberOfDims = 2;
    linearConfig_t *linearConfig = linearLayer->layerConfig;
    parameter_t *bias = linearConfig->bias;
    outputShape->dimensions[0] = inputShape.dimensions[0];
    outputShape->dimensions[1] = bias->tensor.shape.dimensions[0];
    outputShape->numberOfDimensions = numberOfDims;
    setOrderOfDimsForNewTensor(numberOfDims, outputShape->orderOfDimensions);
}

void initLinearLayer(layer_t *layer, linearConfig_t *linearConfig) {
    layer->type = LINEAR;
    layer->layerConfig = linearConfig;
    layer->forward = linearForward;
    layer->backward = linearBackward;
    layer->calcOutputShape = calcOutputShapeLinear;
}
