#ifndef MSELOSSDOUTPUT_H
#define MSELOSSDOUTPUT_H
#include "AiHelpers.h"

/*! @brief Calculates the Derivative of the MSE Loss in respect to the output.
 *
 * @param output : tensor of outputs
 * @param label : tensor of labels
 * @return : derivative of MSE in respect to the output
 */
qTensor_t *MSELossBackward(qTensor_t *output, qTensor_t *label);


#endif // MSELOSSDOUTPUT_H
