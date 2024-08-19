#ifndef ENV5_USB_PROTOCOL_CUSTOM_COMMANDS_HEADER
#define ENV5_USB_PROTOCOL_CUSTOM_COMMANDS_HEADER

#include <stdbool.h>
#include <stddef.h>

#include "UsbProtocolBase.h"

/* region TYPEDEFS */

/*!
 * @brief function providing the prototype for new command handle
 *
 * @important use `sendHandle` and `readHandle` to send/receive data @n
 *            call `usbProtocolWaitForAcknowledgement()` after send!
 *
 * @catuion byte order on RP2040="Little Endian", byte order for protocol="Big Endian":
 *          convert numbers with `__builtin_bswap32(uint32_t input)` or `__builtin_bswap16(uint16_t
 * input)`
 *
 * @param[in] payload pointer to the buffer holding the function payload;
 * @param[in] payloadLength size of @p data in Bytes
 */
typedef void (*usbProtocolCommandHandle)(const uint8_t *payload, size_t payloadLength);

typedef struct usbProtocolMessage {
    size_t length;
    uint8_t *message;
} usbProtocolMessage_t;

/* endregion TYPEDEFS */

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
 * @important call `usbProtocolFreeMessageBuffer(...)` to remove allocated memory of message after
 * send
 */
usbProtocolMessage_t *usbProtocolCreateMessage(uint8_t command, uint8_t *payload,
                                               uint32_t payloadLength);

/*!
 * @brief clear protocol message buffer
 */
void usbProtocolFreeMessageBuffer(usbProtocolMessage_t *buffer);

/*!
 * @brief wait to receive ack
 *
 * @retval true if ack received
 * @retval false else
 */
bool usbProtocolWaitForAcknowledgement(void);

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
bool usbProtocolChecksumPassed(uint8_t expectedChecksum, int numberOfArguments, ...);

/*!
 * @brief register a new command
 *
 * @code
 * void usbProtocolRegisterCommand(
 *      size_t identifier,
 *      usbProtocolCommandHandle command
 * );
 * @endcode
 *
 * @param[in]identifier identifier in range 128-255
 * @param[in]command pointer to function handling the command
 *
 * @throws USB_PROTOCOL_ERROR_INVALID_ID @p identifier out of range
 * @throws USB_PROTOCOL_ERROR_NULL_POINTER command is a null pointer
 */
void usbProtocolRegisterCommand(size_t identifier, usbProtocolCommandHandle command);

/*!
 * @brief unregister a new command
 *
 * @code
 * void usbProtocolRegisterCommand(
 *      size_t identifier,
 *      usbProtocolCommandHandle command
 * );
 * @endcode
 *
 * @param[in]identifier identifier in range 128-255
 *
 * @throws USB_PROTOCOL_ERROR_INVALID_ID @p identifier out of range
 */
void usbProtocolUnregisterCommand(size_t identifier);

#endif // ENV5_USB_PROTOCOL_CUSTOM_COMMANDS_HEADER
