#include "Sequential.h"
#include "Layer.h"
#include "MSE.h"
#include "TensorConversion.h"

#include <Linear.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

// Important: For now the calloc abstraction layer is located in sequential, because it is not needed anywhere else
static uint8_t *reserveMemory(size_t numberOfBytes) {
    uint8_t *ptr = calloc(1, numberOfBytes);
    return ptr;
}

static void freeReservedMemory(uint8_t *ptr) {
    free(ptr);
}

static size_t calcBytesOutputData(quantization_t *outputQ, size_t numberOfValues) {
    switch (outputQ->type) {
    case FLOAT32:
        return numberOfValues * sizeof(float);
    case ASYM:
        size_t bitsPerElement = calcBitsPerElement(outputQ);
        return ceil((bitsPerElement * numberOfValues) / 8);
    default:
        return 0;
    }
}

void sequentialForward(layer_t **model, size_t numberOfLayers, tensor_t *input, tensor_t *output) {
    for (size_t i = 0; i < numberOfLayers; i++) {
        layer_t *currentLayer = model[i];

        layerType_t currentLayerType = currentLayer->type;

        size_t outputNumberOfDims = input->shape->numberOfDimensions;

        size_t sizeDims = outputNumberOfDims * sizeof(size_t);
        size_t *outDims = reserveMemory(sizeDims);
        size_t *outOrder = reserveMemory(sizeDims);

        shape_t outShape = {.dimensions = outDims,
                            .numberOfDimensions = outputNumberOfDims,
                            .orderOfDimensions = outOrder};

        calcOutputShapeFn_t calcOutputShape = layerFunctions[currentLayerType].calcOutputShape;
        calcOutputShape(currentLayer, input->shape, &outShape);

        size_t numValues =
            calcNumberOfElementsByShape(&outShape);

        size_t sizeData = calcBytesOutputData(currentLayer->outputQ, numValues);
        uint8_t *data = reserveMemory(sizeData);
        uint8_t *sparsityBitmask = reserveMemory(numValues);

        tensor_t intermediateOutput;
        setTensorValues(&intermediateOutput, data, &outShape, input->quantization, sparsityBitmask);

        forwardFn_t forward = layerFunctions[currentLayerType].forward;
        forward(currentLayer, input, &intermediateOutput);

        if (i == numberOfLayers - 1) {
            copyTensor(output, &intermediateOutput);

            freeReservedMemory(outDims);
            freeReservedMemory(outOrder);
            freeReservedMemory(data);
            freeReservedMemory(sparsityBitmask);

            break;
        }

        copyTensor(input, &intermediateOutput);

        freeReservedMemory(outDims);
        freeReservedMemory(outOrder);
        freeReservedMemory(data);
        freeReservedMemory(sparsityBitmask);
    }
}

void getLossFunctionByType(lossFunctionType_t lossType, lossFn_t *lossFunction) {
    switch (lossType) {
    case MSE:
        *lossFunction = MSELossBackward;
        break;
    case CROSS_ENTROPY:
        // lossFunction = crossEntropySoftmaxBackward;
        break;
    default:
        printf("Loss type not found");
        break;
    }
}

static void initLayerOutputs(tensor_t **layerOutputs, layer_t **model, size_t sizeNetwork) {
    for (size_t i = 0; i < sizeNetwork; i++) {
        layer_t *currentLayer = model[i];
        layerType_t currentLayerType = currentLayer->type;
        calcOutputShapeFn_t calcOutputShape = layerFunctions[currentLayerType].calcOutputShape;
        size_t numberOfDims = layerOutputs[i]->shape->numberOfDimensions;

        size_t *dims = reserveMemory(numberOfDims * sizeof(size_t));
        size_t *order = reserveMemory(numberOfDims * sizeof(size_t));

        shape_t *outShape = reserveMemory(sizeof(shape_t));
        outShape->dimensions = dims;
        outShape->numberOfDimensions = numberOfDims;
        outShape->orderOfDimensions = order;

        calcOutputShape(currentLayer, layerOutputs[i]->shape, outShape);

        size_t numberOfValues = calcNumberOfElementsByShape(outShape);
        size_t sizeData = calcBytesOutputData(currentLayer->outputQ, numberOfValues);
        uint8_t *data = reserveMemory(sizeData);
        uint8_t *sparsityBitmask = reserveMemory(numberOfValues);

        tensor_t *tensor = reserveMemory(sizeof(tensor_t));
        tensor->data = data;
        tensor->quantization = currentLayer->outputQ;
        tensor->shape = outShape;
        tensor->sparsityBitmask = sparsityBitmask;

        layerOutputs[i + 1] = tensor;
    }
}

