#ifndef RELU_H
#define RELU_H
#include "AiHelpers.h"
#include "Quantization.h"

typedef struct ReLUConfig
{
} ReLUConfig_t;

typedef void(*reluForward_t)(uint8_t* rawBytes, size_t byteIndex, uint8_t* outputData, size_t bytesPerElement);
typedef void(*reluBackward_t)(uint8_t* rawBytes, uint8_t* gradBytes, size_t byteIndex, uint8_t* outputData, size_t bytesPerElement);


/*! @brief
 *
 * @return : Pointer to ReLUConfig
 */
ReLUConfig_t* initReLUConfig();

layerForward_t* initReLULayerForward();

layerForwardBackward_t* initReLULayerForwardBackward();

void freeReLUForward(layerForward_t* layer);

void freeReLUBackward(layerForwardBackward_t* layer);

/*! @brief Forward call for the ReLU layer
 *
 * @param config: gives the number of inputs
 * @param inputQTensor: pointer to input qTensor_t
 * @param outputQuantization: Pointer to wanted quantization of output
 * @return : calculates the output
 */
qTensor_t* ReLUForward(void* config, qTensor_t* inputQTensor, quantization_t *outputQuantization);

/*! @brief Backward call for the ReLU layer calculating the gradients in respect to the inputs
 *
 * @param config : gives the number of inputs
 * @param grad : partial gradients of loss function till here
 * @param input : input that was put into the layer
 * @return : partial gradients of loss function for the previous layers
 */
qTensor_t* ReLUBackward(void* config, qTensor_t* grad, qTensor_t* input);

#endif //RELU_H
