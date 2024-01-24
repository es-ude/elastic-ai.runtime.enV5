#ifndef ENV5_MIDDLEWARE_HEADER
#define ENV5_MIDDLEWARE_HEADER

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

void middlewareInit();
void middlewareDeinit();

/* region MIDDLEWARE */

/*! \brief reconfigure the FPGA with the Middleware Multi-Boot featuer
 *  \ingroup FPGA
 *
 * @param address address where the binary file starts in the flash
 */
void middlewareConfigureFpga(uint32_t address);

void middlewareSetFpgaLeds(uint8_t leds);
uint8_t middlewareGetLeds(void);

void middlewareUserlogicEnable(void);
void middlewareUserlogicDisable(void);

/* endregion MIDDLEWARE */

/* region USER LOGIC / SKELETON */

/*! \brief send data to the FPGA
 *  \ingroup FPGA
 *
 * @param address address where the data is sent
 * @param data    data that is sent to the FPGA
 * @param length  size of the data to send in bytes
 */
void middlewareWriteBlocking(uint32_t address, uint8_t *data, size_t length);

/*! \brief read data from the FPGA
 *  \ingroup FPGA
 *
 * @param address address where the data should be read from
 * @param data    data that is read from the FPGA
 * @param length  size of the data to read in bytes
 */
void middlewareReadBlocking(uint32_t address, uint8_t *data, size_t length);

/*! \ingroup FPGA
 *
 * requires the enV5HWController function `env5HwFpgaInit()` to be called beforehand
 *
 * @return returns true if the computation is done
 */
bool middlewareUserlogicGetBusyStatus(void);

/* endregion USER LOGIC / SKELETON */

#endif // ENV5_MIDDLEWARE_HEADER
