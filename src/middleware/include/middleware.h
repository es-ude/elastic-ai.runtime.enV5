#ifndef ENV5_MIDDLEWARE_HEADER
#define ENV5_MIDDLEWARE_HEADER

#include <stdint.h>
#include <stdbool.h>

void middlewareInit();
void middlewareDeinit();

/*! \brief configure FPGA
 *  \ingroup FPGA
 *
 * @param address address where the binary file starts in the flash
 */
void middlewareConfigureFpga(uint32_t address);

/*! \brief send data to the FPGA
 *  \ingroup FPGA
 *
 * @param address address where the data is sent
 * @param data    data that is sent to the FPGA
 * @param length  size of the data to send in bytes
 */
void middlewareWriteBlocking(uint32_t address, uint8_t *data, uint16_t length);
/*! \brief read data from the FPGA
 *  \ingroup FPGA
 *
 * @param address address where the data should be read from
 * @param data    data that is read from the FPGA
 * @param length  size of the data to read in bytes
 * @return
 */
uint8_t middlewareReadBlocking(uint32_t address, uint8_t *data, uint16_t length);

void middlewareSetFpgaLeds(uint8_t leds);
uint8_t middlewareGetLeds(void);
uint8_t middlewareGetDesignId(void);

void middlewareUserlogicEnable(void);
void middlewareUserlogicDisable(void);
bool middlewareUserlogicGetBusyStatus(void);

#endif // ENV5_MIDDLEWARE_HEADER
