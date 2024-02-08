#ifndef ENV5_COMMON_H
#define ENV5_COMMON_H

#include <stdbool.h>
#include <stdio.h>

#ifndef SOURCE_FILE
#define SOURCE_FILE "no Source file defined!"
#endif

#ifdef DEBUG_MODE
#define DEBUG_MODE_ON true
#else
#define DEBUG_MODE_ON false
#endif

/*!
 * \brief print log messages
 * \param str string to print in printf-format
 * \param ... replacements for string
 */
#define PRINT(str, ...)                                                                            \
    do {                                                                                           \
        printf("[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                            \
        printf(str, ##__VA_ARGS__);                                                                \
        printf("\n");                                                                              \
    } while (false)

/*!
 * \brief print a byte array (uint8_t arrays);
 * \param prefix string to print before byte array
 * \param byteArray pointer to first byte of the array
 * \param numberOfBytes length of the byte array
 */
#define PRINT_BYTE_ARRAY(prefix, byteArray, numberOfBytes)                                         \
    do {                                                                                           \
        printf("[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                            \
        printf(prefix);                                                                            \
        for (size_t index = 0; index < numberOfBytes; index++) {                                   \
            printf("0x%02X ", byteArray[index]);                                                   \
        }                                                                                          \
        printf("\n");                                                                              \
    } while (false)

/*!
 * \brief print message only in DEBUG profile
 * \param str string to print in printf-format
 * \param ... replacements for string
 */
#define PRINT_DEBUG(str, ...)                                                                      \
    do {                                                                                           \
        if (DEBUG_MODE_ON) {                                                                       \
            printf("\033[0;33m[%s: %s] ", SOURCE_FILE, __FUNCTION__);                              \
            printf(str, ##__VA_ARGS__);                                                            \
            printf("\033[0m\n");                                                                   \
        }                                                                                          \
    } while (false)

/*!
 * \brief print a byte array only in DEBUG profile
 * \param prefix string to print before byte array
 * \param byteArray pointer to first byte of the array
 * \param numberOfBytes length of the byte array
 */
#define PRINT_BYTE_ARRAY_DEBUG(prefix, byteArray, numberOfBytes)                                   \
    do {                                                                                           \
        if (DEBUG_MODE_ON) {                                                                       \
            printf("[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                        \
            printf(prefix);                                                                        \
            for (size_t index = 0; index < numberOfBytes; index++) {                               \
                printf("0x%02X ", byteArray[index]);                                               \
            }                                                                                      \
            printf("\n");                                                                          \
        }                                                                                          \
    } while (false)

#endif /* ENV5_COMMON_H */
