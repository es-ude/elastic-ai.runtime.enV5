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
 * @param[in] checksum checksum for data to send
 *
 * @returns pointer to a struct holding message and length
 *
 * @important call `freeMessageFrame(...)` to remove allocated memory of message after
 * send
 */
usbProtocolMessageFrame_t *createMessageFrame(uint8_t command, size_t payloadLength,
                                              uint8_t *payload, uint8_t checksum);

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

//! convert uin32_t to big endian encoded byte array
void convertUint32ToByteArray(uint32_t in, uint8_t out[4]);
//! convert a byte array to uint32_t
uint32_t convertByteArrayToUint32(const uint8_t in[4]);

//! read arbitrary number of bytes with read-handle
void readBytes(uint8_t *data, size_t numberOfBytes);

//! @brief calculate XOR based checksum of given message
uint8_t calculateChecksum(usbProtocolMessage_t *message);
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
bool checksumPassed(uint8_t expectedChecksum, usbProtocolMessage_t *message);

//! send ACK
void sendAck(void);

//! send NACK
void sendNack(void);

#endif // ENV5_TOOLS_HEADER