static void initGrads(tensor_t **grads, tensor_t **layerOutputs, size_t sizeNetwork) {
    for (size_t i = 0; i <= sizeNetwork; i++) {
        shape_t *currentShape = layerOutputs[i]->shape;
        quantization_t *currentQ = layerOutputs[i]->quantization;

        size_t *dims = reserveMemory(currentShape->numberOfDimensions * sizeof(size_t));
        size_t *order = reserveMemory(currentShape->numberOfDimensions * sizeof(size_t));
        shape_t *inShape = reserveMemory(sizeof(shape_t));
        inShape->dimensions = dims;
        inShape->numberOfDimensions = currentShape->numberOfDimensions;
        inShape->orderOfDimensions = order;

        memcpy(inShape->dimensions, currentShape->dimensions, currentShape->numberOfDimensions * sizeof(size_t));
        memcpy(inShape->orderOfDimensions, currentShape->orderOfDimensions, currentShape->numberOfDimensions * sizeof(size_t));

        setOrderOfDimsForNewTensor(inShape->numberOfDimensions, inShape->orderOfDimensions);

        size_t numberOfValues = calcNumberOfElementsByShape(currentShape);
        size_t sizeData = calcBytesOutputData(currentQ, numberOfValues);
        uint8_t *data = reserveMemory(sizeData);
        uint8_t *sparsityBitmask = reserveMemory(sizeData);

        tensor_t *tensor = reserveMemory(sizeof(tensor_t));
        tensor->data = data;
        tensor->quantization = currentQ;
        tensor->shape = inShape;
        tensor->sparsityBitmask = sparsityBitmask;

        grads[i] = tensor;
    }
}

static void freeShape(tensor_t* tensor) {
    freeReservedMemory(tensor->shape->dimensions);
    freeReservedMemory(tensor->shape->orderOfDimensions);
    freeReservedMemory(tensor->shape);
}

static void freeData(tensor_t *tensor) {
    freeReservedMemory(tensor->data);
    freeReservedMemory(tensor->sparsityBitmask);
}

static void freeTensorPointer(tensor_t *tensor) {
    freeReservedMemory(tensor);
}

static void deInitTensorPtrArray(tensor_t** tensorPtrArray, size_t sizeNetwork, size_t startIndex) {
    for(size_t i = startIndex; i <= sizeNetwork; i++) {
        freeShape(tensorPtrArray[i]);
        freeData(tensorPtrArray[i]);
        freeTensorPointer(tensorPtrArray[i]);
    }
}



/*! IMPORTANT: We assume, that if you use Cross Entropy as your loss function,
 * you also use Softmax with it. We introduce Softmax as a dedicated Layer,
 * but in the backward pass it is ignored. We do this, because the Cross Entropy Backward
 * already takes the Softmax Backward into account.
 */
void sequentialCalculateGrads(layer_t **model, size_t sizeNetwork,
                              lossFunctionType_t lossFunctionType, tensor_t *input, tensor_t *label,
                              trainingStats_t *trainingStats) {

    tensor_t *layerOutputs[sizeNetwork + 1];
    layerOutputs[0] = input;
    initLayerOutputs(layerOutputs, model, sizeNetwork);

    // Forward pass
    for (size_t i = 0; i < sizeNetwork; i++) {
        layer_t *currentLayer = model[i];
        layerType_t currentLayerType = currentLayer->type;
        forwardFn_t forward = layerFunctions[currentLayerType].forward;
        forward(currentLayer, layerOutputs[i], layerOutputs[i + 1]);
    }

    copyTensor(trainingStats->output, layerOutputs[sizeNetwork]);

    // LOSS
    lossFn_t lossFn;
    getLossFunctionByType(lossFunctionType, &lossFn);

    tensor_t *grads[sizeNetwork + 1];
    initGrads(grads, layerOutputs, sizeNetwork);

    lossFn(layerOutputs[sizeNetwork], label, grads[sizeNetwork]);
    copyTensor(trainingStats->loss, grads[sizeNetwork]);

    // Backward pass
    size_t backwardIndex = sizeNetwork - 1;
    if (lossFunctionType == CROSS_ENTROPY) {
        backwardIndex -= 1;
    }


    for (int i = (int)backwardIndex; i >= 0; i--) {
        layerType_t layerType = model[i]->type;
        backwardFn_t backward = layerFunctions[layerType].backward;
        backward(model[i], layerOutputs[i], grads[i+1], grads[i]);
    }

    deInitTensorPtrArray(layerOutputs, sizeNetwork, 1);
    deInitTensorPtrArray(grads, sizeNetwork, 0);
}
