#ifndef LINEAR_H
#define LINEAR_H

#include "AiHelpers.h"
#include "Quantization.h"

typedef struct linearConfig
{
    parameterQTensor_t* weight;
    parameterQTensor_t* bias;
} linearConfig_t;

typedef void(*linearForward_t)(linearConfig_t *linearConfig, qTensor_t *inputQTensor,  qTensor_t *outputQTensor, size_t i, size_t inputSize, size_t inputBytesPerElement);
typedef void(*linearBackward_t)(linearConfig_t *linearConfig, qTensor_t *inputQTensor, qTensor_t *gradTensor, qTensor_t *outputQTensor, size_t lossIndex, size_t inputSize);

qTensor_t *initLinearOutputQTensor(linearConfig_t *linearConfig, qTensor_t *inputQTensor, quantization_t *outputQuantization);

/*! @brief Gets linearConfig_t for a given weight & bias
 *
 * @param weightTensor : Pointer to tensor containing weights
 * @param biasTensor : Pointer to Tensor containing bias
 * @return: pointer to linearConfig_t
 */
linearConfig_t* initLinearConfigWithWeightBias(parameterQTensor_t* weightQTensor, parameterQTensor_t* biasQTensor);

/*! @brief Return pointer to linearConfig_t with automatic weight and bias instantiation, likely glorot
 *
 * init Low Prio!
 * @param inputSize : input size of linear layer
 * @param outputSize : output size of linear layer
 * @return : pointer to linearConfig_t
 */
linearConfig_t* initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Returns pointer to layerForward_t
 *
 * @param weightTensor : pointer to tensor of weights of the linear layer
 * @param biasTensor : pointer to tensor of bias of the linear layer
 * @return : pointer to layerForward_t
 */
layerForward_t* initLinearLayerForwardWithWeightBias(parameterQTensor_t* weightQTensor,
                                                     parameterQTensor_t* biasQTensor);
/*! @brief Return point to layerForward_t for linear layer
 * linearConfig_t with automatic weight and bias instatiation, likely glorot init
 * Low Prio
 *
 * @param inputSize : inputSize of linear layer
 * @param outputSize : outputSize of linear layer
 * @return : pointer to layerForward_t
 */
layerForward_t* initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Returns pointer to layerForwardBackward_t for a given weight and bias
 *
* @param weightTensor : pointer to tensor of weights of the linear layer
 * @param biasTensor : pointer to tensor of bias of the linear layer
 * @return : pointer to layerForwardBackward_t
 */
layerForwardBackward_t* initLinearLayerForwardBackwardWithWeightBias(parameterQTensor_t* weightQTensor, parameterQTensor_t* biasQTensor);

/*! @brief Return point to layerForward_t for linear layer
 * linearConfig_t with automatic weight and bias instatiation, likely glorot init
 * Low Prio
 *
 * @param inputSize : inputSize of linear layer
 * @param outputSize : outputSize of linear layer
 * @return : pointer to layerForwardBackward_t
 */
layerForwardBackward_t* initLinearLayerBackwardWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Forward call for linear layer
 *
 * @param config : Config of linear layer
 * @param inputTensor : Tensor with inputs for the linear layer
 * @return : output of the linear layer
 */
qTensor_t *linearForward(void *config, qTensor_t *inputQTensor, quantization_t *outputQuantization);

/*! @brief Backward call for the linear layer that calculates the gradients in respect to the inputs
 * and in respect to the parameters
 *
 * @param config : Config of linear layer
 * @param gradTensor : partial gradients of loss function till here
 * @param inputTensor : input that was put into the layer
 * @return : partial gradients of loss function for the previous layers
 */
qTensor_t *linearBackward(void *config, qTensor_t *lossQTensor, qTensor_t *outputQTensor,
                          quantization_t *outputQuantization);

#endif //LINEAR_H
