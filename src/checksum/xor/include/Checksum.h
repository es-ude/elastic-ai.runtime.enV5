#ifndef ENV5_CHECKSUM_HEADER
#define ENV5_CHECKSUM_HEADER

#include <stdint.h>

/*!
 * @brief calculate XOR based checksum of two bytes
 *
 * @param[in,out] input1 byte 1 for the XOR
 * @param[in] input2 byte 2 for the XOR
 *
 * @returns result of the XOR operation
 *
 * @throws CHECKSUM_CALCULATION_EXCEPTION calculation failed
 */
uint8_t checksum(uint8_t input1, uint8_t input2);

#endif // ENV5_CHECKSUM_HEADER
