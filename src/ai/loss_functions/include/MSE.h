#ifndef MSELOSSDOUTPUT_H
#define MSELOSSDOUTPUT_H
#include "AiHelpers.h"

/*! @brief Calculates the Derivative of the MSE Loss in respect to the output.
 *
 * @param output : array of outputs
 * @param label : array of labels
 * @param size : size of outputs and labels
 * @return : derivative of MSE in respect to the output
 */
float *MSELossDOutput(float *output, float *label, size_t size);

#endif // MSELOSSDOUTPUT_H
