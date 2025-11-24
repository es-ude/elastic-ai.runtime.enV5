#include "Linear.h"
#include "Add.h"
#include "DTypes.h"
#include "Layer.h"
#include "Matmul.h"
#include "Rounding.h";
#include "TensorConversion.h"

#include <stdio.h>
#include <string.h>
#include <tgmath.h>

void linearInitConfig(linearConfig_t *linearConfig, parameter_t *weights, parameter_t *bias) {
    linearConfig->weights = weights;
    linearConfig->bias = bias;
}

static void forwardFloat32(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    transposeTensor(w, 0, 1);

    /*printf("Shape input:\n");
    printShape(input->shape);
    printf("\n");
    printf("Shape w:\n");
    printShape(w->shape);
    printf("\n");*/

    matmulFloat32Tensors(input, w, output);
    transposeTensor(w, 0, 1);
    addFloat32TensorsInplace(output, b);


}

static void forwardAsym(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {

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
    setTensorValuesForConversion(outputSymInt32Data, &outputSymInt32Q, output, &outputSymInt32);

    matmulSymInt32Tensors(&weightsSymInt32, &inputSymInt32, &outputSymInt32);

    addInt32TensorToSymInt32TensorInplace(&outputSymInt32, b);

    convertTensor(&outputSymInt32, output);
}

void linearForward(layer_t *linearLayer, tensor_t *input, tensor_t *output) {
    linearConfig_t *linearConfig = linearLayer->config->linear;

    tensor_t *weights = getTensorFromParameter(linearConfig->weights);

    tensor_t *bias = getTensorFromParameter(linearConfig->bias);

    if (linearLayer->qType == FLOAT_LAYER) {
        // TODO mismatched inputQ and layerQType
        forwardFloat32(weights, bias, input, output);
    } else if (linearLayer->qType == ASYM_LAYER) {

        forwardAsym(weights, bias, input, output);
    }
}

void linearCalcWeightGradsFloat32(tensor_t *forwardInput, tensor_t *loss, tensor_t *weightGrads) {
    //transposeTensor(forwardInput, 0, 1);
    /*printf("Shape forward input:\n");
    printShape(forwardInput->shape);
    printf("\n");

    printf("Shape loss:\n");
    printShape(loss->shape);
    printf("\n");*/

    matmulFloat32Tensors(loss, forwardInput,  weightGrads);
    //transposeTensor(forwardInput, 0, 1);
}

// Important: Batch size is not supported
void linearCalcBiasGradsFloat32(tensor_t *biasGrads, tensor_t *loss) {
    addFloat32TensorsInplace(biasGrads, loss);
}

void linearCalcPropLossFloat32(tensor_t *weights, tensor_t *loss, tensor_t *propLoss) {
    transposeTensor(loss, 0, 1);
    /*printf("Shape loss:\n");
    printShape(loss->shape);
    printf("\n");
    printf("Shape weights:\n");
    printShape(weights->shape);
    printf("\n");*/

    matmulFloat32Tensors(loss, weights, propLoss);
    transposeTensor(loss, 0, 1);
}

static void backwardFloat(linearConfig_t *linearConfig, tensor_t *forwardInput, tensor_t *loss,
                          tensor_t *propLossTensor) {
    size_t numberOfWeights =
        calcNumberOfElementsByShape(linearConfig->weights->param->shape);

    tensor_t *weightGrad = getGradTensorFromParameter(linearConfig->weights);

    tensor_t *biasGrad = getGradTensorFromParameter(linearConfig->bias);

    tensor_t intermediateWGrad;
    float intermediateWGradData[numberOfWeights];
    setTensorValues(&intermediateWGrad, intermediateWGradData, weightGrad->shape, weightGrad->quantization, weightGrad->sparsityBitmask);

    linearCalcWeightGradsFloat32(forwardInput, loss, &intermediateWGrad);
    addFloat32TensorsInplace(weightGrad, &intermediateWGrad);

    linearCalcBiasGradsFloat32(biasGrad, loss);

    tensor_t *weightData = getTensorFromParameter(linearConfig->weights);

    linearCalcPropLossFloat32(weightData, loss, propLossTensor);
}

void linearCalcWeightGradsAsym(tensor_t *loss, tensor_t *forwardInput, tensor_t *weightGrads) {
    transposeTensor(forwardInput, 0, 1);
    matmulSymInt32Tensors(loss, forwardInput, weightGrads);
    transposeTensor(forwardInput, 0, 1);
}

void linearCalcBiasGradsAsym(tensor_t *biasGrads, tensor_t *loss) {
    addSymInt32TensorsInplace(biasGrads, loss);
}

void linearCalcPropLossAsym(tensor_t *weights, tensor_t *loss, tensor_t *propLoss) {
    transposeTensor(weights, 0, 1);
    matmulSymInt32Tensors(weights, loss, propLoss);
    transposeTensor(weights, 0, 1);
}

static void backwardAsym(linearConfig_t *linearConfig, tensor_t *forwardInput, tensor_t *loss,
                         tensor_t *propLossTensor) {

    size_t outputSize = linearConfig->weights->param->shape->dimensions[0];
    size_t inputSize = linearConfig->weights->param->shape->dimensions[1];
    size_t numberOfWeights =
        calcNumberOfElementsByShape(linearConfig->weights->param->shape);
    size_t numberOfBiases =
        calcNumberOfElementsByShape(linearConfig->bias->param->shape);
    size_t numberOfLosses = calcNumberOfElementsByTensor(loss);

    // Get data and grad tensors from parameters
    tensor_t *weightsAsym = getTensorFromParameter(linearConfig->weights);
    tensor_t *weightGradsAsym = getGradTensorFromParameter(linearConfig->weights);

    tensor_t *biasGradsAsym = getGradTensorFromParameter(linearConfig->bias);
    // ___________________________________________________________________________________

    // Get everything as SymInt32 tensors
    tensor_t weightsSymInt32;
    asymQConfig_t *weightsAsymQC = linearConfig->weights->param->quantization->qConfig;
    symInt32QConfig_t weightsSymInt32QC;
    initSymInt32QConfig(weightsAsymQC->roundingMode, &weightsSymInt32QC);
    quantization_t weightsSymInt32Q;
    initSymInt32Quantization(&weightsSymInt32QC, &weightsSymInt32Q);
    int32_t weightsIntData[numberOfWeights];
    setTensorValuesForConversion(weightsIntData, &weightsSymInt32Q, weightsAsym, &weightsSymInt32);
    convertTensor(weightsAsym, &weightsSymInt32);

    tensor_t forwardInputSymInt32;
    asymQConfig_t *forwardInputAsymQC = forwardInput->quantization->qConfig;
    symInt32QConfig_t forwardInputSymInt32QC;
    initSymInt32QConfig(forwardInputAsymQC->roundingMode, &forwardInputSymInt32QC);
    quantization_t forwardInputSymInt32Q;
    initSymInt32Quantization(&forwardInputSymInt32QC, &forwardInputSymInt32Q);
    int32_t forwardInputIntData[outputSize];
    setTensorValuesForConversion(forwardInputIntData, &forwardInputSymInt32Q, forwardInput,
                                 &forwardInputSymInt32);
    convertTensor(forwardInput, &forwardInputSymInt32);

    tensor_t lossSymInt32;
    asymQConfig_t *lossAsymQC = loss->quantization->qConfig;
    symInt32QConfig_t lossSymInt32QC;
    initSymInt32QConfig(lossAsymQC->roundingMode, &lossSymInt32QC);
    quantization_t lossSymInt32Q;
    initSymInt32Quantization(&lossSymInt32QC, &lossSymInt32Q);
    int32_t lossSymInt32Data[numberOfLosses];
    setTensorValuesForConversion(lossSymInt32Data, &lossSymInt32Q, loss, &lossSymInt32);
    convertTensor(loss, &lossSymInt32);

    tensor_t weightGradsSymInt32;
    asymQConfig_t *weightGradsAsymQC = linearConfig->weights->grad->quantization->qConfig;
    symInt32QConfig_t weightGradsSymInt32QC;
    initSymInt32QConfig(weightGradsAsymQC->roundingMode, &weightGradsSymInt32QC);
    quantization_t weightGradsSymInt32Q;
    initSymInt32Quantization(&weightGradsSymInt32QC, &weightGradsSymInt32Q);
    int32_t weightGradsSymInt32Data[numberOfWeights];
    setTensorValuesForConversion(weightGradsSymInt32Data, &weightGradsSymInt32Q, weightGradsAsym,
                                 &weightGradsSymInt32);
    convertTensor(weightGradsAsym, &weightGradsSymInt32);

    tensor_t biasGradsSymInt32;
    asymQConfig_t *biasGradsAsymQC = linearConfig->bias->grad->quantization->qConfig;
    symInt32QConfig_t biasGradsSymInt32QC;
    initSymInt32QConfig(biasGradsAsymQC->roundingMode, &biasGradsSymInt32QC);
    quantization_t biasGradsSymInt32Q;
    initSymInt32Quantization(&biasGradsSymInt32QC, &biasGradsSymInt32Q);
    int32_t biasGradsSymInt32Data[numberOfBiases];
    setTensorValuesForConversion(biasGradsSymInt32Data, &biasGradsSymInt32Q, biasGradsAsym,
                                 &biasGradsSymInt32);
    convertTensor(biasGradsAsym, &biasGradsSymInt32);

    tensor_t propLossSymInt32;
    asymQConfig_t *propLossAsymQC = propLossTensor->quantization->qConfig;
    symInt32QConfig_t propLossSymInt32QC;
    initSymInt32QConfig(propLossAsymQC->roundingMode, &propLossSymInt32QC);
    quantization_t propLossSymInt32Q;
    initSymInt32Quantization(&propLossSymInt32QC, &propLossSymInt32Q);
    int32_t propLossSymInt32Data[inputSize];
    setTensorValuesForConversion(propLossSymInt32Data, &propLossSymInt32Q, propLossTensor,
                                 &propLossSymInt32);
    convertTensor(propLossTensor, &propLossSymInt32);
    // ______________________________________________________________-

    tensor_t intermediateWeightGradsSymInt32;
    int32_t intermediateWeightGradsData[numberOfWeights];
    setTensorValues(&intermediateWeightGradsSymInt32, intermediateWeightGradsData, weightGradsSymInt32.shape, weightGradsSymInt32.quantization, NULL);

    // Weight gradients
    linearCalcWeightGradsAsym(&lossSymInt32, &forwardInputSymInt32, &intermediateWeightGradsSymInt32);
    addSymInt32TensorsInplace(&weightGradsSymInt32, &intermediateWeightGradsSymInt32);
    convertTensor(&weightGradsSymInt32, weightGradsAsym);

    // Bias gradients
    linearCalcBiasGradsAsym(&biasGradsSymInt32, &lossSymInt32);
    biasGradsSymInt32QC.scale = weightsAsymQC->scale;
    convertTensor(&biasGradsSymInt32, biasGradsAsym);

    // Propagated loss
    linearCalcPropLossAsym(&weightsSymInt32, &lossSymInt32, &propLossSymInt32);
    convertTensor(&propLossSymInt32, propLossTensor);
}

void linearBackward(layer_t *linearLayer, tensor_t *forwardInput, tensor_t *loss,
                    tensor_t *propLoss) {

    linearConfig_t *lConfig = linearLayer->config->linear;

    if (linearLayer->qType == FLOAT_LAYER) {
        backwardFloat(lConfig, forwardInput, loss, propLoss);

    } else if (linearLayer->qType == ASYM_LAYER) {
        backwardAsym(lConfig, forwardInput, loss, propLoss);
    }
}

void linearCalcOutputShape(layer_t *linearLayer, shape_t *inputShape, shape_t *outputShape) {
    size_t batchSize = inputShape->dimensions[0];

    linearConfig_t *cfg = linearLayer->config->linear;
    shape_t *weightShape = cfg->weights->param->shape;
    size_t outFeatures = weightShape->dimensions[0];

    outputShape->dimensions[0] = outFeatures;
    outputShape->dimensions[1] = batchSize;

    outputShape->numberOfDimensions = inputShape->numberOfDimensions;

    setOrderOfDimsForNewTensor(inputShape->numberOfDimensions, outputShape->orderOfDimensions);
}
