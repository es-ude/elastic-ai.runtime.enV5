#ifndef ENV5_COMMON_H
#define ENV5_COMMON_H

#include <stdio.h>

#ifndef SOURCE_FILE
#define SOURCE_FILE "no Source file defined!"
#endif

/*!
 * \brief print log messages
 * \param str string to print in printf-format
 * \param ... replacements for string
 */
#define PRINT(str, ...)                                                                            \
    {                                                                                              \
        printf("[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                            \
        printf(str, ##__VA_ARGS__);                                                                \
        printf("\n");                                                                              \
    }

/*!
 * \brief print a byte array (uint8_t arrays)
 * \param prefix string to print before byte array
 * \param byteArray pointer to first byte of the array
 * \param numberOfBytes length of the byte array
 */
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
/*!
 * \brief print message only in DEBUG profile
 * \param str string to print in printf-format
 * \param ... replacements for string
 */
#define PRINT_DEBUG(str, ...)                                                                      \
    {                                                                                              \
        printf("\033[0;33m[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                  \
        printf(str, ##__VA_ARGS__);                                                                \
        printf("\033[0m\n");                                                                       \
    }

/*!
 * \brief print a byte array only in DEBUG profile
 * \param prefix string to print before byte array
 * \param byteArray pointer to first byte of the array
 * \param numberOfBytes length of the byte array
 */
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

#endif /* ENV5_COMMON_H */
