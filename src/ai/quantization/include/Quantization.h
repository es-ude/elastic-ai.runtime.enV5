#ifndef QUANTIZATION_H
#define QUANTIZATION_H
#include <stddef.h>
#include <stdint.h>

// todo find implementation for 16 bit data types
typedef uint16_t fixedPoint_t;
typedef uint16_t float16_t;
typedef float float32_t;
typedef double float64_t;
// TODO add linear_t (how big?)

/*! @brief Enum of possible quantization types
 */
typedef enum qtype
{
    FIXEDPOINT_Q,
    FLOAT16_Q,
    FLOAT32_Q,
    FLOAT64_Q,
    LINEAR_Q
} qtype_t;

/*! @brief Describes fixed point quantization
 * @param intBits:
 * @param fracBits:
 */
typedef struct fixedPointQ
{
    size_t signBit;
    size_t intBits;
    size_t fracBits;
} fixedPointQ_t;

/*! @brief Describes floating point quantization
 * @param mantissaBits:
 * @param exponentBits:
 */
typedef struct floatQ
{
    size_t signBit;
    size_t exponentBits;
    size_t mantissaBits;
} floatQ_t;

/*! @brief Describes linear quantization
 * @param scale:
 * @param zeroPoint:
 * @param qMin:
 * @param qMax:
 */
typedef struct linearQ
{
    float scale;
    int32_t zeroPoint;
    int32_t qMin; // IMMER n
    int32_t qMax;
} linearQ_t;


typedef struct quantization
{
    qtype_t type;
    void* qConfig;
} quantization_t;


typedef struct qTensor
{
    uint8_t* data;
    quantization_t* quantization;
    size_t numberOfDimensions;
    size_t* dimensions;
} qTensor_t;

typedef struct parameterQTensor
{
    qTensor_t* dataTensor;
    qTensor_t* gradTensor;
} parameterQTensor_t;

uint16_t read16Bits(uint8_t* startIndex);
float32_t readBytesAsFloat32(uint8_t* startIndex);
float64_t readBytesAsFloat64(uint8_t* startIndex);

quantization_t* initQuantizationByType(const qtype_t type);
size_t calcBytesPerElement(quantization_t* quantization);

qTensor_t* initQTensor(uint8_t* data, size_t numberOfDimensions, size_t* dimensions, quantization_t* quantization);
parameterQTensor_t* initParameterQTensor(uint8_t* data, size_t numberOfDimensions, size_t* dimensions,
                                         quantization_t* dataQuantization, quantization_t* gradQuantization);

int16_t reconstruct16(uint8_t* bytes);
#endif //QUANTIZATION_H
