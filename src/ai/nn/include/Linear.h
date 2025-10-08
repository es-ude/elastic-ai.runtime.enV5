#ifndef LINEAR_H
#define LINEAR_H

#include "AiHelpers.h"

typedef struct linearConfig
{
    parameterTensor_t* weight;
    parameterTensor_t* bias;
} linearConfig_t;

tensor_t *initLinearOutputTensor(linearConfig_t *linearConfig, tensor_t *inputTensor);

/*! @brief Gets linearConfig_t for a given weight & bias
 *
 * @param weightTensor : Pointer to tensor containing weights
 * @param biasTensor : Pointer to Tensor containing bias
 * @return: pointer to linearConfig_t
 */
linearConfig_t* initLinearConfigWithWeightBias(parameterTensor_t* weightTensor, parameterTensor_t* biasTensor);

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
layerForward_t* initLinearLayerForwardWithWeightBias(parameterTensor_t* weightTensor,
                                                     parameterTensor_t* biasTensor);
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
layerForwardBackward_t* initLinearLayerForwardBackwardWithWeightBias(tensor_t* weightTensor, tensor_t* biasTensor);

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
tensor_t* linearForward(void* config, tensor_t* inputTensor);

/*! @brief Backward call for the linear layer that calculates the gradients in respect to the inputs
 * and in respect to the parameters
 *
 * @param config : Config of linear layer
 * @param gradTensor : partial gradients of loss function till here
 * @param inputTensor : input that was put into the layer
 * @return : partial gradients of loss function for the previous layers
 */
tensor_t* linearBackward(void* config, tensor_t* gradTensor, tensor_t* inputTensor);

#endif //LINEAR_H
