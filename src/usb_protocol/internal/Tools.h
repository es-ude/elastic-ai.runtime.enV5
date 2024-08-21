#ifndef ENV5_TOOLS_HEADER
#define ENV5_TOOLS_HEADER

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct usbProtocolMessageFrame {
    size_t length;
    uint8_t *data;
} usbProtocolMessageFrame_t;

extern usbProtocolReadData readHandle;
extern usbProtocolSendData sendHandle;

/*!
 * @brief creates a message to be sent
 *
 * @param[in] command command for the message
 * @param[in] payload payload to be sent
 * @param[in] payloadLength length of the payload to be sent
 *
 * @returns pointer to a struct holding message and length
 *
 * @important call `freeMessageFrame(...)` to remove allocated memory of message after
 * send
 */
usbProtocolMessageFrame_t *createMessageFrame(uint8_t command, size_t payloadLength,
                                              uint8_t *payload);

/*!
 * @brief clear protocol message buffer
 */
void freeMessageFrame(usbProtocolMessageFrame_t *buffer);

/*!
 * @brief wait to receive ack
 *
 * @retval true if ack received
 * @retval false else
 */
bool waitForAcknowledgement(void);

/*!
 * @brief method to evaluate checksum of arbitrary number of byte arrays
 *
 * @param[in] expectedChecksum expected checksum
 * @param[in] numberOfArguments number of arguments provided
 * @param[in] ... multiples of `uint8_t *data, size_t length`
 *
 * @retval true if checksum matches
 * @retval false else
 */
bool checksumPassed(uint8_t expectedChecksum, int numberOfArguments, ...);

//! convert a byte array to uint32_t
uint32_t convertBytes(const uint8_t data[4]);

//! read arbitrary number of bytes with read-handle
void readBytes(uint8_t *data, size_t numberOfBytes);

//! @brief calculate XOR based checksum of given byte arrays
uint8_t calculateChecksum(int numberOfArguments, va_list data);

//! @brief method to get checksum of arbitrary number of byte arrays
uint8_t getChecksum(int numberOfArguments, ...);

#endif // ENV5_TOOLS_HEADER
