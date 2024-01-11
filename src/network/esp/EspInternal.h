#ifndef ENV5_ESP_INTERNAL_HEADER
#define ENV5_ESP_INTERNAL_HEADER

#include <stdbool.h>

/*! @brief check if ESP responds to commands */
bool espInternalCheckIsResponding(void);

/*! @brief sends reset command to ESP */
bool espInternalSoftReset(void);

#endif /* ENV5_ESP_INTERNAL_HEADER */
