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
 *            call `waitForAcknowledgement()` after send!
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
    uint8_t command;
    size_t payloadLength;
    uint8_t *payload;
} usbProtocolMessage_t;

/* endregion TYPEDEFS */

/*!
 * @brief send a message
 *
 * @param[in] message buffer holding data for a message
 *
 * @returns status of acknowledgement
 * @retval true if ack received
 * @retval else if nack received
 */
bool usbProtocolSendMessage(usbProtocolMessage_t *message);

/*!
 * @brief read a whole message from sender
 *
 * @param[out] message buffer to store received message
 *
 * @returns result of checksum comparison
 * @retval true if checksum matches
 * @retval false else
 *
 * @throws USB_PROTOCOL_ERROR_READ_FAILED
 */
bool usbProtocolReadMessage(usbProtocolMessage_t *message);

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
