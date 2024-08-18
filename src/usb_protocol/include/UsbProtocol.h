#ifndef ENV5_USB_PROTOCOL_HEADER
#define ENV5_USB_PROTOCOL_HEADER

#include <stddef.h>

#include "UsbProtocolTypedefs.h"

/*!
 * @brief initializes the usb protocol handler
 *
 * @code
 * void usbProtocolInit(
 *      usbProtocolReadCommand readFunction,
 *      usbProtocolSendData sendFunction
 * );
 * @endcode
 *
 * @param[in] readFunction function providing input for the protocol handler
 * @param[in] sendFunction function allowing the handler to send a response
 *
 * @throws USB_PROTOCOL_ERROR_NULL_POINTER a function provided is not defined
 */
void usbProtocolInit(usbProtocolReadData readFunction, usbProtocolSendData sendFunction);

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

/*!
 * @brief function waiting for data (command + payload + checksum) (BLOCKING)
 *
 * @returns pointer to buffer holding the received command and payload
 *
 * @throws USB_PROTOCOL_ERROR_CHECKSUM_FAILED checksum mismatch
 */
usbProtocolReceiveBuffer usbProtocolWaitForCommand(void);

/*!
 * @brief function handling the received command
 *
 * @param[in] buffer buffer with received data to be handled
 *
 * @throws USB_PROTOCOL_ERROR_NULL_POINTER if command id doesn't have a registered function
 * associated
 * @throws USB_PROTOCOL_ERROR_INVALID_ID if command id is not valid
 */
void usbProtocolHandleCommand(usbProtocolReceiveBuffer buffer);

#endif // ENV5_USB_PROTOCOL_HEADER
