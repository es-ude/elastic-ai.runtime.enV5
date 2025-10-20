//
// Created by Leo Buron on 20.10.25.
//

#ifndef ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
#define ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
#include <stddef.h>
#include <stdint.h>
typedef enum dtype
{
    INT32,
    FLOAT32,
    LINEAR
} dtype_t;

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


typedef struct encoding
{
    dtype_t type;
    void* qConfig;
} encoding_t;



#endif // ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
