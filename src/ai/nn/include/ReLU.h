#ifndef RELU_H
#define RELU_H
#include "AiHelpers.h"

typedef struct ReLUConfig
{
    size_t size;
} ReLUConfig_t;

/*! @brief
 *
 * @param size : Number of values
 * @return : Pointer to ReLUConfig
 */
ReLUConfig_t* initReLUConfig(size_t size);

layerForward_t* initReLULayerForward();

layerForwardBackward_t* initReLULayerForwardBackward();

void freeReLUForward(layerForward_t* layer);

void freeReLUBackward(layerForwardBackward_t* layer);

/*! @brief Forward call for the ReLU layer
 *
 * @param config : gives the number of inputs
 * @param input : give the inputs
 * @return : calculates the output
 */
tensor_t* ReLUForward(void* config, tensor_t* inputTensor);

/*! @brief Backward call for the ReLU layer calculating the gradients in respect to the inputs
 *
 * @param config : gives the number of inputs
 * @param grad : partial gradients of loss function till here
 * @param input : input that was put into the layer
 * @return : partial gradients of loss function for the previous layers
 */
tensor_t* ReLUBackward(void* config, tensor_t* grad, tensor_t* input);

#endif //RELU_H
