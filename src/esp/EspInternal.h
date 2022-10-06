#ifndef ENV5_ESP_INTERNAL_HEADER
#define ENV5_ESP_INTERNAL_HEADER

#include <stdbool.h>

/*! \brief check if ESP responds to commands */
bool checkIsResponding(void);

/*! \brief sends reset command to ESP */
bool softReset(void);

#endif /* ENV5_ESP_INTERNAL_HEADER */
