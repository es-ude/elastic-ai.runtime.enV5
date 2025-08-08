#ifndef LINEAR_H
#define LINEAR_H

#include "AiHelpers.h"

typedef struct linearConfig{
    size_t inputSize;
    size_t outputSize;
    parameter_t *weight;
    parameter_t *bias;
} linearConfig_t;

/*! @brief Gets linearConfig_t for a given weight & bias
 *
 * @param weight : pointer to array of weight of linear layer
 * @param sizeWeights: number of weights in array
 * @param bias : pointer to array of bias of linear layer
 * @param sizeBias : number of bias in array
 * @return: pointer to linearConfig_t
 */
linearConfig_t *initLinearConfigWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias);

/*! @brief Return pointer to linearConfig_t with automatic weight and bias instantiation, likely glorot
 *
 * init Low Prio!
 * @param inputSize : input size of linear layer
 * @param outputSize : output size of linear layer
 * @return : pointer to linearConfig_t
 */
linearConfig_t *initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Returns pointer to layerForward_t
 *
 * @param weight : pointer to array of weights of the linear layer
 * @param sizeWeights: size of array of weights
 * @param bias : pointer to array of bias of the linear layer
 * @param sizeBias : size of array of weights
 * @return : pointer to layerForward_t
 */
layerForward_t *initLinearLayerForwardWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias);

/*! @brief Return point to layerForward_t for linear layer
 * linearConfig_t with automatic weight and bias instatiation, likely glorot init
 * Low Prio
 *
 * @param inputSize : inputSize of linear layer
 * @param outputSize : outputSize of linear layer
 * @return : pointer to layerForward_t
 */
layerForward_t *initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Returns pointer to layerForward_t for a given weight and bias
 *
 * @param weight : pointer to array of weights of the linear layer
 * @param sizeWeights: size of array of weights
 * @param bias : pointer to array of bias of the linear layer
 * @param sizeBias : size of array of weights
 * @return : pointer to layerForwardBackward_t
 */
layerForwardBackward_t *initLinearLayerBackwardWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias);

/*! @brief Return point to layerForward_t for linear layer
 * linearConfig_t with automatic weight and bias instatiation, likely glorot init
 * Low Prio
 *
 * @param inputSize : inputSize of linear layer
 * @param outputSize : outputSize of linear layer
 * @return : pointer to layerForwardBackward_t
 */
layerForwardBackward_t *initLinearLayerBackwardWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Forward call for linear layer
 *
 * @param config : Config of linear layer
 * @param input : inputs for the linear layer
 * @return : output of the linear layer
 */
float *linearForward(linearConfig_t *config, float *input);

/*! @brief Backward call for the linear layer that calculates the gradients in respect to the inputs
 * and in respect to the parameters
 *
 * @param config : Config of linear layer
 * @param grad : partial gradients of loss function till here
 * @param input : input that was put into the layer
 * @return : partial gradients of loss function for the previous layers
 */
float *linearBackward(linearConfig_t *config, float *grad, float *input);

#endif //LINEAR_H
