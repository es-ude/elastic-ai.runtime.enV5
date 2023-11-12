#ifndef ENV5_COMMON_H
#define ENV5_COMMON_H

#include <stdio.h>

#ifndef SOURCE_FILE
#define SOURCE_FILE "no Source file defined!"
#endif

#define PRINT(str, ...)                                                                            \
    {                                                                                              \
        printf("[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                            \
        printf(str, ##__VA_ARGS__);                                                                \
        printf("\n");                                                                              \
    }

#define PRINT_BYTE_ARRAY(prefix, byteArray, numberOfBytes)                                         \
    {                                                                                              \
        printf("[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                            \
        printf(prefix);                                                                            \
        for (size_t index = 0; index < numberOfBytes; index++) {                                   \
            printf("0x%02X ", byteArray[index]);                                                   \
        }                                                                                          \
        printf("\n");                                                                              \
    }

#ifdef DEBUG_MODE
#define PRINT_DEBUG(str, ...)                                                                      \
    {                                                                                              \
        printf("\033[0;33m[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                  \
        printf(str, ##__VA_ARGS__);                                                                \
        printf("\033[0m\n");                                                                       \
    }
#define PRINT_BYTE_ARRAY_DEBUG(prefix, byteArray, numberOfBytes)                                   \
    {                                                                                              \
        printf("[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                            \
        printf(prefix);                                                                            \
        for (size_t index = 0; index < numberOfBytes; index++) {                                   \
            printf("0x%02X ", byteArray[index]);                                                   \
        }                                                                                          \
        printf("\n");                                                                              \
    }
#else
#define PRINT_DEBUG(str, ...)                                                                      \
    {}
#define PRINT_BYTE_ARRAY_DEBUG(prefix, byteArray, numberOfBytes)                                   \
    {}
#endif

#define ASSERT(test)                                                                               \
    if (!(test))                                                                                   \
        return 0;

#endif /* ENV5_COMMON_H */
