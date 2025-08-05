#ifndef SGD_H
#define SGD_H
#include "AiHelpers.h"

// Momentum is same Size as Parameter struct
typedef struct momentumBuffer {
    void *parameter; // Pointer to ONE parameter struct: Bin mir gerade nicht sicher, ob das so mit dem * so muss
    float *momentums; // Array of momentums of size of parameter
} momentumBuffer_t;

typedef struct SGDConfig {
    float lr; // factor for learning rate
    float momentum; // factor for momentum
    float weightDecay; // factor to decrease the weight
    momentumBuffer_t *momentum_buffer; // array of momentum buffers
    size_t sizeMomentumBuffers; //number of elements in momentum buffers
} SGDConfig_t;

/*! @brief Initalizes the SGDConfig for a given array of layerForwardBackward with lr, momentum &
 * weightDecay
 *
 * @param model : Pointer to array of type layerForwardBackward_t
 * @param sizeModel : Size of the array
 * @param lr : learning rate
 * @param momentum : momentum factor
 * @param weightDecay : weight decay factor
 * @return : Pointer to SGDConfig
 */
SGDConfig_t *initSGDConfig(layerForwardBackward_t *model, size_t sizeModel, float lr, float momentum, float weightDecay);

/*! @brief SGD Step implementation
 * Perform all updates elementwise in this sequence. Also, see
 * https://docs.pytorch.org/docs/stable/generated/torch.optim.SGD.html
 * grad = grad + weightDecay * parameter
 * momentumBuffer = momentum * momentumBuffer + grad
 * parameter = parameter - lr * momentumBuffer
 *
 * @param SGDConfig : Pointer to SGDConfig.
 */
void SGDStep(SGDConfig_t *SGDConfig);

/*! @brief Zeros the gradients for a given SGDConfig
 *
 * @param SGDConfig : Pointer to SGDConfig
 */
void SGDZeroGrad(SGDConfig_t *SGDConfig);

/*! @brief initalizes the momentumBuffer for a given parameter
 *
 * @param parameter: pointer to parameter
 * @return : Pointer to momentumBuffer
 */
momentumBuffer_t *initMomentumBuffer(void *parameter);
#endif //SGD_H
