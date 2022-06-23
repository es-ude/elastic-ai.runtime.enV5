#ifndef SENSOR_BOARD_COMMON_H
#define SENSOR_BOARD_COMMON_H

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

#ifdef DEBUG
#define PRINT_DEBUG(str, ...)                                                                      \
    {                                                                                              \
        printf("\033[0;33m[%s: %s] ", SOURCE_FILE, __FUNCTION__);                                  \
        printf(str, ##__VA_ARGS__);                                                                \
        printf("\033[0m\n");                                                                       \
    }
#else
#define PRINT_DEBUG(str, ...)                                                                      \
    {}
#endif

#define ASSERT(test)                                                                               \
    if (!(test))                                                                                   \
        return 0;

#endif // SENSOR_BOARD_COMMON_H
